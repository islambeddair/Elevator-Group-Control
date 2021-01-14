
/**
  ******************************************************************************
  *@file    	Main.h
  *@authors		Eng.Akram Sayed <akramsayed.m@gmail.com>
  *@date    	17/2/2014
  *@brief   	This file contains All Options for My Project 			.\n\n
  *@Detail
  * \b PROJECT   \n&nbsp;&nbsp; 		Duplex_Elevator_Control_System. 		\n
  * \b H/W       \n&nbsp;&nbsp; 		Micro Controller STM8L151M8. 	\n
  * \b S/W       \n&nbsp;&nbsp; 		IAR Systems IDE & Compiler.		\n
  *
  *****************************************************************************
  */
/*
 * @defgroup 	Application
 * @{
 */
#ifndef	_CONFIG_FILE_h
#define	_CONFIG_FILE_h
/************************************* Headers ****************************************/
#ifdef __cplusplus
 extern "C" {
#endif
#include <stdio.h>
#include "string.h"
/************************************* Defines *****************************************/
#define	NUMBER_FLOOR		          16		/*!- Define the number of building floors*/
#define	MaxPossibleFloor		  32		/*!- Maximum Possible floors could be Added to system*/
#define TOTAL_ELEVATOR_NO	          4	
#define OutOfServiceCounterTimeout	  (uint16_t)30		/*!- 30 Sec time out*/
#define USR_INF_CHK_NEW_REQ_TIMEOUT	  (uint16_t)2	        /*!- 3 sec*/
#define FLOOR_SWITCH_NO		          (uint8_t)2				   /*!- used to define the number of switches per floor*/
#define TOTAL_SWITCHES_NO	          (uint8_t)(NUMBER_FLOOR*FLOOR_SWITCH_NO-2)  /*!- used to define total number of building switched without UP16 and DOWN1*/
#define REQUEST_DOWN_SECTION              (TOTAL_SWITCHES_NO/2)
#define UART_ELEVATOR_1		          huart6
#define UART_ELEVATOR_2		          huart1
#define UART_ELEVATOR_3		          huart4
#define UART_ELEVATOR_4	                  huart5    
typedef struct
{
  GPIO_TypeDef* Port;//[4]={GPIOB,GPIOB,GPIOB,GPIOA};
  uint16_t Pin;//[4]={GPIO_PIN_13,GPIO_PIN_14,GPIO_PIN_15,GPIO_PIN_8};
}StatusLed;
/************************************* Data Type ***************************************/
typedef enum
{
	FLOOR_1=0,
	FLOOR_2=1,
	FLOOR_3=2,
	FLOOR_4=3,
	FLOOR_5=4,
	FLOOR_6=5,
	FLOOR_7=6,
	FLOOR_8=7,
	FLOOR_9=8,
	FLOOR_10=9,
	FLOOR_11=10,
	FLOOR_12=11,
	FLOOR_13=12,
	FLOOR_14=13,
	FLOOR_15=14,
	FLOOR_16=15,
	TOTAL_FLOOR_NO=16,
}Position_Enm_TypeDef;
typedef enum
{       
        STOP =0X00,     /*!- used if the elevator hasn't ant internal or external requests*/
	UP   =0X01,
	DOWN =0X02,
	BOTH =0x08      /*!- used if the same floor has the two different direction requests*/
}Direction_Enm_TypeDef;
typedef enum
{
  UP_SW_LED=0x00,
  DOWN_SW_LED=0x01,
}Floor_Switch_Led_TypeDef;
typedef enum
{
	NOT_MOVE = 0X00,
	SLOW = 0X04,
	FAST = 0X08,				
}Speed_Enm_TypeDef;
/************************************* Data Type ***************************************/
/*!- Describe the gpio for each floor*/
typedef enum
{
  FLOOR_NO_1,
  FLOOR_NO_2,
  FLOOR_NO_3,
  FLOOR_NO_4,
  FLOOR_NO_5,
  FLOOR_NO_6,
  FLOOR_NO_7,
  FLOOR_NO_8,
  FLOOR_NO_9,
  FLOOR_NO_10,
  FLOOR_NO_11,
  FLOOR_NO_12,
  FLOOR_NO_13,
  FLOOR_NO_14,
  FLOOR_NO_15,
  FLOOR_NO_16,
  END_FLOOR,
}Floor_No_TypeDef;
/************************************* External Variables ******************************/
/************************************* Functions ***************************************/
typedef enum
{
  NOT_SENT=(uint8_t)0x00,
  SENT=(uint8_t)0x01,
}Request_Elevator_TypeDef;
typedef enum
{
  USER_INF_NEW_REQ= (uint16_t)0x00001,		        /*!- new request flag*/
  USER_INF_SET_SHT_REG_LEDS= (uint16_t)0x00002,		/*!- Send data to shift register flag*/
  USER_INF_SET_NEW_TICKS_EVENT= (uint16_t)0x00004,
}User_Inf_Events_DataType;
typedef enum
{
  NOT_DETECTED,
  DETECTED
}Request_Detection_Enm_TypeDef;
typedef enum
{
  ASSIGNED_ELEVATOR_1,
  ASSIGNED_ELEVATOR_2,
  ASSIGNED_ELEVATOR_3,
  ASSIGNED_ELEVATOR_4,
  NOT_ASSIGNED,
}Request_Assigned_Elv_Enm_TypeDef;
typedef struct
{
  uint8_t active;
  uint8_t position;
  //Direction_Enm_TypeDef direction;
  uint16_t timer;
}Request_TypeDef;
typedef struct
{
  uint8_t active;
  uint8_t position;
}Request_Internal_TypeDef;
 /*!- Specify the system state machine*/
typedef enum
{
  IDLE_STATE,
  COMM_STATE,
  DECISIAN_MAKE_STATE,
  CONTROL_STATE,
}System_State_Machine_Enm_TypeDef;

typedef enum
{
  ELEVATOR_CTRL_NEW_REQ= 					(uint8_t)0x01,		/*!- Activated from User interface Module*/
  ELEVATOR_CTRL_NEW_SEC= 					(uint8_t)0x02,		/*!- Activated from RTC*/
  ELEVATOR_CTRL_READ_STATUS_OK= 				(uint8_t)0x04,		/*!- Activated from Elevator Control Module*/
  ELEVATOR_CTRL_MONITOR_REQUESTS=				(uint8_t)0x08,		/*!- Activated from Communication Module*/
  ELEVATOR_CTRL_NEW_ORDER= 					(uint8_t)0x10,		/*!- Activated from Decision Maker Module*/
  ELEVATOR_CTRL_SEND_ELEVATOR_ORDER_ACK= 			(uint8_t)0x20,		/*!- Activated from Communication Module*/
  ELEVATOR_CTRL_ELEVETOR_OUT_OF_SERVICE= 			(uint8_t)0x40,
  ELEVATOR_CTRL_ELEVETOR_IN_SERVICE= 				(uint8_t)0x80,
}Elevator_Ctrl_Events_DataType;

typedef enum
{
  IN_SERVICE,
  OUT_SERVICE
}Elevator_Status_Enm_TypeDef;
typedef enum
{
  ELEVATOR_1,
  ELEVATOR_2,
  ELEVATOR_3,
  ELEVATOR_4,
  TOTAL_NO_ELEVETOR
}Elevator_No_TypeDef;
typedef enum
{
  CLOSED,
  OPENED
}Door_Status_Enm_TypeDef;
typedef struct
{
  Elevator_Status_Enm_TypeDef  status;
  uint8_t                      position;
  Direction_Enm_TypeDef        direction;
  Speed_Enm_TypeDef            Speed;
  Door_Status_Enm_TypeDef      door_status;
  uint8_t                      MaxFloor;
  uint8_t                      StoppageTime;
  uint16_t                     fit;
  Request_TypeDef              internal_request[NUMBER_FLOOR];
  Request_TypeDef              external_requestUP[NUMBER_FLOOR];
  Request_TypeDef              external_requestDOWN[NUMBER_FLOOR];
}Elevator_TypeDef;
#if 0
Elevator_TypeDef elevator[TOTAL_ELEVATOR_NO]=
{
  /*!- Elevator #1*/
  {
    IN_SERVICE,
    2,
    UP,
    CLOSED,
    {{0}},
    {{0}},
  },
};
#endif
typedef struct
{
  Request_TypeDef request;
  Request_Detection_Enm_TypeDef detection;
  uint8_t assigned_elv;
  Request_Elevator_TypeDef request_elevator_status;              /*!- indicate if this request already sent to elevator or not*/
}Request_User_Inf_TypeDef;
/*
 * Version name
 *
 * 			x.y.zz_mm
 *
 * x----> 	0	: 	Development version
 * 			1	:	Release version
 *
 * y---->	0	:	Not Tested
 * 			1	: 	Tested
 *
 * zz--->		:	most significant Number of version	"increment Ascending "
 * mm--->		:	lest significant Number of version	"increment Ascending "
 *
 * */
#ifdef __cplusplus
}
#endif
#endif /*__ _CONFIG_FILE_h */
/*
*@}
*/
