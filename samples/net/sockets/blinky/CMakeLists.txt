# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(blinky)
#target_sources(app PRIVATE src/main.c src/mqtt-sn/mqttsn_client.c src/mqtt-sn/mqttsn_gateway_discovery.c src/mqtt-sn/mqttsn_gateway_discovery.c src/mqtt-sn/mqttsn_packet_fifo.c src/mqtt-sn/mqttsn_packet_receiver.c src/mqtt-sn/mqttsn_packet_receiver.c src/mqtt-sn/mqttsn_packet_sender.c src/mqtt-sn/mqttsn_platform.c src/mqtt-sn/mqttsn_transport_ot.c src/mqtt-sn/MQTTSNPacket.c src/mqtt-sn/MQTTSNConnectClient.c src/mqtt-sn/MQTTSNConnectServer.c src/mqtt-sn/MQTTSNDeserializePublish.c src/mqtt-sn/MQTTSNSearchClient.c src/mqtt-sn/MQTTSNSearchServer.c src/mqtt-sn/MQTTSNSerializePublish.c src/mqtt-sn/MQTTSNSubscribeClient.c src/mqtt-sn/MQTTSNSubscribeServer.c src/mqtt-sn/MQTTSNUnsubscribeClient.c src/mqtt-sn/MQTTSNUnsubscribeServer.c)
#SET(CMAKE_C_FLAGS  -DUART_AS_SERIAL_TRANSPORT=1)
set(COMMON_INCLUDES
    ${OT_PUBLIC_INCLUDES}
    #/home/dafran/zephyrproject/modules/lib/openthread/examples/platforms
    #/home/dafran/zephyrproject/modules/lib/openthread/src/core
    #/home/dafran/zephyrproject/modules/lib/openthread/src/cli
)
target_sources(app PRIVATE src/main.c ) 
#set_target_properties(app PROPERTIES LINK_DEPENDS /home/dafran/zephyrproject/modules/lib/openthread/examples/platforms/nrf528xx/nrf52840/nrf52840.ld/nrf52840_bootloader_uart.ld)
target_include_directories(app PRIVATE ${COMMON_INCLUDES})

