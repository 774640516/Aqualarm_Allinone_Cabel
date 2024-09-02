/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-03-03     Rick       the first version
 */
#include "stdlib.h"
#include "debug.h"
#include "signal_led.h"
//#include "pin_config.h"

//定义信号灯对象句柄
static led_t led_obj_off_red = {0};
static led_t led_obj_loss_red = {0};
static led_t led_obj_off_red_once = {0};
static led_t led_obj_off_red_three = {0};
static led_t led_obj_on_green = {0};

static led_t beep_obj = {0};
static led_t beep_obj_loss = {0};
static led_t beep_obj_once = {0};
static led_t beep_obj_three = {0};

uint8_t led_valve_on_pause_flag = 0;

uint32_t led_tick = 0;

static void off_red_on(void *param)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
}

static void off_red_off(void *param)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET);
}

static void on_green_on(void *param)
{
    if(led_valve_on_pause_flag == 0)
    {
        GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
    }
}

static void on_green_off(void *param)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
}

static void beep_on(void *param)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET);
}

static void beep_close(void *param)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
}

void led_handle(void)
{
    if(led_tick > 10)
    {
        led_tick = 0;
        led_ticks();
    }
}

void beep_three_times(void)
{
    led_start(&beep_obj_three);
    led_start(&led_obj_off_red_three);
}

void led_valve_fail(void)
{
    led_set_mode(&beep_obj, 3,"200,200,");
    led_start(&beep_obj);
    led_set_mode(&led_obj_off_red, 3,"200,200,");
    led_start(&led_obj_off_red);
}

void led_factory_start(void)
{
    led_set_mode(&beep_obj, 5,"200,200,");
    led_start(&beep_obj);
    led_set_mode(&led_obj_off_red, 5,"200,200,");
    led_start(&led_obj_off_red);
}

void led_moto_fail_start(void)
{
    led_set_mode(&beep_obj, LOOP_PERMANENT,"200,200,200,200,200,200,200,200,200,200,200,10000,");
    led_start(&beep_obj);
    led_set_mode(&led_obj_off_red, LOOP_PERMANENT,"200,200,200,200,200,200,200,200,200,200,200,10000,");
    led_start(&led_obj_off_red);
}

void led_master_lost_start(void)
{
    led_set_mode(&beep_obj_loss, LOOP_PERMANENT,"200,200,200,5000,");
    led_start(&beep_obj_loss);
    led_set_mode(&led_obj_loss_red, LOOP_PERMANENT,"200,200,200,5000,");
    led_start(&led_obj_loss_red);
}

void led_water_alarm_start(void)
{
    led_set_mode(&beep_obj, LOOP_PERMANENT,"200,200,200,200,200,5000,");
    led_start(&beep_obj);
    led_set_mode(&led_obj_off_red, LOOP_PERMANENT,"200,200,200,200,200,5000,");
    led_start(&led_obj_off_red);
}

void beep_stop(void)
{
    led_stop(&beep_obj_loss);
    led_stop(&beep_obj);
}

void beep_key_down(void)
{
    led_start(&beep_obj_once);
    led_start(&led_obj_off_red_once);
}

void beep_once(void)
{
    led_start(&beep_obj_once);
}

void led_valve_on_pause(void)
{
    led_stop(&led_obj_on_green);
    led_valve_on_pause_flag = 1;
}

void led_valve_on_resume(void)
{
    led_valve_on_pause_flag = 0;
    if(get_valve_status())
    {
        led_start(&led_obj_on_green);
    }
}

void led_ntc_alarm(void)
{
    led_set_mode(&beep_obj, LOOP_PERMANENT,"50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50\
                            ,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,2000,");
    led_set_mode(&led_obj_off_red, LOOP_PERMANENT,"50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50\
                            ,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,2000,");
    led_set_mode(&led_obj_on_green, LOOP_PERMANENT,"50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50\
                            ,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,2000,");
    led_start(&led_obj_off_red);
    led_start(&led_obj_on_green);
    led_start(&beep_obj);
}

void led_valve_on(void)
{
    led_stop(&led_obj_off_red);
    led_set_mode(&led_obj_on_green,LOOP_PERMANENT,"200,0,");
    led_start(&led_obj_on_green);
}

void led_valve_off(void)
{
    led_stop(&led_obj_on_green);
}

void led_warn_off(void)
{
    led_stop(&led_obj_off_red);
}

void led_loss_off(void)
{
    led_stop(&led_obj_loss_red);
}

void led_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // LED1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; // LED2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; // BUZZ
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);

    led_create(&led_obj_off_red,off_red_on, off_red_off, NULL);
    led_set_mode(&led_obj_off_red, LOOP_PERMANENT, "200,1,");

    led_create(&led_obj_loss_red,off_red_on, off_red_off, NULL);
    led_set_mode(&led_obj_loss_red, LOOP_PERMANENT, "200,200,");

    led_create(&led_obj_off_red_once,off_red_on, off_red_off, NULL);
    led_set_mode(&led_obj_off_red_once, 1, "200,1,");

    led_create(&led_obj_off_red_three,off_red_on, off_red_off, NULL);
    led_set_mode(&led_obj_off_red_three, 3, "200,200,");

    led_create(&led_obj_on_green,on_green_on, on_green_off, NULL);
    led_set_mode(&led_obj_on_green, 3, "200,200,");

    led_create(&beep_obj,beep_on, beep_close, NULL);
    led_set_mode(&beep_obj, LOOP_PERMANENT, "200,200,");

    led_create(&beep_obj_loss,beep_on, beep_close, NULL);
    led_set_mode(&beep_obj_loss, LOOP_PERMANENT, "200,200,");

    led_create(&beep_obj_once,beep_on, beep_close, NULL);
    led_set_mode(&beep_obj_once, 1, "200,1,");

    led_create(&beep_obj_three,beep_on, beep_close, NULL);
    led_set_mode(&beep_obj_three, 3, "200,200,");
}
