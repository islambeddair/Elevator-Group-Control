
/**
******************************************************************************
*@file    	Elevator_Control.h
*@authors	Eng.Akram Sayed <akramsayed.m@gmail.com>
*@date    	17/2/2014
*@authors	Eng.Islam Bedair <islambedair2@gmail.com>
*@date    	01/01/2020
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

#ifndef	_ELEVATOR_CONTROL_H
#define	_ELEVATOR_CONTROL_H
/************************************* Headers ****************************************/
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "User_Interface.h"
#include "Config_File.h"
#include <string.h>			/*!- include the memory copy and memory set functions*/
/************************************* Defines *****************************************/
/*!- Send event to Communication Module*/
#define	ELEVATOR_CTRL_READ_ELEVATOR_1()
/*!- Send event to Communication Module*/
#define	ELEVATOR_CTRL_READ_ELEVATOR_2()
/*!- Send event to Communication Module*/
#define	ELEVATOR_CTRL_SEND_ELEVATOR_1()
/*!- Send event to Communication Module*/
#define	ELEVATOR_CTRL_SEND_ELEVATOR_2()
/*!set NEW ORDERflag*/
#define ELEVATOR_CTRL_NEW_ORDER()		                                elevator_ctrl_set_event(ELEVATOR_CTRL_NEW_ORDER)
/*!- get elevators  status*/
#define ELEVATOR_CTRL_GET_ELEVATOR_STATUS(elevator_cpy,elevator_no)	 	elevator_ctrl_get_elevator_status(elevator_cpy,elevator_no)
/*!- set all elevators external requests*/
#define ELEVATOR_CTRL_SET_ELEVATOR_REQUETS(elevator_request_cpy,elevator_no) 	elevator_ctrl_set_elevator_requests(elevator_request_cpy,elevator_no)
/*!-get the current system state machine */
#define ELEVATOR_CTRL_CHK_STATE()                                               elevator_ctrl_get_state_machine()
/************************************* Data Type ***************************************/

/************************************* External Variables ******************************/
/************************************* Functions ***************************************/
void elevator_Set_elevator_Request(uint8_t elevator_no,uint8_t index,Direction_Enm_TypeDef direction);
void elevator_Clear_elevator_Request(uint8_t elevator_no,uint8_t index,Direction_Enm_TypeDef direction);
uint8_t elevator_Get_elevator_Request(uint8_t elevator_no,uint8_t index,Direction_Enm_TypeDef direction);
void ClrFramCounter(void);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  the elevator control module task.
* @param	None
* @retval None
* @note
*/
void elevator_control_task( void);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get all elevators  status
* @param	elevator_cpy: pointer to Elevator_TypeDefSystem_State_machine
* @retval None
* @note  elevator_cpy must to be array of TOTAL_ELEVATOR_NO.
*/
void elevator_ctrl_read_elevator_status(void);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  check the handled requests
* @param	NOne
* @retval None
* @note
*/
void elevator_ctrl_chk_handled_req(void);

/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get all elevators  status
* @param	NOne
* @retval None
* @note
*/
void elevator_ctrl_handle_req(void);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get elevators  status
* @param	elevator_cpy: pointer to Elevator_TypeDef
* @retval None
* @note  None.
*/
void elevator_ctrl_get_elevator_status(Elevator_TypeDef *elevator_cpy ,uint8_t elevator_no);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  set all elevators  status
* @param	elevator_cpy: pointer to Elevator_TypeDef
* @param	elevator_no: elevator number
* @retval None
* @note  elevator_cpy must to be array of TOTAL_ELEVATOR_NO.
*/
void elevator_ctrl_set_elevator_status(Elevator_TypeDef *,uint8_t);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  	set all elevators external requests
* @param	elevator_cpy: pointer to external requests
* @param	elevator_no: elevator number
* @retval None
*/
void elevator_ctrl_set_elevator_requests(Request_TypeDef *,uint8_t);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  	elevator_ctrl_check elevator availability
* @param	elevator_no: elevator number
* @retval 	Elevator_Status_Enm_TypeDef
*/
Elevator_Status_Enm_TypeDef elevator_ctrl_chk_elevator_availability(uint8_t elevator_no);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  Set elevator ctrl events.
* @param	event: any event from Elevator_Ctrl_Events_DataType
* @retval None
* @note
*/
void elevator_ctrl_set_event(Elevator_Ctrl_Events_DataType,uint8_t ElevatorNo);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  Set elevator ctrl events.
* @param	event: any event from Elevator_Ctrl_Events_DataType
* @retval None
* @note
*/
void elevator_ctrl_clr_event(Elevator_Ctrl_Events_DataType,uint8_t ElevatorNo);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  Check elevator ctrl events.
* @param	event: any event from Elevator_Ctrl_Events_DataType
* @retval Bool:   any Value-->True
* 				0-->False
* @note
*/
uint8_t elevator_ctrl_check_event(Elevator_Ctrl_Events_DataType,uint8_t ElevatorNo);


void elevator_set_elevator_availability(Elevator_Status_Enm_TypeDef StatusType,uint8_t elevator_no);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get the current system state machine
* @param	None
* @retval  the current system state machine
* @note
*/
System_State_Machine_Enm_TypeDef elevator_ctrl_get_state_machine(uint8_t elevator_no);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  	set the current system state machine
* @param	the current system state machine
* @retval  None
* @note
*/
void elevator_ctrl_set_state_machine(System_State_Machine_Enm_TypeDef,uint8_t elevator_no);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get elevators  position
* @param	elevator_no: Elevator Number
* @retval None
* @note  None.
*/
uint8_t elevator_ctrl_get_elevator_position(uint8_t elevator_no);
#ifdef __cplusplus
}
#endif
#endif 
/*
*@}
*/
