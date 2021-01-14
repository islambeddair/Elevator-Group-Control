
/**
******************************************************************************
*@file    	User_Interface.h
*@authors	Eng.Akram Sayed <akramsayed.m@gmail.com>
*@date    	7/4/2014
*@authors	Eng.Islam Bedair <islambedair2@gmail.com>
*@date    	01/01/2020
*@brief   	This file contains All option of user interface module			.\n\n
*@Detail
* \b PROJECT   \n&nbsp;&nbsp; 		Duplex_Elevator_Control_System. 		\n
* \b H/W       \n&nbsp;&nbsp; 		Micro Controller STM8L151M8. 	\n
* \b S/W       \n&nbsp;&nbsp; 		IAR Systems IDE & Compiler.		\n
*****************************************************************************
*/
/*
* @defgroup 	Application
* @{
*/
#ifndef	_USER_INTERFACE_H
#define	_USER_INTERFACE_H
/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
 extern "C" {
#endif
/************************************* Headers ****************************************/
#include "main.h"   
#include "Config_File.h"
#include  "Elevator_Control.h"
#include  "Comm.h"
# include <string.h>
#include "ShiftIn165.h"
#include "ShiftOut595.h"
/****************************************************  Defines  *****************************************/
/************************************* Data Type ***************************************/
void SetRequestLed(void);
void   DecRequestStatuse(void);
uint16_t GetRequestStatus(void);
void   ClearAllRequests(void);
int CalcAverageWaitingTime(int ElevatorCounter, int floor_index,Direction_Enm_TypeDef direction); //calculat the average waiting time 
void selectionSort(int a[]) ;
uint8_t findSmallestElement(int Array[],uint8_t floor,Direction_Enm_TypeDef direction);
/************************************* External Variables ******************************/
/************************************* Functions ***************************************/

/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get all elevators  status
* @param	NOne
* @retval None
* @note
*/
void LandingCallsTimer(void);

void LandingCallsTimerScaner(void);

void SendCallsToElevatorWithSameCarPosition(void);

void HandleCallsForOutOffServiceElevator(void);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief      the user interface module task.
* @param	None
* @retval     None
* @note
*/
void user_interface_task(void);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief      Check new requests and arrange it in suitable format.
* @param	None
* @retval     None
* @note
*/
void check_new_req(void);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief        Get new requests.
* @param	None
* @retval       None
* @note
*/
void ElevatorDispatching(void);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief        We may have to wait a long time if there are too many passengers or not enough elevators. 
*               Just how long we wait depends on the dispatching strategy the elevators use to decide where to go. 
*               For example, if passengers on several floors have requested pickups, which should be served first? 
*               If there are no pickup requests, how should the elevators distribute themselves to await the next request? 
*               Elevator dispatching is a good example of a stochastic optimal control problem of economic importance 
*               that is too large to solve by classical techniques such as dynamic programming.
* @param	
* @retval     None
*/
void get_all_req(Request_TypeDef* all_request_copy,Direction_Enm_TypeDef direction);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get one request.
* @param	request_copy: is pointer of one request;
* @param	index: the index of require request;
* @retval None
* @note	all_request_copy must to be pointer to only one request
*/
void get_one_req(Request_TypeDef *,uint8_t,Direction_Enm_TypeDef direction);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  check a specific requesr if active ir not
* @param	index: the index of require request;
* @retval None
*/
uint8_t get_req_active(uint8_t,Direction_Enm_TypeDef direction);



void set_req_elevator_detection(uint8_t index, Direction_Enm_TypeDef direction, Request_Detection_Enm_TypeDef detection);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  assign elevator to the request
* @param	index: the index of require request;
* 		elevator: elevator number
* @retval None
*/
void set_req_assigned_elevator(uint8_t index,Direction_Enm_TypeDef direction,uint8_t);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get the request assigned elevator
* @param	index: the index of require request;
* @retval uint8_t
*/
uint8_t get_req_assigned_elevator(uint8_t index,Direction_Enm_TypeDef direction);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief        indicate if this request was sent or not to elevator
* @param	index: the index of require request;
* 		status: status
* @retval None
*/
void set_req_elevator_status(uint8_t index,Direction_Enm_TypeDef direction,Request_Elevator_TypeDef );
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief        indicate if this request was sent or not to elevator
* @param	index: the index of require request;
* @retval       Request_Elevator_TypeDef
*/
Request_Elevator_TypeDef get_req_elevator_status(uint8_t index,Direction_Enm_TypeDef direction);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief        clr handled request.
* @param	request_copy: is pointer of only one handled request;
* @retval       None
* @note	request_copy must to be pointer to only one request.
*/
//void clr_req(Request_TypeDef *);
void clr_req(uint8_t index,Direction_Enm_TypeDef direction);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief        Set user interface events.
* @param	None
* @retval       None
* @note
*/
void usr_inf_set_event(User_Inf_Events_DataType);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief        Clr user interface events.
* @param	None
* @retval       None
* @note
*/
void usr_inf_clr_event(User_Inf_Events_DataType);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief        check user interface events.
* @param	None
* @retval       None
* @note
*/
bool check_event_status(User_Inf_Events_DataType);
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief        init the user interface
* @param	None
* @retval       None
* @note
*/
void user_interface_init(void);
void TestRequist(void);
#ifdef __cplusplus
}
#endif
#endif /*__ _USER_INTERFACE_H */
/*
*@}
*/
