/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-27     Rick       the first version
 */
//#include "pin_config.h"
#include "water_work.h"
#include "button.h"
#include "debug.h"

Button_t key_on_btn;
Button_t key_off_btn;
Button_t water_leak_btn;
Button_t water_lost_btn;

extern enum Device_Status DeviceStatus;

void key_on_click_handle(void)
{
    printf("key_on_click_handle\r\n");
    switch(DeviceStatus)
    {
    case ValveClose:
        valve_open();
        break;
    case ValveOpen:
        beep_once();
        break;
    case MasterSensorLost:
        valve_open();
        break;
    case MasterSensorLeak:
        beep_three_times();
        break;
    case MasterSensorAbnormal:
        beep_three_times();
        break;
    case LearnDevice:
        break;
    case InternalValveFail:
        break;
    case ExternalValveFail:
        break;
    case MasterLowTemp:
        break;
    default:
        break;
    }
}

void key_off_click_handle(void)
{
    printf("key_off_click_handle\r\n");
    switch(DeviceStatus)
    {
    case ValveClose:
        beep_key_down();
        break;
    case ValveOpen:
        valve_close();
        break;
    case MasterSensorLost:
        valve_close();
        beep_stop();
        break;
    case MasterSensorLeak:
        beep_stop();
        break;
    case MasterSensorAbnormal:
        beep_key_down();
        warning_all_clear();
        break;
    case LearnDevice:
        break;
    case InternalValveFail:
        beep_stop();
        break;
    case ExternalValveFail:
        beep_stop();
        break;
    case MasterLowTemp:
        beep_stop();
        beep_key_down();
        break;
    default:
        break;
     }
}

void water_leak_up_callback(void)
{
    MasterStatusChangeToDeAvtive();
    printf("water_leak_up_callback\r\n");
}

void water_leak_down_callback(void)
{
    extern WariningEvent MasterSensorLeakEvent;
    warning_enable(MasterSensorLeakEvent);
    printf("water_leak_down_callback\r\n");
}

void water_lost_plugin_callback(void)
{
    warning_lost_clear();
    printf("water_lost_up_callback\r\n");
}

void water_lost_plugout_callback(void)
{
    extern WariningEvent MasterSensorLostEvent;
    warning_enable(MasterSensorLostEvent);
    printf("water_lost_down_callback\r\n");
}

uint8_t read_key_on(void)
{
    return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0);
}
uint8_t read_key_off(void)
{
    return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3);
}
uint8_t read_water_leak(void)
{
    return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0);
}
uint8_t read_water_lost(void)
{
    return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9);
}

void button_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; // KEY1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // KEY2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // WATER_SINGAL
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // DISCONNECT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    Button_Create("key_on_btn", &key_on_btn, read_key_on, 0);
    Button_Create("key_off_btn", &key_off_btn, read_key_off, 0);
    Button_Create("water_leak_btn", &water_leak_btn, read_water_leak, 0);
    Button_Create("water_lost_btn", &water_lost_btn, read_water_lost, 0);
    Button_Attach(&key_on_btn, BUTTON_DOWN, key_on_click_handle);
    Button_Attach(&key_off_btn, BUTTON_DOWN, key_off_click_handle);
    Button_Attach(&water_leak_btn, BUTTON_UP, water_leak_up_callback);
    Button_Attach(&water_leak_btn, BUTTON_DOWN, water_leak_down_callback);
    Button_Attach(&water_lost_btn, BUTTON_UP, water_lost_plugin_callback);
    Button_Attach(&water_lost_btn, BUTTON_DOWN, water_lost_plugout_callback);
}
