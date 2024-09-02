/*
 * Copyright (c) 2019 All rights reserved.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-14     WKJay        first version
 * 2022-01-09     kyle         add led switch callback parameter
 */

#include "signal_led.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

static led_t *first_handle = NULL;

void led_create(led_t *led_ptr,void (*switch_on)(void *args), void (*switch_off)(void *args), void *args)
{
    //创建对象前先关闭led
    switch_off(args);
    led_ptr->switch_on      = switch_on;
    led_ptr->switch_off     = switch_off;
    led_ptr->args      = args;
}

/**
 * Name:    led_get_blink_arr
 * Brief:    获得信号灯一个周期内闪烁时间的数组（仅内部调用，用户无需关注）
 * Input:
 *  @handle:   led 内部信息句柄
 * Output:  None
 */
static void led_get_blink_arr(led_t *handle)
{
    char *blink_modeTemp = NULL;
    uint8_t blinkCntNum = 0;

    if (handle->blink_arr)
    {
//        led_free(handle->blink_arr);
        memset(handle->blink_arr,0,sizeof(handle->blink_arr));
    }

    //获取数组长度
    for (blink_modeTemp = handle->blink_mode; *blink_modeTemp != '\0'; blink_modeTemp++)
    {
        if (*blink_modeTemp == ',')
        {
            handle->mode_cnt++;
        }
    }
    if (*(blink_modeTemp - 1) != ',')
        handle->mode_cnt++;

    //存储数据
    for (blink_modeTemp = handle->blink_mode; *blink_modeTemp != '\0'; blink_modeTemp++)
    {
        handle->blink_arr[blinkCntNum] = atol(blink_modeTemp);
        //计算出计数变量的值（根据信号灯定时器定时时间）

        if ((handle->blink_arr[blinkCntNum] % LED_TICK_TIME))
        {
            handle->blink_arr[blinkCntNum] = handle->blink_arr[blinkCntNum] / LED_TICK_TIME + 1;
        }
        else
        {
            handle->blink_arr[blinkCntNum] /= LED_TICK_TIME;
        }
        blinkCntNum++;
        while (*blink_modeTemp != ',')
        {
            if (*blink_modeTemp == '\0')
            {
                blink_modeTemp--;
                break;
            }
            blink_modeTemp++;
        }
    }
}

/**
 * Name:    led_set_mode
 * Brief:   设置信号灯的工作模式
 * Input:
 *  @loop:        循环次数 (0xff 永久循环)
 *  @blink_mode:   一个周期的闪烁方式(字符串形式,如："200,200,200,2000,"表示亮200ms,灭200ms,亮200ms,灭2000ms)
 *                注意：blink_mode字符串必须以英文逗号分隔，且最后以英文逗号结尾
 * Output:  led内部信息结构体
 */
void led_set_mode(led_t *handle, uint8_t loop, char *blink_mode)
{
//    led_internal_t *led_internal_ptr = NULL;
//    led_internal_ptr = led_get_internal(handle);

    handle->loop = loop;
    handle->loop_temp = loop;
    memset(handle->blink_mode,0,sizeof(handle->blink_mode));
    memcpy(handle->blink_mode,blink_mode,strlen(blink_mode));
//    led_internal_ptr->blink_mode = blink_mode;
    handle->handle_cnt = 0;
    handle->blink_point = 0;
    handle->mode_pointer = 0;
    handle->mode_cnt = 0;
    led_get_blink_arr(handle);
}

/**
 * Name:    led_switch
 * Brief:   信号灯状态翻转（内部调用）
 * Input:
 *  @led_handle:   led 句柄
 * Output:  None
 */
static void led_switch(led_t *led_handle)
{
//    led_internal_t *led_internal_ptr = NULL;
//    led_internal_ptr = led_get_internal(led_handle);

    if (led_handle->mode_pointer % 2)
    {
        led_handle->switch_off(led_handle->args);
        led_handle->state = LED_OFF;
    }
    else
    {
        led_handle->switch_on(led_handle->args);
        led_handle->state = LED_ON;
    }
}

/**
 * Name:    led_toggle
 * Brief:   信号灯状态翻转（用户调用）
 * Input:
 *  @led_handle:   led 句柄
 * Output:  None
 */
void led_toggle(led_t *led_handle)
{
//    led_internal_t *led_internal_ptr = NULL;
//    led_internal_ptr = led_get_internal(led_handle);

    if (led_handle->state == LED_OFF)
    {
        led_handle->switch_on(led_handle->args);
        led_handle->state = LED_ON;
    }
    else
    {
        led_handle->switch_off(led_handle->args);
        led_handle->state = LED_OFF;
    }
}

/**
 * Name:    led_handle
 * Brief:   核心工作函数
 * Input:
 *  @crt_handle:   led 句柄
 * Output:  None
 */
static void led_handle(led_t *led_handle)
{
//    led_internal_t *led_internal_ptr = NULL;
//    led_internal_ptr = led_get_internal(crt_handle);

    if (led_handle->loop_temp)
    {
        led_handle->handle_cnt++;
    __repeat:
        if ((led_handle->handle_cnt - 1) == led_handle->blink_point)
        {
            if (led_handle->mode_pointer < led_handle->mode_cnt)
            {
                led_handle->blink_point += led_handle->blink_arr[led_handle->mode_pointer];
                if (led_handle->blink_arr[led_handle->mode_pointer] == 0) //时间为0的直接跳过
                {
                    led_handle->mode_pointer++;
                    goto __repeat;
                }
                else
                {
                    led_switch(led_handle);
                }
                led_handle->mode_pointer++;
            }
            else
            {
                led_handle->mode_pointer = 0;
                led_handle->blink_point = 0;
                led_handle->handle_cnt = 0;
                if (led_handle->loop_temp != LOOP_PERMANENT)
                    led_handle->loop_temp--;
            }
        }
    }
    else
    {
        led_stop(led_handle);
        if (led_handle->led_blink_over_callback)
        {
            led_handle->led_blink_over_callback(led_handle);
        }
    }
}

/**
 * Name:    led_start
 * Brief:   信号灯开启（若没有调用此函数开启信号灯则信号灯不会工作）
 * Input:
 *  @led_handle:   led 句柄
 * Output:  0：             正常
 *          HANDLE_EXIST：  已处于工作状态
 */
uint8_t led_start(led_t *led_handle)
{
//    led_internal_t *led_internal_ptr = NULL;
//    led_internal_ptr = led_get_internal(led_handle);

    led_handle->loop_temp = led_handle->loop; //启动时将其重置

    led_t *temp_handle = first_handle;
    while (temp_handle)
    {
        if (temp_handle == led_handle)
            return HANDLE_EXIST;         //已存在，则退出
        temp_handle = temp_handle->next; //设置链表当前指针
    }

    led_handle->next = first_handle;
    first_handle = led_handle;

    return 0;
}

/**
 * Name:    led_stop
 * Brief:   信号灯关闭（关闭信号灯后仍可调用开启函数开启）
 * Input:
 *  @led_handle:   led 句柄
 * Output:  None
 */
void led_stop(led_t *led_handle)
{
    led_t **handle_pointer = NULL;
    led_t *handle_destory = NULL;

    led_handle->switch_off(led_handle->args); //关闭灯

    for (handle_pointer = &first_handle; *handle_pointer;)
    {
        handle_destory = *handle_pointer;

        if (handle_destory == led_handle)
        {
            *handle_pointer = handle_destory->next;
        }
        else
        {
            handle_pointer = &handle_destory->next;
        }
    }
}

/**
 * Name:    led_set_blink_over_callback
 * Brief:   设置led闪烁结束后的回调函数
 * Input:
 *  @led_handle:   led句柄
 *  @callback:      回调函数
 * Output:  None
 */
void led_set_blink_over_callback(led_t *led_handle, led_blink_over_callback callback)
{
//    led_internal_t *led_internal_ptr = NULL;
//    led_internal_ptr = led_get_internal(led_handle);

    led_handle->led_blink_over_callback = callback;
}

/*@brief    信号灯心跳函数
 *@note     必须循环调用该函数，否则信号灯将不会工作。
 *          可以将其放入线程或定时器中，保证每隔LED_TICK_TIME毫秒调用即可
 *          LED_TICK_TIME是在"signal_led.h"中定义的宏，信号灯的工作基于该宏
 *          保证该宏所定义的时间为信号灯心跳函数调用的周期！
 * */
void led_ticks(void)
{
    led_t *current_handle;

    for (current_handle = first_handle; current_handle != NULL; current_handle = current_handle->next)
    {
        led_handle(current_handle);
    }
}
