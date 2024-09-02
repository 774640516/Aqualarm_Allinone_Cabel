/*
 * my_SPI_Slave.h
 *
 *  Created on: Apr 7, 2024
 *      Author: GW
 */

#ifndef USER_MY_SPI_SLAVE_H_
#define USER_MY_SPI_SLAVE_H_

// typedef struct
// {
//     uint8_t Reserve;
//     uint8_t Device_Info;
//     uint8_t Control_Valve;
//     uint8_t Check_Valve;
//     uint8_t Error_Valve;
//     uint16_t Input_Power;
//     uint16_t Output_Power;
// }MY_PD_CONTROL;

extern UINT8  Tmr_Ms_Dlt;

void my_SPI_Master_Init();
void my_spi_SNK(uint8_t PDO_Len,uint16_t Current);
void my_spi_SRC(uint8_t PDO_Len, uint16_t Current, uint8_t status);
void my_spi_disconnect();
void my_spi_handle();

void my_SPI_ControlData_on(uint8_t addr,uint8_t bit);
void my_SPI_ControlData_off(uint8_t addr,uint8_t bit);

void my_spi_set_hall(uint8_t hall);

void my_spi_set_valve();
uint8_t my_spi_get();

uint8_t my_time_tick(uint16_t *data);

void my_spi_open_valve();    //spi瀛愭満鎵撳紑闃�闂�
void my_spi_close_valve();   //spi瀛愭満鍏抽棴闃�闂�
void my_spi_check_valve();   //spi瀛愭満闃�闂ㄨ嚜妫�
void my_spi_read_status_of_valve();  //SPI瀛愭満璇诲彇寮�鍏冲埌浣�
void my_spi_read_error_of_valve();   //SPI瀛愭満璇诲彇闃�闂ㄥ紓甯�
#endif /* USER_MY_SPI_SLAVE_H_ */
