/* echo-client.c - Networking echo client */

/*
 * Copyright (c) 2017 Intel Corporation.
 * Copyright (c) 2018 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * The echo-client application is acting as a client that is run in Zephyr OS,
 * and echo-server is run in the host acting as a server. The client will send
 * either unicast or multicast packets to the server which will reply the packet
 * back to the originator.
 *
 * In this sample application we create four threads that start to send data.
 * This might not be what you want to do in your app so caveat emptor.
 */

#include <logging/log.h>
LOG_MODULE_REGISTER(net_echo_client_sample, LOG_LEVEL_DBG);

#include <zephyr.h>
#include <errno.h>
#include <stdio.h>

#include <net/socket.h>
#include <net/tls_credentials.h>

#include <net/net_mgmt.h>
#include <net/net_event.h>
#include <net/net_conn_mgr.h>


#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/thread.h"
#include "openthread/tasklet.h"
#include "openthread/ip6.h"
#include "openthread/mqttsn.h"
#include "openthread/dataset.h"
#include "openthread/link.h"
//#include "openthread-system.h"

#define NETWORK_NAME "OpenThread"
#define PANID 0xFACE
//#define EXTPANID {0x33, 0x33, 0x33, 0x33, 0x44, 0x44, 0x44, 0x44}
#define DEFAULT_CHANNEL 25
//#define MASTER_KEY {0x33, 0x33, 0x44, 0x44, 0x33, 0x33, 0x44, 0x44, 0x33, 0x33, 0x44, 0x44, 0x33, 0x33, 0x44, 0x44}

#define GATEWAY_PORT 47193
#define GATEWAY_ADDRESS "fd11:22::1637:7871:d626:bf3e"

#define GATEWAY_MULTICAST_PORT 47193
#define GATEWAY_MULTICAST_ADDRESS "ff02::1"
#define GATEWAY_MULTICAST_RADIUS 8

#define CLIENT_ID "NRF52840"
#define CLIENT_PORT 47193


//static const uint8_t sExpanId[] = EXTPANID;
//static const uint8_t sMasterKey[] = MASTER_KEY;

//static void SearchGateway(otInstance *instance);
//static void HandleSearchGw(const otIp6Address* aAddress, uint8_t aGatewayId, void* aContext);

/*static void HandleConnected(otMqttsnReturnCode aCode, void* aContext)
{
    OT_UNUSED_VARIABLE(aCode);
    OT_UNUSED_VARIABLE(aContext);
    printf("Connected\n");
    otInstance *instance = (otInstance *)aContext;
    const char *clientStateString;
    otMqttsnClientStateToString(otMqttsnGetState(instance), &clientStateString);
    printf("MQTT state:%s\n",clientStateString);
    // Handle connected
}

static void HandleDisconnected(otMqttsnDisconnectType aType, void* aContext)
{
    OT_UNUSED_VARIABLE(aType);
    OT_UNUSED_VARIABLE(aContext);
    printf("Disconnected\n");
    // Handle disconnect
}*/


inline uint16_t Swap16(uint16_t v)
{
    return (((v & 0x00ffU) << 8) & 0xff00) | (((v & 0xff00U) >> 8) & 0x00ff);
}


/*static void HandleSearchGw(const otIp6Address* aAddress, uint8_t aGatewayId, void* aContext)
{
    printf("call into handler\n");
    OT_UNUSED_VARIABLE(aGatewayId);
    // Handle SEARCHGW response received
    // Connect to received address
    otInstance *instance = (otInstance *)aContext;
    otIp6Address address = *aAddress;
    // Set MQTT-SN client configuration settings
    otMqttsnConfig config;
    config.mClientId = CLIENT_ID;
    config.mKeepAlive = 30;
    config.mCleanSession = true;
    config.mPort = GATEWAY_MULTICAST_PORT;
    config.mAddress = &address;
    config.mRetransmissionCount = 3;
    config.mRetransmissionTimeout = 5;

    // Register connected callback
    otMqttsnSetConnectedHandler(instance, HandleConnected, (void *)instance);
    // Connect to the MQTT broker (gateway)+
    //otMqttsnConnect(instance, &config);

}

static void SearchGateway(otInstance *instance)
{
    otIp6Address address;
    otIp6AddressFromString(GATEWAY_MULTICAST_ADDRESS, &address);

    otMqttsnSetSearchgwHandler(instance, HandleSearchGw, (void *)instance);
    // Send SEARCHGW multicast message
    otMqttsnSearchGateway(instance, &address, GATEWAY_MULTICAST_PORT, GATEWAY_MULTICAST_RADIUS);
}*/

int main(int aArgc, char *aArgv[])
{
    
    //error = otMqttsnStart(instance, CLIENT_PORT);
    //SearchGateway(instance);

    
    return 0;
}

