#include "debug.h"
#include "water_work.h"

uint32_t factory_tick = 0;

// void pd_chip_factory_gpio_set(void)
// {
//     uint8_t data = 0;

//     data |= GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) << 2;
//     data |= GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) << 3;
//     data |= GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9) << 4;
//     data |= GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0) << 5;
//     data |= GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) << 6;
//     data |= GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) << 7;
//     //pd_spi_write_single(device,0x08,data);
// }

extern UINT8  Tmr_Ms_Dlt;
extern enum Device_Status DeviceStatus;
// extern uint8_t valve_status;
// extern uint8_t internal_valve_open_result;
// extern uint8_t internal_valve_close_result;
// extern uint8_t internal_valve_check_result;
// extern uint8_t external_valve_open_result;
// extern uint8_t external_valve_close_result;
// extern uint8_t external_valve_check_result;

void pd_chip_factory_info_set(void)
{
    uint8_t data = 0;

    data = DeviceStatus;
    data |= get_valve_test_status();
    // data |= valve_status << 1;
    // data |= internal_valve_close_result << 2;
    // data |= internal_valve_open_result << 3;
    // data |= internal_valve_check_result << 4;
    // data |= external_valve_close_result << 5;
    // data |= external_valve_open_result << 6;
    // data |= external_valve_check_result << 7;

    // printf("valve_status = %d  %d  %d  %d %d\r\n",
    //     valve_status,internal_valve_close_result,internal_valve_open_result,internal_valve_check_result,get_valve_status());
//    printf("info data = %d\r\n",data);
    my_Valve_Info_Status_Set(data);
    // pd_spi_write_single(device,0x09,data);
}
void factory_handle(void)
{
    // period 1s
    static uint8_t simu_valve_check = 0;
    static uint8_t simu_open_valve = 0;
    static uint8_t simu_close_valve = 0;

    // pd_chip_factory_gpio_set();
    pd_chip_factory_info_set();

    if (simu_valve_check != (my_Valve_Action_Status_Get() & 0x02))
    {
        simu_valve_check = my_Valve_Action_Status_Get() & 0x02;
        if (simu_valve_check)
        {
            valve_check();
        }
    }

    if (simu_close_valve != (my_Valve_Action_Status_Get() & 0x04))
    {
        simu_close_valve = my_Valve_Action_Status_Get() & 0x04;
        if (simu_close_valve)
        {
            valve_close();
        }
    }

    if (simu_open_valve != (my_Valve_Action_Status_Get() & 0x08))
    {
        simu_open_valve = my_Valve_Action_Status_Get() & 0x08;
        if (simu_open_valve)
        {
            valve_open();
        }
    }
}

void Device_Factory_Handle()
{
    static uint16_t factory_time = 1000;
    if (factory_time > Tmr_Ms_Dlt)
        factory_time -= Tmr_Ms_Dlt;
    else
        factory_time = 0;
    if (factory_time == 0 && my_Valve_Factory_Status_Get())
    {
        factory_time = 1000;
        factory_handle();
    }
}
