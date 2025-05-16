/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/26
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *GPIO routine:
 *PA0 push-pull output.
 *
 ***Only PA0--PA15 and PC16--PC17 support input pull-down.
 */

#include "debug.h"
#include "PD_Process.h"
#include "my_I2C_Device.h"
#include "my_PD_Device.h"

uint16_t set_Vbus;

uint8_t valve_connect_status = 0;
uint8_t valve_status = 0;
uint16_t valve_time = 0;

void my_rcc()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                               RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO |
                               RCC_AHBPeriph_USBPD | RCC_APB2Periph_TIM1 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_I2C1 | RCC_AHBPeriph_USBFS,
                           ENABLE);
}

void IWDG_Feed_Init(u16 prer, u16 rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(prer);
    IWDG_SetReload(rlr);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    my_rcc();
    USART_Printf_Init(921600);
    GPIO_Toggle_INIT();

    warning_init();
    button_init();
    led_init();
    valve_init();

    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("V1.0.3\r\n");
    PD_Init();

    TIM1_Init(999, 48 - 1);
    IIC_Init(80000, 0x02);
    my_SC8726_Init();
    my_SPI_Master_Init(); 
    IWDG_Feed_Init(IWDG_Prescaler_128, 4000);

    while (1)
    {
        IWDG_ReloadCounter();
        my_TIM_Handle();
        my_PD_Handle();
        my_SC8726_Handle();
        my_spi_handle();
        my_valve_check_Handle();
        button_handle();
        led_handle();
        valve_handle();
    }
}
