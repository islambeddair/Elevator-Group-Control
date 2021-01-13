
/**
******************************************************************************
*@file    	Elevator_Control.c
*@authors	Eng.Akram Sayed <akramsayed.m@gmail.com>
*@date    	17/2/2014
*@authors	Eng.Islam Bedair <islambedair2@gmail.com>
*@date    	01/01/2020
*@brief   	This file contains the main Elevator Control task.	\n\n
*@Detail
* \b PROJECT   \n&nbsp;&nbsp; 		Duplex_Elevator_Control_System. \n
* \b H/W       \n&nbsp;&nbsp; 		Micro Controller STM8L151M8. 	\n
* \b S/W       \n&nbsp;&nbsp; 		IAR Systems IDE & Compiler.     \n
*
*****************************************************************************
*/
/**
* @addtogroup 	Elevator_Control
* @{
*/
/************************************* Headers ****************************************/
#include "Elevator_Control.h"
/************************************* Headers ****************************************/
///*!- brief  get all requests.*/
#define ELEVATOR_CTRL_GET_ALL_REQ(all_request_copy,direction)		get_all_req(all_request_copy,direction)
///*!- clr handled request.*/
#define ELEVATOR_CTLR_CLR_REQ(all_request_copy,direction) 		clr_req(all_request_copy,direction)
#define ExternalRequestTimeOut                                           60 //60 Second
/************************************* Global Variables ******************************/
static System_State_Machine_Enm_TypeDef system_state_machine[TOTAL_ELEVATOR_NO];
static uint8_t elevator_ctrl_flags[TOTAL_ELEVATOR_NO];
static Elevator_TypeDef elevator[TOTAL_ELEVATOR_NO];
/*
={
{
IN_SERVICE,
FLOOR_1,
STOP,
NOT_MOVE,
CLOSED,
{0},
{0},
{0}
  },
{
IN_SERVICE,
FLOOR_1,
STOP,
NOT_MOVE,
CLOSED,
{0},
{0},
{0}
  }
};
*/
/************************************* Functions *************************************/
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief       the elevator control module task.
* @param       None
* @retval      None
* @note
*/
void elevator_control_task(void)
{
  static uint8_t elevator_no=0;
    if((system_state_machine[elevator_no]==IDLE_STATE)) /*!- check the state machine is idle state*/
    {
      if((elevator_ctrl_check_event(ELEVATOR_CTRL_SEND_ELEVATOR_ORDER_ACK,elevator_no))) /*!- Check if read elevator status done*/
      {
        system_state_machine[elevator_no]=CONTROL_STATE;  /*!-Set the state system machine*/
        //elevator_ctrl_chk_handled_req();     /*! check the handled requests*/
        //elevator_ctrl_set_event(ELEVATOR_CTRL_MONITOR_REQUESTS);        /*!- set the monitor requests flag */
        elevator_ctrl_clr_event(ELEVATOR_CTRL_SEND_ELEVATOR_ORDER_ACK,elevator_no);   /*!- Clear read elevators status ok*/
      }
    }
    /*!- check the state machine is control state*/
    if((system_state_machine[elevator_no]==CONTROL_STATE))
    {
      /*!- Check if the monitor request is activated or not*/
      if(elevator_ctrl_check_event(ELEVATOR_CTRL_MONITOR_REQUESTS,elevator_no))
      {
        //elevator_ctrl_monitor_requests();  /*!- Monitor all requests*/
        elevator_ctrl_clr_event(ELEVATOR_CTRL_MONITOR_REQUESTS,elevator_no);  /*!- Clear mONITOR REQUETS flag*/
      }
      system_state_machine[elevator_no]=IDLE_STATE;  /*!-Set the state system machine*/
    }
    if(elevator[elevator_no].status==IN_SERVICE)
      elevator_ctrl_set_event(ELEVATOR_CTRL_ELEVETOR_IN_SERVICE,elevator_no);
    else
      elevator_ctrl_set_event(ELEVATOR_CTRL_ELEVETOR_OUT_OF_SERVICE,elevator_no);
    
    elevator_no++;if(elevator_no==TOTAL_ELEVATOR_NO)elevator_no=0;
}
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief        check the handled requests
* @param	NOne
* @retval       None
* @note
*/

void elevator_ctrl_chk_handled_req(void)
{
  Request_TypeDef all_request_UP_copy[NUMBER_FLOOR];
  ELEVATOR_CTRL_GET_ALL_REQ(all_request_UP_copy,UP); //- Copy all current requests
  //Request_TypeDef all_request_DOWN_copy[NUMBER_FLOOR];
  //ELEVATOR_CTRL_GET_ALL_REQ(all_request_DOWN_copy,DOWN); //- Copy all current requests
  //Search on all floors and all directions
  for(uint8_t i=0;i<NUMBER_FLOOR;i++)
  {
    if(all_request_UP_copy[i].active)
    {
      //bool stat=true;
      //for(int ElevatorNo=0;ElevatorNo< TOTAL_ELEVATOR_NO;ElevatorNo++)
      //if(elevator[ElevatorNo].external_requestUP[i].active==0)stat=false;
      //if(stat==true)ELEVATOR_CTLR_CLR_REQ(all_request_UP_copy[i].active,UP);//!- clr handled request.
    }
  }
}


/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get all elevators  status
* @param	NOne
* @retval None
* @note
*/
void elevator_ctrl_handle_req(void)
{
  /*!- */
  for( uint8_t elevator_no=0;elevator_no< TOTAL_ELEVATOR_NO;elevator_no++)
  {
    /*!- Clear new request flag*/
    elevator_ctrl_clr_event(ELEVATOR_CTRL_NEW_REQ,elevator_no);
    /*!- Change the system state machine*/
    system_state_machine[elevator_no]=DECISIAN_MAKE_STATE;
  }
}
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get elevators  status
* @param	elevator_cpy: pointer to Elevator_TypeDef
* @retval None
* @note  None.
*/
void elevator_ctrl_get_elevator_status(Elevator_TypeDef *elevator_cpy,uint8_t elevator_no)
{
  memmove((Elevator_TypeDef*)elevator_cpy,(Elevator_TypeDef *)&elevator[elevator_no],sizeof(Elevator_TypeDef));
}
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  	set all elevators  status
* @param	elevator_cpy: pointer to Elevator_TypeDef
* @param	elevator_no: elevator number
* @retval None
* @note  elevator_cpy must to be array of TOTAL_ELEVATOR_NO.
*/
void elevator_ctrl_set_elevator_status(Elevator_TypeDef * elevator_cpy,uint8_t elevator_no)
{
  memmove((Elevator_TypeDef*)&elevator[elevator_no],(Elevator_TypeDef *)elevator_cpy,sizeof(Elevator_TypeDef));
}
/*
function to clear fram counter which inreased by 1 evry received fram from current elevator
*/
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  	set all elevators external requests
* @param	elevator_cpy: pointer to external requests
* @param	elevator_no: elevator number
* @retval       None
*/
void elevator_ctrl_set_elevator_requests(Request_TypeDef * elevator_request_cpy,uint8_t elevator_no)
{
  memmove((Request_TypeDef*)&elevator[elevator_no].external_requestUP,(Request_TypeDef *)elevator_request_cpy,NUMBER_FLOOR);
  memmove((Request_TypeDef*)&elevator[elevator_no].external_requestDOWN,(Request_TypeDef *)elevator_request_cpy,NUMBER_FLOOR);
}
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  	elevator_ctrl_check elevator availability
* @param	elevator_no: elevator number
* @retval 	Elevator_Status_Enm_TypeDef
*/
Elevator_Status_Enm_TypeDef elevator_ctrl_chk_elevator_availability(uint8_t elevator_no)
{
  return(elevator[elevator_no].status);
}
void elevator_set_elevator_availability(Elevator_Status_Enm_TypeDef StatusType,uint8_t elevator_no)
{
  elevator[elevator_no].status=StatusType;
}
inline void elevator_Clear_elevator_Request(uint8_t elevator_no,uint8_t index,Direction_Enm_TypeDef direction)
{
   if (direction==UP)
    elevator[elevator_no].external_requestUP[index].active=0;
  else if (direction==DOWN)
    elevator[elevator_no].external_requestDOWN[index].active=0;
}
void elevator_Set_elevator_Request(uint8_t elevator_no,uint8_t index,Direction_Enm_TypeDef direction)
{
   if (direction==UP)
    elevator[elevator_no].external_requestUP[index].active=1;
  else if (direction==DOWN)
    elevator[elevator_no].external_requestDOWN[index].active=1;
}
uint8_t elevator_Get_elevator_Request(uint8_t elevator_no,uint8_t index,Direction_Enm_TypeDef direction)
{
  uint8_t Active;
   if (direction==UP)
   Active= elevator[elevator_no].external_requestUP[index].active;
  else if (direction==DOWN)
   Active= elevator[elevator_no].external_requestDOWN[index].active;
  
  return Active;
}
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief       get elevators  position
* @param	elevator_no: Elevator Number
* @retval      None
* @note        None.
*/
uint8_t elevator_ctrl_get_elevator_position(uint8_t elevator_no)
{
  return(elevator[elevator_no].position);
}
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief        Set elevator ctrl events.
* @param	event: any event from Elevator_Ctrl_Events_DataType
* @param	elevator_no: elevator number
* @retval None
* @note
*/

void elevator_ctrl_set_event(Elevator_Ctrl_Events_DataType event,uint8_t ElevatorNo)
{
  switch(event)
  {
  case ELEVATOR_CTRL_NEW_REQ:
    elevator_ctrl_flags[ElevatorNo]|=ELEVATOR_CTRL_NEW_REQ;
    break;
  case ELEVATOR_CTRL_READ_STATUS_OK:
    elevator_ctrl_flags[ElevatorNo]|=ELEVATOR_CTRL_READ_STATUS_OK;
    break;
  case ELEVATOR_CTRL_NEW_SEC:
    elevator_ctrl_flags[ElevatorNo]|=ELEVATOR_CTRL_NEW_SEC;
    /*!- Set monitor requests task*/
    elevator_ctrl_flags[ElevatorNo]|=ELEVATOR_CTRL_MONITOR_REQUESTS;
    break;
  case ELEVATOR_CTRL_MONITOR_REQUESTS:
    elevator_ctrl_flags[ElevatorNo]|=ELEVATOR_CTRL_MONITOR_REQUESTS;
    break;
  case ELEVATOR_CTRL_NEW_ORDER:
    elevator_ctrl_flags[ElevatorNo]|=ELEVATOR_CTRL_NEW_ORDER;
    break;
  case ELEVATOR_CTRL_SEND_ELEVATOR_ORDER_ACK:
    elevator_ctrl_flags[ElevatorNo]|=ELEVATOR_CTRL_SEND_ELEVATOR_ORDER_ACK;
    break;
  case ELEVATOR_CTRL_ELEVETOR_OUT_OF_SERVICE: 
    elevator_ctrl_flags[ElevatorNo]|=ELEVATOR_CTRL_ELEVETOR_OUT_OF_SERVICE;
    break;
  case ELEVATOR_CTRL_ELEVETOR_IN_SERVICE:
    elevator_ctrl_flags[ElevatorNo]|=ELEVATOR_CTRL_ELEVETOR_IN_SERVICE;
    break;
  }
}
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief       Set elevator ctrl events.
* @param	event: any event from Elevator_Ctrl_Events_DataType
* @retval      None
* @note
*/
void elevator_ctrl_clr_event(Elevator_Ctrl_Events_DataType event ,uint8_t ElevatorNo)
{
  switch(event)
  {
  case ELEVATOR_CTRL_NEW_REQ:
    elevator_ctrl_flags[ElevatorNo]&=~ELEVATOR_CTRL_NEW_REQ;
    break;
  case ELEVATOR_CTRL_READ_STATUS_OK:
    elevator_ctrl_flags[ElevatorNo]&=~ELEVATOR_CTRL_READ_STATUS_OK;
    break;
  case ELEVATOR_CTRL_NEW_SEC:
    elevator_ctrl_flags[ElevatorNo]&=~ELEVATOR_CTRL_NEW_SEC;
    break;
  case ELEVATOR_CTRL_MONITOR_REQUESTS:
    elevator_ctrl_flags[ElevatorNo]&=~ELEVATOR_CTRL_MONITOR_REQUESTS;
    break;
  case ELEVATOR_CTRL_NEW_ORDER:
    elevator_ctrl_flags[ElevatorNo]&=~ELEVATOR_CTRL_NEW_ORDER;
    break;
  case ELEVATOR_CTRL_SEND_ELEVATOR_ORDER_ACK:
    elevator_ctrl_flags[ElevatorNo]&=~ELEVATOR_CTRL_SEND_ELEVATOR_ORDER_ACK;
    break;
  case ELEVATOR_CTRL_ELEVETOR_OUT_OF_SERVICE:
    elevator_ctrl_flags[ElevatorNo]&=~ELEVATOR_CTRL_ELEVETOR_OUT_OF_SERVICE;
    break;
  case ELEVATOR_CTRL_ELEVETOR_IN_SERVICE:
    elevator_ctrl_flags[ElevatorNo]&=~ELEVATOR_CTRL_ELEVETOR_IN_SERVICE;
    break;
  }
}
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  Check elevator ctrl events.
* @param	event: any event from Elevator_Ctrl_Events_DataType
* @retval Bool:   any Value-->True
* 				0-->False
* @note
*/
uint8_t elevator_ctrl_check_event(Elevator_Ctrl_Events_DataType event,uint8_t ElevatorNo)
{
  return (elevator_ctrl_flags[ElevatorNo]&event);
}
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get the current system state machine
* @param	None
* @retval  the current system state machine
* @note
*/
System_State_Machine_Enm_TypeDef elevator_ctrl_get_state_machine(uint8_t elevator_no)
{
  return(system_state_machine[elevator_no]);
}
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  	set the current system state machine
* @param	the current system state machine
* @retval      None
* @note
*/
void elevator_ctrl_set_state_machine(System_State_Machine_Enm_TypeDef current_state_machine,uint8_t elevator_no)
{
  system_state_machine[elevator_no]=current_state_machine;
}

