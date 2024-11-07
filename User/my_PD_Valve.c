/*
 * my_PD_Valve.c
 *
 *  Created on: Jul 19, 2024
 *      Author: GW
 */

#include "my_PD_Valve.h"
#include "PD_Process.h"
#include "my_SPI_Slave.h"

static uint8_t valve_status = 0;        // 阀门连接状态
static uint8_t valve_open_switch = 0;   // 阀门开到位HALL
static uint8_t valve_close_switch = 0;  // 阀门关到位HALL
static uint8_t valve_power_error = 0;   // 阀门功率错误
static uint8_t valve_check_error = 0;   // 阀门自检错误
static uint8_t valve_open_timeout = 0;  // 阀门开到位超时
static uint8_t valve_close_timeout = 0; // 阀门关到位超时

static uint8_t valve_factory = 0; // 阀门产测标志
static uint8_t valve_action = 0;  // 阀门测试标志位
static uint8_t valve_info = 0;    // 阀门产测状态

static uint8_t spi_connect_status = 0; // 与SPI通讯状态

uint8_t my_Valve_Factory_Status_Get() // 读取阀门是否进入产测模式
{
    return valve_factory;
}
uint8_t my_Valve_Action_Status_Get() // 读取阀门产测对应内容标志位
{
    return valve_action;
}
uint8_t my_Valve_Status_Get() // 读取与阀门的连接状态
{
    return valve_status;
}
void my_Valve_Connect_PD() // 主机作为供电端连接阀门
{
    valve_status = 1;
}
void my_Valve_Connect_SPI() // 子机作为供电端连接阀门
{
    valve_status = 2;
}
void my_Valve_Factory_On() // 启动阀门产测
{
    valve_factory = 1;
}
// void my_Vakve_Action_Status_Set(uint8_t data) // 设置阀门产测对应内容
// {
//     valve_action = data;
// }
void my_Valve_Action_Bit_On(uint8_t addr) // 设置阀门产测对应内容
{
    valve_action |= (1 << addr);
}
void my_Valve_Info_Status_Set(uint8_t data) // 设置阀门产测中的各种状态
{
    valve_info = data;
    if (valve_status == 1)
    {
        my_pd_Test_Send(valve_info);
    }
    else if (valve_status == 2)
    {
        my_spi_info_valve();
        spi_connect_status = 0;
    }
}
uint8_t my_Valve_Info_Status_Get() // 获取阀门产测中的各种状态
{
    return valve_info;
}
void my_Valve_Disconnect() // 主机或子机与阀门断开连接
{
    valve_status = 0;
    valve_open_switch = 0;
    valve_close_switch = 0;
    valve_power_error = 0;
    valve_check_error = 0;
    valve_open_timeout = 0;
    valve_close_timeout = 0;
}

void my_Valve_Controls_Open() // 打开阀门
{
    printf("set External Valve Open\r\n");
    if (valve_status == 1)
    {
        valve_open_timeout = 0;
        my_pd_open_valve();
    }
    else if (valve_status == 2)
    {
        my_spi_open_valve();
        spi_connect_status = 0;
    }
    else
    {
        printf("Open Valve Error Disconnect\r\n");
    }
}
void my_Valve_Controls_Close() // 关闭阀门
{
    printf("set External Valve Close\r\n");
    if (valve_status == 1)
    {
        valve_close_timeout = 0;
        my_pd_close_valve();
    }
    else if (valve_status == 2)
    {
        my_spi_close_valve();
        spi_connect_status = 0;
    }
    else
    {
        printf("Close Valve Error Disconnect\r\n");
    }
}
void my_Valve_Controls_Check() // 阀门自检
{
    printf("set External Valve Check\r\n");
    if (valve_status == 1)
    {
        valve_check_error = 0;
        valve_open_timeout = 0;
        my_pd_check_valve();
    }
    else if (valve_status == 2)
    {
        my_spi_check_valve();
        spi_connect_status = 0;
    }
    else
    {
        printf("Check Valve Error Disconnect\r\n");
    }
}
void my_Valve_Controls_Read_Status() // 读取阀门开关到位
{
}
void my_Valve_Controls_Read_Error() // 读取阀门异常状态
{
    if (valve_status == 2)
    {
        my_spi_read_error_of_valve();
        spi_connect_status = 0;
    }
}

void my_Valve_Set_power_error(uint8_t power) // 设置阀门功率异常
{
    valve_power_error = power;
}
void my_Valve_Set_check_error(uint8_t check) // 设置阀门自检异常
{
    valve_check_error = check;
}
void my_Valve_Set_open_timeout(uint8_t open) // 设置阀门打开超时
{
    valve_open_timeout = open;
}
void my_Valve_Set_close_timeout(uint8_t close) // 设置阀门关闭超时
{
    valve_close_timeout = close;
}
void my_Valve_Set_Error(uint8_t power, uint8_t check, uint8_t open, uint8_t close) // 设置阀门4种异常
{
    valve_power_error = power;
    valve_check_error = check;
    valve_open_timeout = open;
    valve_close_timeout = close;
}
void my_Valve_Set_Status(uint8_t open, uint8_t close) // 设置阀门开关到位
{
    valve_open_switch = open;
    valve_close_switch = close;
}

void my_Spi_Connect_Error() // 与子机SPI通讯异常状态获取
{
    spi_connect_status = 1;
}

uint8_t my_Valve_Get_Status() // 读取阀门状态
{
    return (valve_open_switch << 1) + valve_close_switch;
}
uint8_t my_Valve_Get_Error() // 读取阀门异常
{
    printf("get External Valve Error %02x\r\n", (valve_power_error << 3) + (valve_check_error << 2) + (valve_open_timeout << 1) + valve_close_timeout);
    return (valve_power_error << 3) + (valve_check_error << 2) + (valve_open_timeout << 1) + valve_close_timeout;
}

void my_PD_Valve_Receive(uint8_t *buff) // PD接收数据处理函数
{
    if (buff[2] == 0xAA && buff[3] == 0x55)
    {
        if (buff[0] == 0x01) // 错误情况
        {
            switch (buff[1])
            {
            case 0: // 关阀超时  超时时间20S一次
                my_Valve_Set_close_timeout(1);
                break;
            case 1: // 关阀HALL异常  关到位HALL触发并且开到位HALL也触发了
                break;
            case 2: // 开阀超时  超时时间20S一次
                my_Valve_Set_open_timeout(1);
                break;
            case 3: // 开阀HALL异常  开到位HALL触发并且关到位HALL也触发了
                break;
            case 4: // 自检模式开阀门10S后，开到位HALL依然没有被触发
                my_Valve_Set_check_error(1);
                break;
            case 5: // 自检模式开阀10S到位后，关阀3S之后，开到位HALL依然是触发状态
                my_Valve_Set_check_error(1);
                break;
            case 6: // 自检关阀3S后 6S内开到位Hall依然没有被触发
                my_Valve_Set_check_error(1);
                break;
            }
        }
        else if (buff[0] == 0x02) // 开关到位HALL信号
        {
            my_Valve_Set_Status((buff[1] & 0x02) >> 1, buff[1] & 0x01);
        }
        else if (buff[0] == 0x03) // 阀门自检成功
        {
            switch (buff[1])
            {
            case 0: // 阀门自检成功
                my_Valve_Set_check_error(0);
                break;
            }
        }
        else if (buff[0] == 0x04)
        {
            switch (buff[1])
            {
            case 0:
                my_Valve_Action_Bit_On(1);
                // my_Valve_Controls_Check();
                break;
            case 1:
                my_Valve_Action_Bit_On(2);
                // my_Valve_Controls_Close();
                break;
            }
        }
    }
    else
    {
        printf("pd data error %d  %d\r\n", buff[2], buff[3]);
    }
}
