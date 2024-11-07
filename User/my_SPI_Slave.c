/*
 * my_SPI_Slave.c
 *
 *  Created on: Apr 7, 2024
 *      Author: GW
 */
#include "debug.h"
#include "my_PD_Device.h"
#include "PD_Process.h"
#include "my_PD_Valve.h"

void SPI1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

extern PD_CONTROL PD_Ctl; /* PD Control Related Structures */

uint8_t receive_buff[10];      // SPI   接收内容buff
uint8_t receive_buff_size = 0; // SPI   接收长度
uint8_t receive_flag = 0;      // SPI   接收完成标志位

uint8_t spi_test_buff[10]; // SPI   发送内容buff
uint8_t spi_test_size = 0; // SPI   发送长度设置
uint8_t spi_test_addr = 0; // SPI   实际发送长度
uint8_t spi_flag = 0;

uint16_t spi_time = 0;
uint8_t spi_thread = 0;
uint8_t spi_snk_flag = 0;

void my_SPI_Device_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    NVIC_InitTypeDef NVIC_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_SSOutputCmd(SPI1, ENABLE);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    SPI_Cmd(SPI1, ENABLE);

    GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1);
}

void my_SPI_Master_Init()
{
    spi_thread = 0;
    spi_snk_flag = 0;
    spi_time = 1500;
    my_SPI_Device_Init();
}

void my_SPI_ControlData_on(uint8_t addr, uint8_t bit)
{
}
void my_SPI_ControlData_off(uint8_t addr, uint8_t bit)
{
}

void my_spi_read(uint8_t read_id, uint8_t read_size)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, 0);
    spi_test_buff[0] = read_id;
    spi_test_buff[1] = 0xff;
    spi_test_buff[2] = 0xff;

    spi_flag = 0;
    spi_test_addr = 0;
    spi_test_size = read_size;
    receive_buff_size = 0;
    SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);
}
void my_spi_write(uint8_t write_id, uint8_t write_data1, uint8_t write_dat2, uint8_t write_size)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, 0);
    spi_test_buff[0] = write_id;
    spi_test_buff[1] = write_data1;
    spi_test_buff[2] = write_dat2;
    spi_test_buff[3] = write_dat2;
    spi_flag = 1;
    spi_test_addr = 0;
    spi_test_size = write_size;
    receive_buff_size = 0;
    SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);
}

void SPI1_IRQHandler(void)
{
    if ((spi_flag == 0 || spi_flag == 2) && SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) != RESET)
    {
        receive_buff[receive_buff_size++] = SPI_I2S_ReceiveData(SPI1);

        if (spi_flag == 0 && receive_buff_size == spi_test_size)
        {
            SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, DISABLE);
            GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1);
            receive_flag = 1;
        }
        else if (spi_flag == 2 && spi_test_addr == spi_test_size)
        {
            receive_flag = 1;
            spi_time = 100;
        }
        else
        {
            SPI_I2S_SendData(SPI1, 0x00);
        }
    }
    if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) != RESET)
    {
        if (spi_test_addr == spi_test_size)
        {
            SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
            SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
            spi_flag = 2;
            // GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1);
            // receive_flag = 1;
            // printf("%d  %d  spi send ok\r\n",spi_test_addr,spi_test_size);
            // spi_time = 100;
        }
        else
        {
            SPI_I2S_SendData(SPI1, spi_test_buff[spi_test_addr++]);
            if (spi_flag == 0)
            {
                SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
                SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
            }
        }
    }
}

uint8_t spi_PDO_Len;
uint16_t spi_Current;

void my_spi_SNK(uint8_t PDO_Len, uint16_t Current) // PD受电端 SPI设置
{
    spi_snk_flag = 1;
    spi_PDO_Len = PDO_Len;
    spi_Current = Current;
    printf("my_spi_SNK\r\n");
}

void my_spi_SRC(uint8_t PDO_Len, uint16_t Current, uint8_t status) // PD供电端 SPI设置
{
}

void my_spi_set_hall(uint8_t hall)
{ // 设置spi HALL标志位
}

void my_spi_disconnect() // PD 断开连接 SPI设置
{
}

void my_spi_set_valve()
{ // SPI 设置阀门状态
}

uint8_t my_spi_get()
{ // SPI获取
}

uint8_t my_time_tick(uint16_t *data)
{
    if (*data > Tmr_Ms_Dlt)
        *data -= Tmr_Ms_Dlt;
    else
        *data = 0;
    if (*data == 0)
    {
        return 1;
    }
    return 0;
}

void my_spi_receive_show()
{
    if (receive_flag == 1)
    {
        receive_flag = 0;
        for (uint8_t i = 0; i < receive_buff_size; i++)
        {
            printf("%02x  ", receive_buff[i]);
        }
        printf("\r\n");
    }
}

static uint8_t spi_control_status = 0;
static uint8_t spi_info_status = 0;
void my_spi_open_valve() // spi子机打开阀门
{
    spi_control_status = 1;
}
void my_spi_close_valve() // spi子机关闭阀门
{
    spi_control_status = 2;
}
void my_spi_check_valve() // spi子机阀门自检
{
    spi_control_status = 3;
}
void my_spi_read_status_of_valve() // SPI子机读取开关到位
{
    // spi_control_status = 4;
}
void my_spi_read_error_of_valve() // SPI子机读取阀门异常
{
    //    spi_control_status = 5;
}
void my_spi_info_valve()
{
    spi_info_status = 1;
}

void my_spi_handle()
{

    switch (spi_thread)
    {
    case 0:
        if (my_time_tick(&spi_time)) // 轮询读取SPI子机的状态
        {
            spi_thread = 1;
            my_spi_read(0x01, 2);
            spi_time = 500;
        }
        if (spi_snk_flag == 1) // 当主机为受电端时
        {
            spi_thread = 5;
            spi_time = 500;
        }
        break;
    case 1:
        if (my_time_tick(&spi_time)) // 读取子机状态超时
        {
            spi_time = 500;
            spi_thread = 0;
        }
        if (receive_flag == 1)
        {
            my_spi_receive_show();
            if (receive_buff[1] == 0x11) // 当子机为受电端时
            {
                spi_thread = 2;
                spi_time = 500;
            }
        }
        break;
    case 2:
        if (my_time_tick(&spi_time)) // 等待500ms后读取子机受电端参数
        {
            my_spi_read(0x05, 3);
            spi_time = 500;
            spi_thread = 3;
        }
        break;
    case 3:
        if (my_time_tick(&spi_time)) // 子机SPI未响应
        {
            spi_time = 500;
            spi_thread = 2;
            //            printf("spi read timeour  %d\r\n\r\n\r\n", spi_thread);
        }
        if (receive_flag == 1)
        {
            my_spi_receive_show();
            if ((receive_buff[1] >> 1) > 0 && (receive_buff[1] >> 1) < 6) // 判断子机数据
            {
                spi_thread = 4;
                my_set_Vbus_Current(receive_buff[1], receive_buff[2]); // 设置主机为供电端 设置输出功率
                if (PD_Ctl.Flag.Bit.Connected == 0)
                {
                    set_PD_STATUS_SRC();
                }
                else
                {
                    my_switch_state_pd(1);
                }
                set_sc8726_select();
            }
            else
            {
                printf("receive_data error\r\n");
            }
        }
        break;
    case 4: // 主机位供电端，SPI无动作
        break;
    case 5:                          // 5-6为设置子机为供电端
        if (my_time_tick(&spi_time)) // 主机等待500ms后设置子机作为供电端的参数
        {
            // printf("0x86\r\n");
            my_spi_write(0x86, (spi_PDO_Len << 1) + (uint8_t)(spi_Current >> 8), (uint8_t)spi_Current, 3);
            spi_thread = 6;
            spi_time = 500;
        }

        break;
    case 6:
        if (receive_flag == 1)
        { // 设置成功后 , 轮询读取子机参数
            if (my_time_tick(&spi_time))
            {
                my_spi_receive_show();
                GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1);
                spi_thread = 7;
                spi_time = 1000;
                // printf("set slave out\r\n");
            }
        }
        else if (my_time_tick(&spi_time))
        { // 子机SPI未响应
            spi_thread = 5;
            spi_time = 1000;
        }
        break;
    case 7: // 7-8为读取子机设备状态，判断是否连接到了阀门
        if (my_time_tick(&spi_time))
        {
            spi_thread = 8;
            spi_time = 500;
            // printf("read slave Status\r\n");
            my_spi_read(0x01, 2);
        }
        break;
    case 8:
        if (my_time_tick(&spi_time)) // 读取子机状态超时
        {
            spi_time = 1000;
            spi_thread = 7;
        }
        if (receive_flag == 1)
        {
            printf("receive Connect");
            my_spi_receive_show();
            if ((receive_buff[1] & 0x7f) == 0x24) // 当子机为供电端连接阀门时
            {
                spi_thread = 9;
                spi_time = 500;
                if (get_valve_status())
                {
                    spi_control_status = 1;
                }
                else
                {
                    spi_control_status = 2;
                }
                my_Valve_Connect_SPI();
                if (receive_buff[1] & 0x80)
                {
                    my_Valve_Factory_On();
                }
            }
        }
        break;
    case 9: // 9-10为1S读取一次SPI阀门状态 ，并且根据外部命令设置和读取SPI子机
        if (my_time_tick(&spi_time))
        {
            spi_thread = 10;
            spi_time = 500;
            if (my_Valve_Factory_Status_Get() == 0)
            {
                my_spi_read(0x07, 3);
            }
            else
            {
                my_spi_read(0x0a, 2);
            }
        }
        else if (spi_control_status)
        {
            switch (spi_control_status)
            {
            case 1:
                my_spi_write(0x82, 0x01, 0x00, 2);
                break;
            case 2:
                my_spi_write(0x82, 0x00, 0x00, 2);
                break;
            case 3:
                my_spi_write(0x83, 0x01, 0x00, 2);
                break;
            }
            spi_thread = 11;
            spi_time = 500;
            spi_control_status = 0;
        }
        else if (spi_info_status)
        {
            my_spi_write(0x89, my_Valve_Info_Status_Get(), 0x00, 2);
            spi_thread = 11;
            spi_time = 500;
            spi_info_status = 0;
        }
        break;
    case 10: // 读取阀门开关到位状态
        if (my_time_tick(&spi_time))
        { // 读取子机状态超时
            spi_time = 1000;
            spi_thread = 9;
            my_Spi_Connect_Error();
        }
        if (receive_flag == 1)
        {
            my_spi_receive_show();
            if (my_Valve_Factory_Status_Get() == 0)
            {
                if ((receive_buff[1] & 0xfc) == 0xfc) // 当子机还来连接着阀门时，判断阀门状态
                {
                    my_Valve_Set_Status((receive_buff[1] & 0x02) >> 1, receive_buff[1] & 0x01);
                    my_Valve_Set_power_error((receive_buff[2] & 0x08) >> 3);
                    my_Valve_Set_check_error((receive_buff[2] & 0x04) >> 2);
                    my_Valve_Set_open_timeout((receive_buff[2] & 0x02) >> 1);
                    my_Valve_Set_close_timeout(receive_buff[2] & 0x01);
                    spi_thread = 9;
                }
                else
                {
                    my_Valve_Disconnect();
                    spi_thread = 7;
                }
            }
            else
            {
                spi_thread = 9;
                if (receive_buff[1] & 0x02)
                { // check
                    // if ((my_Valve_Action_Status_Get() & 0x02) == 0)
                    // {
                    //     spi_control_status = 3;
                    // }
                    my_Valve_Action_Bit_On(1);
                }
                if (receive_buff[1] & 0x04)
                { // close
                    // if ((my_Valve_Action_Status_Get() & 0x04) == 0)
                    // {
                    //     spi_control_status = 2;
                    // }
                    my_Valve_Action_Bit_On(2);
                }
            }
            spi_time = 500;
        }
        break;
    case 11: // 设置阀门开关自检
        if (receive_flag == 1)
        { // 设置成功后 , 轮询读取子机参数
            if (my_time_tick(&spi_time))
            {
                my_spi_receive_show();
                GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1);
                spi_thread = 9;
                spi_time = 500;
            }
        }
        else if (my_time_tick(&spi_time))
        { // 子机SPI未响应
            spi_thread = 9;
            spi_time = 500;
            my_Spi_Connect_Error();
        }
        break;
    case 12: // 读取阀门异常
        if (my_time_tick(&spi_time))
        { // 读取子机状态超时
            spi_time = 1000;
            spi_thread = 9;
            my_Spi_Connect_Error();
        }
        if (receive_flag == 1)
        {
            my_spi_receive_show();
            //            my_Valve_Set_Status((receive_buff[1] & 0x02) >> 1, receive_buff[1] & 0x01);
            my_Valve_Set_power_error((receive_buff[1] & 0x08) >> 3);
            my_Valve_Set_check_error((receive_buff[1] & 0x04) >> 2);
            my_Valve_Set_open_timeout((receive_buff[1] & 0x02) >> 1);
            my_Valve_Set_close_timeout(receive_buff[1] & 0x01);
            spi_time = 1000;
            spi_thread = 9;
        }
        break;
    }
}
