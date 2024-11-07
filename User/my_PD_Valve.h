/*
 * my_PD_Valve.h
 *
 *  Created on: Jul 19, 2024
 *      Author: GW
 */

#ifndef USER_MY_PD_VALVE_H_
#define USER_MY_PD_VALVE_H_

#include "debug.h"

uint8_t my_Valve_Status_Get();   //读取与阀门的连接状态
void my_Valve_Connect_PD();     //主机作为供电端连接阀门
void my_Valve_Connect_SPI();    //子机作为供电端连接阀门
void my_Valve_Disconnect();     //主机或子机与阀门断开连接

void my_Valve_Set_power_error(uint8_t power);    //设置阀门功率异常
void my_Valve_Set_check_error(uint8_t check);    //设置阀门自检异常
void my_Valve_Set_open_timeout(uint8_t open);    //设置阀门打开超时
void my_Valve_Set_close_timeout(uint8_t close);  //设置阀门关闭超时
void my_Valve_Set_Error(uint8_t power, uint8_t check, uint8_t open, uint8_t close);    //设置阀门4种异常
void my_Valve_Set_Status(uint8_t open, uint8_t close);      //设置阀门开关到位

void my_Valve_Controls_Open(); // 打开阀门
void my_Valve_Controls_Close(); // 关闭阀门
void my_Valve_Controls_Check(); // 阀门自检
void my_Valve_Controls_Read_Status();    //读取阀门开关到位
void my_Valve_Controls_Read_Error(); //读取阀门异常状态

uint8_t my_Valve_Get_Status();   //读取阀门状态
uint8_t my_Valve_Get_Error();    //读取阀门异常

void my_Spi_Connect_Error();     //与子机SPI通讯异常状态获取



uint8_t my_Valve_Factory_Status_Get();   //读取阀门是否进入产测模式
void my_Valve_Factory_On();  //启动阀门产测

uint8_t my_Valve_Action_Status_Get();    //读取阀门产测对应内容标志位
void my_Valve_Action_Bit_On(uint8_t addr);   // 设置阀门产测对应内容

void my_Valve_Info_Status_Set(uint8_t data); //设置阀门产测中的各种状态;
uint8_t my_Valve_Info_Status_Get();  //获取阀门产测中的各种状态

#endif /* USER_MY_PD_VALVE_H_ */
