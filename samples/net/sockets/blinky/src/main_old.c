/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mqtt-sn/mqttsn_client.h"
#include <zephyr.h>
#include <stdio.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <string.h>


#include <openthread/config.h>
#include <openthread/dataset_ftd.h>
#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/tasklet.h>
#include <openthread/instance.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <openthread/cli_config.h>

static void mqttsn_init(otInstance *aInstance);
void mqttsn_evt_handler(mqttsn_client_t * p_client, mqttsn_event_t * p_event);
static void gateway_info_callback(mqttsn_event_t * p_event);
static void connected_callback(void);
static void disconnected_callback(void);
static void regack_callback(mqttsn_event_t * p_event);
static void received_callback(mqttsn_event_t * p_event);
static void timeout_callback(mqttsn_event_t * p_event);
static void searchgw_timeout_callback(mqttsn_event_t * p_event);
static void subscribe();
static void connect_opt_init(void);
//static void publish(void);

static void setNetworkConfiguration(otInstance *aInstance);
void handleNetifStateChanged(uint32_t aFlags, void *aContext);
#ifndef OPENTHREAD_ENABLE_COVERAGE
#define OPENTHREAD_ENABLE_COVERAGE 0
#endif

#define SEARCH_GATEWAY_TIMEOUT 5                                            /**< MQTT-SN Gateway discovery procedure timeout in [s]. */

#define SCHED_QUEUE_SIZE       32                                           /**< Maximum number of events in the scheduler queue. */
#define SCHED_EVENT_DATA_SIZE  APP_TIMER_SCHED_EVENT_DATA_SIZE              /**< Maximum app_scheduler event size. */

static mqttsn_client_t      m_client;                                       /**< An MQTT-SN client instance. */
static mqttsn_remote_t      m_gateway_addr;                                 /**< A gateway address. */
static uint8_t              m_gateway_id;                                   /**< A gateway ID. */
static mqttsn_connect_opt_t m_connect_opt;                                  /**< Connect options for the MQTT-SN client. */
//static uint8_t              m_led_state        = 0;                         /**< Previously sent BSP_LED_2 command. */
static uint16_t             m_msg_id           = 0;                         /**< Message ID thrown with MQTTSN_EVENT_TIMEOUT. */
static char                 m_client_id[]      = "nRF52840";      /**< The MQTT-SN Client's ID. */
static char                 m_topic_name_pub[]     = "nRF52840/data"; /**< Name of the topic corresponding to subscriber's BSP_LED_2. */
static mqttsn_topic_t       m_topic_pub            =                            /**< Topic corresponding to subscriber's BSP_LED_2. */
{
    .p_topic_name = (unsigned char *)m_topic_name_pub,
    .topic_id     = 0,
};

/////////////////////////////////////////////////////
static char                 m_topic_name_sub[]     = "nRF52840/cmd"; /**< Name of the topic corresponding to subscriber's BSP_LED_2. */
static mqttsn_topic_t       m_topic_sub            =                            /**< Topic corresponding to subscriber's BSP_LED_2. */
{
    .p_topic_name = (unsigned char *)m_topic_name_sub,
    .topic_id     = 0,
};

static bool g_sub_registered = false;
static bool g_led_2_on = false;

int main(int argc, char *argv[])
{
    otSysInit(argc, argv);
    otInstance *instance;

    printf("begin to connect!!\n");


    instance = otInstanceInitSingle();
    printf("single instance!!\n");


    otSetStateChangedCallback(instance, handleNetifStateChanged, instance);
    
    setNetworkConfiguration(instance);

    otIp6SetEnabled(instance, true);
    printf("ifconfig up!!\n");

    otThreadSetEnabled(instance, true);

    printf("thread start!!\n");

    mqttsn_init(instance);
   
    while (1)
    {
        otTaskletsProcess(instance);
        otSysProcessDrivers(instance);
    }

    otInstanceFinalize(instance);


    return 0;
}

void handleNetifStateChanged(uint32_t aFlags, void *aContext)
{
   if ((aFlags & OT_CHANGED_THREAD_ROLE) != 0)
   {
       otDeviceRole changedRole = otThreadGetDeviceRole(aContext);

       switch (changedRole)
       {
       case OT_DEVICE_ROLE_LEADER:
           printf("Leader\n");
           break;

       case OT_DEVICE_ROLE_ROUTER:
           printf("Router\n");
           break;
       
       case OT_DEVICE_ROLE_CHILD:
           printf("Child\n");
           break;
       
       case OT_DEVICE_ROLE_DETACHED:
           printf("Detached\n");
	   break;
       case OT_DEVICE_ROLE_DISABLED:
           /* Clear LED4 if Thread is not enabled. */
           printf("Disable\n");
           break;
        }
    }

}

/**
 * Override default network settings, such as panid, so the devices can join a network
 */
void setNetworkConfiguration(otInstance *aInstance)
{
    static char          aNetworkName[] = "OpenThread";
    otOperationalDataset aDataset;

    memset(&aDataset, 0, sizeof(otOperationalDataset));
   
    /*
     * Fields that can be configured in otOperationDataset to override defaults:
     *     Network Name, Mesh Local Prefix, Extended PAN ID, PAN ID, Delay Timer,
     *     Channel, Channel Mask Page 0, Network Master Key, PSKc, Security Policy
     */
    aDataset.mActiveTimestamp                      = 1;
    aDataset.mComponents.mIsActiveTimestampPresent = true;
     
    /* Set Channel to 15 */
    aDataset.mChannel                      = 25;
    aDataset.mComponents.mIsChannelPresent = true;
    
    /* Set Pan ID to 2222 */
    aDataset.mPanId                      = (otPanId)0xFACE;
    aDataset.mComponents.mIsPanIdPresent = true;

    /* Set Extended Pan ID to C0DE1AB5C0DE1AB5 */
    //uint8_t extPanId[OT_EXT_PAN_ID_SIZE] = {0xDE, 0xAD, 0x00, 0xBE, 0xEF, 0x00, 0xCA, 0xFE};
    //memcpy(aDataset.mExtendedPanId.m8, extPanId, sizeof(aDataset.mExtendedPanId));
    //aDataset.mComponents.mIsExtendedPanIdPresent = true;
    
    /* Set master key to 1234C0DE1AB51234C0DE1AB51234C0DE */
    //uint8_t key[OT_MASTER_KEY_SIZE] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    //memcpy(aDataset.mMasterKey.m8, key, sizeof(aDataset.mMasterKey));
    //aDataset.mComponents.mIsMasterKeyPresent = true;

    /* Set Network Name to OTCodelab */
    size_t length = strlen(aNetworkName);
    memcpy(aDataset.mNetworkName.m8, aNetworkName, length);
    aDataset.mComponents.mIsNetworkNamePresent = true;

    otDatasetSetActive(aInstance, &aDataset);

    /*otLinkModeConfig mode;
    memset(&mode, 0, sizeof(mode));
    mode.mRxOnWhenIdle       = true; 
    mode.mSecureDataRequests = false;
    mode.mDeviceType         = true;
    mode.mNetworkData        = true;


    otThreadSetLinkMode(aInstance, mode);*/
    
    
    /* Set the router selection jitter to override the 2 minute default.
       CLI cmd > routerselectionjitter 20
       Warning: For demo purposes only - not to be used in a real product */
    // jitterValue = 20;
    //otThreadSetRouterSelectionJitter(aInstance, jitterValue);

}

static void mqttsn_init(otInstance *aInstance)
{
    printf("MQTT-SN initing\n");
    uint32_t error_code=mqttsn_client_init(&m_client,
                                           MQTTSN_DEFAULT_CLIENT_PORT,
                                           mqttsn_evt_handler,
                                           aInstance);

    connect_opt_init();
}

void mqttsn_evt_handler(mqttsn_client_t * p_client, mqttsn_event_t * p_event)
{
    switch(p_event->event_id)
    {
        case MQTTSN_EVENT_GATEWAY_FOUND:
	    printf("Getting gateway info\n");
            gateway_info_callback(p_event);
            break;

        case MQTTSN_EVENT_CONNECTED:
            printf("Getting connected\n");
            connected_callback();
            break;

        case MQTTSN_EVENT_DISCONNECT_PERMIT:
            printf("Getting disconnected\n");
            disconnected_callback();
            break;

        case MQTTSN_EVENT_REGISTERED:
            printf("Registering event\n");
            regack_callback(p_event);
            break;

        case MQTTSN_EVENT_PUBLISHED:
            break;

	case MQTTSN_EVENT_SUBSCRIBED:
            break;

	case MQTTSN_EVENT_UNSUBSCRIBED:
            break;

        case MQTTSN_EVENT_RECEIVED:
            printf("Receiving event\n");
            received_callback(p_event);
            break;

        case MQTTSN_EVENT_TIMEOUT:
	    printf("Event Timing out\n");
            timeout_callback(p_event);
            break;

        case MQTTSN_EVENT_SEARCHGW_TIMEOUT:
            printf("Searching gateway timing out\n");
            searchgw_timeout_callback(p_event);
            break;

        default:
            break;
    }
}
static void gateway_info_callback(mqttsn_event_t * p_event)
{
    m_gateway_addr = *(p_event->event_data.connected.p_gateway_addr);
    m_gateway_id   = p_event->event_data.connected.gateway_id;
}
static void connected_callback(void)
{
    printf("Connected to Broker\n");

    uint32_t error_code=mqttsn_client_topic_register(&m_client,
                                                     m_topic_pub.p_topic_name,
                                                     strlen(m_topic_name_pub),
                                                     &m_msg_id);
}
static void disconnected_callback(void)
{
    printf("Disconnected to Broker\n");
}
static void regack_callback(mqttsn_event_t * p_event)
{
    printf("MQTT-SN event: Topic has been registered with ID: %d.\r\n",
                 p_event->event_data.registered.packet.topic.topic_id);
    
    if (!g_sub_registered)
    {   
        m_topic_pub.topic_id = p_event->event_data.registered.packet.topic.topic_id;

        g_sub_registered = true;

        uint32_t error_code=mqttsn_client_topic_register(&m_client,
                                                     m_topic_sub.p_topic_name,
                                                     strlen(m_topic_name_sub),
                                                     &m_msg_id);

    }
    else 
    {
        // store id
        m_topic_sub.topic_id = p_event->event_data.registered.packet.topic.topic_id;
        // subscribe
        subscribe();
    }

}
static void received_callback(mqttsn_event_t * p_event)
{
    if (p_event->event_data.published.packet.topic.topic_id == m_topic_sub.topic_id)
    {
        uint8_t* p_data = p_event->event_data.published.p_payload;


        // turn LEDs on/off
        if (p_data[0] == '1') {
            //LEDS_ON(BSP_LED_2_MASK);
            printf("LED 1 ON\n");
            g_led_2_on = true;
        }
        else {
            //LEDS_OFF(BSP_LED_2_MASK);
            printf("LED 1 OFF\n");
            g_led_2_on = false;
        }
    }
    else
    {
        printf("MQTT-SN event: Content to unsubscribed topic received. Dropping packet.\r\n");
    }
}
static void timeout_callback(mqttsn_event_t * p_event)
{
    printf("Time out\n");
}
static void searchgw_timeout_callback(mqttsn_event_t * p_event)
{
    printf("MQTT-SN event: Gateway discovery result: 0x%x.\r\n", p_event->event_data.discovery);
    if (p_event->event_data.discovery == 0)
    {
        //LEDS_ON(BSP_LED_2_MASK);
        printf("LED 1 ON\n");
        g_led_2_on = true;
    }
}
static void subscribe()
{
    uint8_t  topic_name_len = strlen(m_topic_name_sub);
    uint32_t error_code=mqttsn_client_subscribe(&m_client, 
        m_topic_sub.p_topic_name, topic_name_len, &m_msg_id);
    printf("Subscribed\n");
}
static void connect_opt_init(void)
{
    m_connect_opt.alive_duration = MQTTSN_DEFAULT_ALIVE_DURATION,
    m_connect_opt.clean_session  = MQTTSN_DEFAULT_CLEAN_SESSION_FLAG,
    m_connect_opt.will_flag      = MQTTSN_DEFAULT_WILL_FLAG,
    m_connect_opt.client_id_len  = strlen(m_client_id),

    memcpy(m_connect_opt.p_client_id, (unsigned char *)m_client_id, m_connect_opt.client_id_len);
}
/*static void publish(void)
{
    char* pub_data = "hello";
    uint32_t err_code = mqttsn_client_publish(&m_client, m_topic_pub.topic_id, 
        (uint8_t*)pub_data, strlen(pub_data), &m_msg_id);
}*/




































