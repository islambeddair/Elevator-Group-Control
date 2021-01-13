
/**
******************************************************************************
*@file    	Comm.c
*@authors	Eng.Akram Sayed <akramsayed.m@gmail.com>
*@date    	17/2/2014
*@authors	Eng.Islam Bedair <islambedair2@gmail.com>
*@date    	01/01/2020
*@brief   	This file contains the communication handler .		\n\n
* \b PROJECT   \n&nbsp;&nbsp; 		Duplex_Elevator_Control_System. 		\n
* \b H/W       \n&nbsp;&nbsp; 		Micro Controller STM8L151M8. 	\n
* \b S/W       \n&nbsp;&nbsp; 		IAR Systems IDE & Compiler.		\n
*
*****************************************************************************
*/
/*
* @defgroup Application
* @{
*/

#ifndef _COMM_H
#define _COMM_H
/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
  /************************************* Headers ****************************************/
#include "main.h"
#include <stdbool.h>
#include "Elevator_Control.h"
#include "Config_File.h"
#include "User_Interface.h"
#include "usart.h"
#include "ShiftOut595.h"
  /************************************* Defines *****************************************/
#define COMM_STX			(uint8_t)0X02
#define COMM_ETX			(uint8_t)0X03
#define COMM_ACK			(uint8_t)0X06
#define COMM_NACK			(uint8_t)0X15
#define COMM_CR				(uint8_t)0X0D
#define COMM_LF				(uint8_t)0X0A
#define COMM_RX_FRAME_SIZE	        (uint8_t)17
#define COMM_TX_FRAME_SIZE	        (uint8_t)11
#define COMM_FRAME_HEADER	        (uint8_t)0x1E //30
#define COMM_ELEVATOR_CTRL_ID	        (uint8_t)0XEC //236
#define COMM_DUPLEX_CTRL_ID	        (uint8_t)0XDC //220
#define COMM_DIR_MASK		        (uint8_t)0x03
#define COMM_SPD_MASK		        (uint8_t)0x0C
#define COMM_RSVD_MASK		        (uint8_t)0xF0
#define COMM_CURRENT_FLOOR_MASK		(uint8_t)0x0F
#define COMM_MAX_FLOOR_NO_MASK		(uint8_t)0xF0
#define FORK_ERR                        (uint8_t)0x10
#define COMM_WAITING_ACK_TIMEOUT	(uint8_t)3		/*!- 3 Sec time out*/
#define COMM_CHANGE_COM_PORT_TIMEOUT	(uint16_t)10		/*!- 10  Sec  time out*/
///************************************* Data Type ***************************************/
typedef enum
{
  COMM_IDLE = 0,
  COMM_WAITING_ACK = 1,
  COMM_SEND_DATA = 2,
  COMM_GET_DATA = 3
}Comm_State_Machine_TypeDef;
  typedef enum
  {
    COMM_NOT_MOVE=		        (uint8_t)0x00,
    COMM_MOVE_UP=		        (uint8_t)0x01,
    COMM_MOVE_DOWN=		        (uint8_t)0x02,
  }Comm_Car_Dir_TypeDef;
  typedef enum
  {
    COMM_STOP=		                (uint8_t)0x00,
    COMM_SLOW=		                (uint8_t)0x04,
    COMM_FAST=		                (uint8_t)0x08,
  }Comm_Car_Spd_TypeDef;
  typedef enum
  {
    COMM_NORM=			        (uint8_t)0x01,///
    COMM_SLIP=			        (uint8_t)0x02,
    COMM_OVERLOAD=	                (uint8_t)0x04,
    COMM_UPL=			        (uint8_t)0x08,///
    COMM_DNL=			        (uint8_t)0x10,///
    COMM_SAFE_LOCK_ERROR=	        (uint8_t)0x20,
    COMM_FORK=			        (uint8_t)0x40,
    COMM_LOCK=			        (uint8_t)0x80,
  }Comm_Status_1_TypeDef;
  typedef enum
  {
    COMM_SERV=				(uint8_t)0x01,
    COMM_CAR_STOP=			(uint8_t)0x02,
    COMM_FIRE=				(uint8_t)0x04,
    COMM_SLWT=				(uint8_t)0x08,////
    COMM_FSTT=				(uint8_t)0x10,
    COMM_O_WT=				(uint8_t)0x20,///
    COMM_PHFL=				(uint8_t)0x40,
    COMM_CAM_STATUS=		        (uint8_t)0x80,///
  }Comm_Status_2_TypeDef;
  typedef enum
  {
    COMM_FLOOR_0=			(uint8_t)0x01,
    COMM_FLOOR_1=			(uint8_t)0x02,
    COMM_FLOOR_2=			(uint8_t)0x04,
    COMM_FLOOR_3=			(uint8_t)0x08,
    COMM_FLOOR_4=			(uint8_t)0x10,
    COMM_FLOOR_5=			(uint8_t)0x20,
    COMM_FLOOR_6=			(uint8_t)0x40,
    COMM_FLOOR_7=			(uint8_t)0x80,
  }Comm_Lower_Floors_TypeDef;
  typedef enum
  {
    COMM_FLOOR_8=			(uint8_t)0x01,
    COMM_FLOOR_9=			(uint8_t)0x02,
    COMM_FLOOR_10=			(uint8_t)0x04,
    COMM_FLOOR_11=			(uint8_t)0x08,
    COMM_FLOOR_12=			(uint8_t)0x10,
    COMM_FLOOR_13=			(uint8_t)0x20,
    COMM_FLOOR_14=			(uint8_t)0x40,
    COMM_FLOOR_15=			(uint8_t)0x80,
  }Comm_Higher_Floors_TypeDef;
  typedef enum
  {
    COMM_SEND_ELEVATOR_ORDER=		0x01,
    COMM_SEND_ACK=				0x02,
    COMM_SEND_NACK=				0x04,
    COMM_FRAME_RECEIVED=			0x10,	/*!- set after receiving frame in ISR*/
    COMM_RECEIVE_ACK=			0x20,
    COMM_NEW_TICKS=				0x40,
  }Comm_Events_TypeDef;
  
  typedef enum
  {
    EVEN,
    ODD
  }Chk_Sum_Typedef;
  typedef enum
  {
    RX_SPEED_DIRECTION,
    RX_STATUS_1,
    RX_STATUS_2,
    RX_MAX_CURRENT_FLOOR,
    RX_DUP_EXT_UP_1,
    RX_DUP_EXT_UP_2,
    RX_DUP_EXT_DOWN_1,
    RX_DUP_EXT_DOWN_2,
    RX_DUP_INT_0_7,
    RX_DUP_INT_8_15,
    Stoppage_Time,  //RX_DUP_INT_DOWN_1,
    RX_DUP_INT_DOWN_2,
    RX_CHECK_SUM_ODD,
    RX_CHECK_SUM_EVEN,
    RX_FRAME_SIZE  
  }Rx_Frame_Bytes_t;
  
  typedef enum
  {
    TX_HEADER,
    TX_SOURCE_ID,
    TX_DESTINATION_ID,
    TX_DUP_EXT_UP_1,
    TX_DUP_EXT_UP_2,
    TX_DUP_EXT_DOWN_1,
    TX_DUP_EXT_DOWN_2,
    TX_CHECK_SUM_ODD,
    TX_CHECK_SUM_EVEN,
    TX_FRAME_SIZE
  }Tx_Frame_Bytes_t;
  void  ToggleElevatorStatusLed(uint8_t elevator);
  void ClearElevatorStatusLed(uint8_t elevator);
  void SetElevatorStatusLed(uint8_t elevator);
  int getstrlen(const char * str);
  void RstOutOfServiceCounter(void);
  uint16_t GetOutOfServiceCounter(void);
  void Rstusr_inf_chk_new_req_counter(void);
  uint16_t Getusr_inf_chk_new_req_counter(void);
  void    SetTxBufferElevator(uint8_t ElevatorNumber,uint8_t index, uint8_t data);
  uint8_t GetTxBufferElevator(uint8_t ElevatorNumber,uint8_t index);
  uint8_t *GetTxBufferAddress(uint8_t ElevatorNumber);
  /************************************* External Variables ******************************/
  /************************************* Functions ***************************************/
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief      handling the communication commend through RS232.
  * @param	None
  * @retval     None
  * @note
  */
  void communication_task(void);
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief     send the selected cmd or data.
  * @param	None
  * @retval    None
  * @note
  */
  //void comm_send(Comm_Send_Cmd_TypeDef );
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief     check the ack frame.
  * @param	None
  * @retval    the result of checking
  * @note
  */
  uint8_t comm_chk_ack(void);
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief     get elevator status
  * @param	None
  * @retval    the result of checking
  * @note
  */
  uint8_t comm_get_elevator_data(uint8_t ElevatorNo);
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief     set elevator order
  * @param	None
  * @retval    None
  * @note
  */
  void comm_set_elevator_data(uint8_t ElevatorNo);
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief     calculate the check sum.
  * @param	*data: pointer to the received data.
  * @param	data_size: number of received data.
  * @retval    return the calculated check sum.
  * @note
  */
  //uint8_t comm_chk_sum(volatile uint8_t *,uint8_t ,Chk_Sum_Typedef);
  uint8_t comm_chk_sum(uint8_t *data,uint8_t StartIndex,uint8_t data_size,Chk_Sum_Typedef chk_sum_t);
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief     Set Communication events.
  * @param	None
  * @retval    None
  * @note
  */
  void comm_set_event(uint8_t ElevatorNo,Comm_Events_TypeDef event);
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief     Clr communication events.
  * @param	None
  * @retval    None
  * @note
  */
  void comm_clr_event(uint8_t ElevatorNo,Comm_Events_TypeDef event);
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief     check communication events.
  * @param	None
  * @retval    None
  * @note
  */
  bool comm_check_event_status(uint8_t ElevatorNo,Comm_Events_TypeDef event);
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief     Determine the active port and elevator no.
  * @param	active_port: is the receiving data port
  * @retval    None
  * @note
  */
  void comm_set_active_elevator(UART_HandleTypeDef *huart);
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief     read the active port and elevator no.
  * @param	active_port: is the receiving data port
  * @retval    None
  * @note
  */
  UART_HandleTypeDef *comm_get_active_elevator(uint8_t ElevatorNo);
  /** @auther	Akram Sayed <akramsayed.m@gmail.com>
  * @brief     read the next_active port and elevator no.
  * @param	uart_elevator_next: is the receiving data port
  * @retval    None
  * @note
  */
  UART_HandleTypeDef  *comm_get_next_active_elevator(void);
  /** @auther	Eng.Islam Bedair <islambedair2@gmail.com>
  * @brief
  * @param  UART_Elevator: the elevator uart port
  * @retval Data that received
  * @note
  */
  /************************************* Data Type ***************************************/
  //HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
  void receive_frame(UART_HandleTypeDef * uart_elevator);
  /** @auther	Eng.Islam Bedair <islambedair2@gmail.com>
  * @brief
  * @param
  * @retval None
  * @note
  */
  void clr_chk_sum(void);
  
  /** @auther	Eng.Islam Bedair <islambedair2@gmail.com>
  * @brief	clear the data frame size
  * @param
  * @retval None
  * @note
  */
  void uart_clr_data_size(void);
  void TIM3_COMM_SET_NEW_TICK(void);
  uint8_t GetElevatorNo(UART_HandleTypeDef *huart);
  //void USART_3_default_rx_isr_cb(UART_HandleTypeDef *huart);
#ifdef __cplusplus
}
#endif
#endif
/*
*@}
*/
