/*
 *  Copyright (c) 2018, Vit Holasek
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <zephyr.h>
//#include <device.h>
//#include <devicetree.h>
//#include <drivers/gpio.h>

#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/thread.h"
#include "openthread/tasklet.h"
#include "openthread/ip6.h"
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

/*static void SearchGateway(otInstance *instance);
static void HandleSearchGw(const otIp6Address* aAddress, uint8_t aGatewayId, void* aContext);*/

static void HandleConnected(otMqttsnReturnCode aCode, void* aContext)
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
}

/*static void MqttsnConnect(otInstance *instance)
{
    otIp6Address address;
    otIp6AddressFromString(GATEWAY_ADDRESS, &address);

    // Set MQTT-SN client configuration settings
    otMqttsnConfig config;
    config.mClientId = CLIENT_ID;
    config.mKeepAlive = 30;
    config.mCleanSession = true;
    config.mPort = GATEWAY_PORT;
    config.mAddress = &address;
    config.mRetransmissionCount = 3;
    config.mRetransmissionTimeout = 10;

    // Register connected callback
    otMqttsnSetConnectedHandler(instance, HandleConnected, (void *)instance);
    // Register disconnected callback
    otMqttsnSetDisconnectedHandler(instance, HandleDisconnected, (void *)instance);
    // Connect to the MQTT broker (gateway)
    otMqttsnConnect(instance, &config);
}*/

inline uint16_t Swap16(uint16_t v)
{
    return (((v & 0x00ffU) << 8) & 0xff00) | (((v & 0xff00U) >> 8) & 0x00ff);
}


static void StateChanged(otChangedFlags aFlags, void *aContext)
{
    otInstance *instance = (otInstance *)aContext;
    if ((aFlags & OT_CHANGED_THREAD_ROLE) != 0)
   {
       otDeviceRole changedRole = otThreadGetDeviceRole(aContext);

       switch (changedRole)
       {
       case OT_DEVICE_ROLE_LEADER:
           printf("Leader\n");
           break;

       case OT_DEVICE_ROLE_ROUTER:
           {
           printf("Router\n");
           printf("Router search gateway\n");
           const otNetifAddress *unicastAddrs = otIp6GetUnicastAddresses(instance);

   	 for (const otNetifAddress *addr = unicastAddrs; addr; addr = addr->mNext)
   	 {
         	 otIp6Address address = addr->mAddress;
         	 printf("End device:%x:%x:%x:%x:%x:%x:%x:%x\n", Swap16(address.mFields.m16[0]), 	Swap16(address.mFields.m16[1]),
	         Swap16(address.mFields.m16[2]), Swap16(address.mFields.m16[3]), 			    Swap16(address.mFields.m16[4]),
                 Swap16(address.mFields.m16[5]), Swap16(address.mFields.m16[6]), 				Swap16(address.mFields.m16[7]));
         }
         if(otMqttsnGetState(instance) == kStateDisconnected){
            printf("begin to connect\n");
            otIp6Address address;
            uint16_t port=47193;
            otIp6AddressFromString(GATEWAY_ADDRESS, &address);
            otMqttsnSetConnectedHandler(instance, HandleConnected, (void *)instance);
            otMqttsnSetDisconnectedHandler(instance, HandleDisconnected, (void *)instance);
            otMqttsnConnectDefault(instance,&address,(uint16_t)port);
            //MqttsnConnect(instance);
	 }  
           //SearchGateway(instance); 
           }
           break;
       
       case OT_DEVICE_ROLE_CHILD:
           {
           printf("Child\n");
           printf("Child search gateway\n");
          const otNetifAddress *unicastAddrs = otIp6GetUnicastAddresses(instance);

   	 for (const otNetifAddress *addr = unicastAddrs; addr; addr = addr->mNext)
   	 {
         	 otIp6Address address = addr->mAddress;
         	 printf("End device:%x:%x:%x:%x:%x:%x:%x:%x\n", Swap16(address.mFields.m16[0]), 	Swap16(address.mFields.m16[1]),
	         Swap16(address.mFields.m16[2]), Swap16(address.mFields.m16[3]), 			    Swap16(address.mFields.m16[4]),
                 Swap16(address.mFields.m16[5]), Swap16(address.mFields.m16[6]), 				Swap16(address.mFields.m16[7]));
         }
           //SearchGateway(instance);
           }
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
    /*if (aFlags & OT_CHANGED_THREAD_ROLE)
    {
        otDeviceRole role = otThreadGetDeviceRole(instance);
        // If role changed to any of active roles then send SEARCHGW message
        if (role == OT_DEVICE_ROLE_CHILD || role == OT_DEVICE_ROLE_ROUTER)
        {
            printf("search gateway\n");
            SearchGateway(instance);
        }
    }*/
    // when thread role changed
   /* if (aFlags & OT_CHANGED_THREAD_ROLE)
    {
        otDeviceRole role = otThreadGetDeviceRole(instance);
        // If role changed to any of active roles and MQTT-SN client is not connected then connect
        if ((role == OT_DEVICE_ROLE_CHILD || role == OT_DEVICE_ROLE_ROUTER)
            && otMqttsnGetState(instance) == kStateDisconnected)
        {
            MqttsnConnect(instance);
        }
    }*/
}


/*static void HandleSearchGw(const otIp6Address* aAddress, uint8_t aGatewayId, void* aContext)
{
    printf("call into handler\n");
    OT_UNUSED_VARIABLE(aGatewayId);
    // Handle SEARCHGW response received
    // Connect to received address
    otInstance *instance = (otInstance *)aContext;
    otIp6Address address = *aAddress;
    printf("address:%x:%x:%x:%x:%x:%x:%x:%x", address.mFields.m16[0], address.mFields.m16[1],
address.mFields.m16[2], address.mFields.m16[3], address.mFields.m16[4],
        address.mFields.m16[5], address.mFields.m16[6], address.mFields.m16[7]);
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
    otMqttsnConnect(instance, &config);

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
    otError error = OT_ERROR_NONE;
    //otExtendedPanId extendedPanid;
    //otMasterKey masterKey;
    otInstance *instance;

    otSysInit(aArgc, aArgv);
    instance = otInstanceInitSingle();

    // Set default network settings
    // Set network name
    error = otThreadSetNetworkName(instance, NETWORK_NAME);
    // Set extended PANID
    //memcpy(extendedPanid.m8, sExpanId, sizeof(sExpanId));
   // error = otThreadSetExtendedPanId(instance, &extendedPanid);
    // Set PANID
    error = otLinkSetPanId(instance, PANID);
    // Set channel
    error = otLinkSetChannel(instance, DEFAULT_CHANNEL);
    // Set masterkey
    //memcpy(masterKey.m8, sMasterKey, sizeof(sMasterKey));
    //error = otThreadSetMasterKey(instance, &masterKey);

    // Register notifier callback to receive thread role changed events
    error = otSetStateChangedCallback(instance, StateChanged, instance);

    // Start thread network
    //otIp6SetSlaacEnabled(instance, true);
    error = otIp6SetEnabled(instance, true);
    error = otThreadSetEnabled(instance, true);
    error = otMqttsnStart(instance, CLIENT_PORT);
    //otCliUartInit(instance);

    while (true)
    {
        otTaskletsProcess(instance);
        otSysProcessDrivers(instance);
    }
    return error;
}

