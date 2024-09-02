/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-25     Rick       the first version
 */
#include <water_work.h>
#include <string.h>

enum Device_Status DeviceStatus = ValveOpen;

WariningEvent NowEvent = {0};
WariningEvent InternalValveFailEvent = {0};
WariningEvent ExternalValveFailEvent = {0};
WariningEvent MasterSensorLostEvent = {0};
WariningEvent MasterSensorLeakEvent = {0};
WariningEvent MasterLowTempEvent = {0};

void warning_enable(WariningEvent event)
{
    if(event.priority >= NowEvent.priority)
    {
        NowEvent.last_id = event.warning_id;
        NowEvent.priority = event.priority;
        if(event.callback != NULL)
        {
            NowEvent.callback = event.callback;
            NowEvent.callback(0);
        }
        printf("Warning_Enable Success,warning id is %d , now priority is %d\r\n",event.warning_id,event.priority);
    }
    else
    {
        printf("Warning_Enable Fail last is %d Now is %d\r\n",NowEvent.priority,event.priority);
    }

}

void warning_event_bind(uint8_t warning_id,uint8_t priority,WariningEvent *event,void (*callback)(void* parameter))
{
    memset(event,0,sizeof(WariningEvent));
    event->warning_id = warning_id;
    event->priority = priority;
    event->callback = callback;
}

void MasterSensorLostEventCallback(void *parameter)
{
    led_valve_on_pause();
    led_master_lost_start();
    DeviceStatus = MasterSensorLost;
}

void MasterStatusChangeToDeAvtive(void)
{
    DeviceStatus = MasterSensorAbnormal;
}

void MasterSensorLeakEventCallback(void *parameter)
{
    valve_close();
    led_warn_off();
    led_water_alarm_start();
    DeviceStatus = MasterSensorLeak;
}

void MasterLowTempEventCallback(void *parameter)
{
    valve_close();
    led_ntc_alarm();
    DeviceStatus = MasterLowTemp;
}

void valvefail_warning_disable(void)
{
    if(DeviceStatus == InternalValveFail || DeviceStatus == ExternalValveFail)
    {
        warning_all_clear();
    }
}

void InternalValveFailEventCallback(void *parameter)
{
    led_valve_on_pause();
    led_moto_fail_start();
    DeviceStatus = InternalValveFail;
}

void ExternalValveFailEventCallback(void *parameter)
{
    led_valve_on_pause();
    led_moto_fail_start();
    DeviceStatus = ExternalValveFail;
}

void warning_init(void)
{
    warning_event_bind(0,0,&NowEvent,0);//本地存储器
    warning_event_bind(1,3,&InternalValveFailEvent,InternalValveFailEventCallback);
    warning_event_bind(2,3,&ExternalValveFailEvent,ExternalValveFailEventCallback);
    warning_event_bind(3,1,&MasterSensorLostEvent,MasterSensorLostEventCallback);
    warning_event_bind(4,4,&MasterSensorLeakEvent,MasterSensorLeakEventCallback);
    warning_event_bind(5,2,&MasterLowTempEvent,MasterLowTempEventCallback);
}

uint32_t warning_status_get(void)
{
    return DeviceStatus;
}

void warning_all_clear(void)
{
    beep_stop();
    led_warn_off();
    led_valve_on_resume();
    DeviceStatus = get_valve_status();
    memset(&NowEvent, 0, sizeof(WariningEvent));
}

void warning_lost_clear(void)
{
    if(DeviceStatus == ValveClose || DeviceStatus == ValveOpen || DeviceStatus == MasterSensorLost)
    {
        warning_all_clear();
        led_loss_off();
        led_valve_on_resume();
    }
}
