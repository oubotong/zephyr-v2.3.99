/**
 * Copyright (c) 2017 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

//#include "app_timer.h"
#include <zephyr.h>
#include "nrf_error.h"
#include "mqttsn_platform.h"
#include "mqttsn_packet_internal.h"
#include "openthread/random_noncrypto.h"
#include "openthread/platform/alarm-milli.h"

/* Available timer is 32-bit. */
#define MQTTSN_PLATFORM_TIMER_MAX_MS UINT32_MAX

static struct k_timer my_status_timer;
static void timer_timeout_handler(void * p_context);
//APP_TIMER_DEF(m_timer_id);

//typedef app_timer_event_t mqttsn_timer_event_t;

static void timer_timeout_handler(void * p_context)
{
    mqttsn_client_t * p_client = (mqttsn_client_t *)p_context;
    mqttsn_client_timeout_handle(p_client);
}

uint32_t mqttsn_platform_init()
{
    return mqttsn_platform_timer_init();
}

uint32_t mqttsn_platform_timer_init()
{
    k_timer_init(&my_status_timer, timer_timeout_handler, NULL);
    return NRF_SUCCESS;
    //UNUSED_VARIABLE(app_timer_init());
    //return app_timer_create(&m_timer_id, APP_TIMER_MODE_SINGLE_SHOT, timer_timeout_handler);
}

uint32_t mqttsn_platform_timer_start(mqttsn_client_t * p_client, uint32_t timeout_ms)
{

    k_timer_start(&my_status_timer, K_MSEC(3000), K_NO_WAIT);
    return NRF_SUCCESS;
    /*uint32_t timeout_ticks = APP_TIMER_TICKS(timeout_ms);
    if (timeout_ticks < 5)
    {
        timeout_ticks = 5;
    }

    return app_timer_start(m_timer_id, timeout_ticks, p_client);*/
}

uint32_t mqttsn_platform_timer_stop()
{
    k_timer_stop(&my_status_timer);
    return NRF_SUCCESS;
    //return app_timer_stop(m_timer_id);
}

uint32_t mqttsn_platform_timer_cnt_get()
{
    return otPlatAlarmMilliGetNow();
}

uint32_t mqttsn_platform_timer_resolution_get()
{
    return MQTTSN_PLATFORM_TIMER_MAX_MS;
}

uint32_t mqttsn_platform_timer_ms_to_ticks(uint32_t timeout_ms)
{
    return k_timer_remaining_get(&my_status_timer)/1000*sys_clock_hw_cycles_per_sec();
    //return APP_TIMER_TICKS(timeout_ms);
}

uint32_t mqttsn_platform_timer_set_in_ms(uint32_t timeout_ms)
{
    return mqttsn_platform_timer_cnt_get() + timeout_ms;
}

uint16_t mqttsn_platform_rand(uint16_t max_val)
{
    return otRandomNonCryptoGetUint16() % max_val;
}
