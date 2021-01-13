
/**
******************************************************************************
*@authors	Eng.Islam Bedair <islambedair2@gmail.com>
*@date    	01/01/2020
*@brief   	This file contains the user interface module handler.		\n\n
*@Detail		        0)Set Led ERROR if one of them is OUT_SERVICE and clear it if all are IN_SERVICE
1) At least we must have one Elevator IN_SERVICE to to do any thing
*                               2) check if  one of them is OUT_SERVICE and has external request 
*                                  then we move all this request completly to the second Elevator
*                               3) Read all Switches and set active requests in suitable format and Set all active requests leds.
*                                       <------ All building requests = (Floor_NO * Sw_No_Per_Floor)-2 =30 Request ----->
*				        <--------- Up Requests	------------>	 <-------------	Down Requests	-------->
*				        from 0			        to 15	 from  0			    to 15 
*				4) Calculat Average waiting time of every elevator according to traveling time and stoppage time 
a)traveling time is FloorTime multiplied by traveling floor numbers..
b)Stoppage time is numbers of external call and internal car calls withen car position to landing call...
4) select minimum AWT and Send new requests.
5)check if all elevators are OUT_SERVICE for OutOfServiceCounterTimeout now is 10 Sec 
*				4) Remove all handled requests according to Elevator control module.
* \b PROJECT   \n&nbsp;&nbsp; 	Duplex_Elevator_Control_System. 		\n
* \b H/W       \n&nbsp;&nbsp; 	Micro Controller STM8L151M8.                    \n
* \b S/W       \n&nbsp;&nbsp; 	IAR Systems IDE & Compiler.	                \n
*****************************************************************************
*/
/**
* @addtogroup 	User_Interface
* @{
*/
/************************************* Headers ****************************************/
#include "User_Interface.h"
#include "Elevator_Control.h"
/************************************* Data types ****************************************/
/************************************* Global Variables ******************************/
static Request_User_Inf_TypeDef all_requestsUP[NUMBER_FLOOR];       //total landing call mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
static Request_User_Inf_TypeDef all_requestsDOWN[NUMBER_FLOOR];     //total landing call mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
static uint16_t                 user_inf_flags;
#define ExternalRequestTIMEOUT   60  // 60 Second 
///////////////////////////////////////////////

/************************************* Functions *************************************/
/** @auther Islam Bedair <islambedair2@gmail.com>
* @brief    the user interface module task.
* @param    None
* @retval   None
* @note
*/  
void user_interface_task(void)
{
  if(GetOutOfServiceCounter()>OutOfServiceCounterTimeout)
    ClearAllRequests();
  else
  {
    if(Getusr_inf_chk_new_req_counter()>USR_INF_CHK_NEW_REQ_TIMEOUT)
    {
      HandleCallsForOutOffServiceElevator();
      //LandingCallsTimer();
      //LandingCallsTimerScaner();
      Rstusr_inf_chk_new_req_counter();
    }
    check_new_req();                            // Check new requests and arrange it in suitable format
    SetRequestLed();                             // Set Led of all active Request 
    SendCallsToElevatorWithSameCarPosition();   // Send Calls To Elevator With Same Car Position
    ElevatorDispatching();                      // dispatching strategy the elevators use to decide where to go.
  }
}

/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        Check new requests and arrange it in suitable format.
* @param	None
* @retval       None
* @note
*/
void check_new_req(void)
{
  //////////////////////////////////////////////////////////////////////// Check up requests /////////////////////////
  uint8_t LandingCall[4];
  //for(int i=0;i<4;i++){LandingCall[i]=0xFF;}
  read_shift_regsUp(LandingCall,MaxPossibleFloor);
  for (uint8_t floor_index = 0;floor_index < NUMBER_FLOOR;floor_index++)
  {
    if((((LandingCall[floor_index/8]>>(floor_index%8))&0x01)==0)&&(all_requestsUP[floor_index].request.active==0))
    {
      all_requestsUP[floor_index].request.active = 1;
      all_requestsUP[floor_index].request.position = floor_index;
      all_requestsUP[floor_index].detection = NOT_DETECTED;
      all_requestsUP[floor_index].request_elevator_status = NOT_SENT;
      all_requestsUP[floor_index].assigned_elv = NOT_ASSIGNED;
      all_requestsUP[floor_index].request.timer = 0;
    }
   // if(all_requestsUP[floor_index].request.active!=0)LandingCall[floor_index/8]&=~(1<<(floor_index%8));
    //else LandingCall[floor_index/8]|=(1<<(floor_index%8));
  }
  //ShiftOutLandingUp(LandingCall,MaxPossibleFloor);
  //for(int i=0;i<4;i++){LandingCall[i]=0xFF;}
  read_shift_regsDN(LandingCall,MaxPossibleFloor);
  for (uint8_t floor_index = 0;floor_index < NUMBER_FLOOR;floor_index++)
  {
    /////////////////////////////////////////////////////////////////////// Check DOWN requests ////////////////////////////
    if((((LandingCall[floor_index/8]>>(floor_index%8))&0x01)==0)&&(all_requestsDOWN[floor_index].request.active==0))
    {
      all_requestsDOWN[floor_index].request.active = 1;
      all_requestsDOWN[floor_index].request.position = floor_index;
      all_requestsDOWN[floor_index].detection = NOT_DETECTED;
      all_requestsDOWN[floor_index].request_elevator_status = NOT_SENT;
      all_requestsDOWN[floor_index].assigned_elv = NOT_ASSIGNED;
      all_requestsDOWN[floor_index].request.timer = 0;
    }
    //if(all_requestsDOWN[floor_index].request.active==1) LandingCall[(floor_index/8)]&=~(1<<(floor_index%8));else LandingCall[(floor_index/8)]|=(1<<(floor_index%8));
  }
  //ShiftOutLandingDN(LandingCall,MaxPossibleFloor);
}
void SetRequestLed(void)
{
  uint8_t LandingCall[4];
  for(int i=0;i<4;i++){LandingCall[i]=0xFF;}
  for (uint8_t floor_index = 0;floor_index < NUMBER_FLOOR;floor_index++)
  {
      if(all_requestsUP[floor_index].request.active==1) 
        LandingCall[(floor_index/8)]&=~(1<<(floor_index%8));
  }
  ShiftOutLandingUp(LandingCall,MaxPossibleFloor);
  for(int i=0;i<4;i++){LandingCall[i]=0xFF;}
  for (uint8_t floor_index = 0;floor_index < NUMBER_FLOOR;floor_index++)
  {
      if(all_requestsDOWN[floor_index].request.active==1) 
        LandingCall[(floor_index/8)]&=~(1<<(floor_index%8));
  }
  ShiftOutLandingDN(LandingCall,MaxPossibleFloor);
}
void HandleCallsForOutOffServiceElevator(void)
{
  for(uint8_t ElevatorNumber=0;ElevatorNumber<TOTAL_ELEVATOR_NO;ElevatorNumber++)
  {
    if(elevator_ctrl_chk_elevator_availability(ElevatorNumber)==OUT_SERVICE)
    {
      //printf("HandleCallsForOutOffServiceElevator =%d\r\n",ElevatorNumber);
      
      /*!- Search on all floors and all directions*/
      for (uint8_t floor_index = 0; floor_index < NUMBER_FLOOR; floor_index++)
      {
        //!- external requests Up 
        if(elevator_Get_elevator_Request(ElevatorNumber,floor_index,UP)==1)
        {
          elevator_Clear_elevator_Request( ElevatorNumber,floor_index,UP);
          all_requestsUP[floor_index].request.active = 1;
          all_requestsUP[floor_index].request.position = floor_index;
          all_requestsUP[floor_index].detection = NOT_DETECTED;
          all_requestsUP[floor_index].request_elevator_status = NOT_SENT;
          all_requestsUP[floor_index].assigned_elv = NOT_ASSIGNED;
          all_requestsUP[floor_index].request.timer = 0;
        }
        //!- external requests DOWN
        if(elevator_Get_elevator_Request(ElevatorNumber,floor_index,DOWN)==1)
        {
          elevator_Clear_elevator_Request(ElevatorNumber,floor_index,DOWN);
          all_requestsDOWN[floor_index].request.active = 1;
          all_requestsDOWN[floor_index].request.position = floor_index;
          all_requestsDOWN[floor_index].detection = NOT_DETECTED;
          all_requestsDOWN[floor_index].request_elevator_status = NOT_SENT;
          all_requestsDOWN[floor_index].assigned_elv = NOT_ASSIGNED;
          all_requestsDOWN[floor_index].request.timer = 0;
        }
      } 
    }
  }
}
/** @auther	Akram Sayed <akramsayed.m@gmail.com>
* @brief  get all elevators  status
* @param	NOne
* @retval None
* @note
*/
void LandingCallsTimerScaner(void)
{
  /*!- Search on all floors and all directions*/
  for (uint8_t floor_index = 0; floor_index < NUMBER_FLOOR; floor_index++)
  {
    //!- external requests UP 
    if(all_requestsUP[floor_index].request.timer >ExternalRequestTIMEOUT)
    {
      elevator_Clear_elevator_Request((get_req_assigned_elevator(floor_index,UP)),floor_index,UP);
      all_requestsUP[floor_index].request.active = 1;
      all_requestsUP[floor_index].request.position = floor_index;
      all_requestsUP[floor_index].detection = NOT_DETECTED;
      all_requestsUP[floor_index].request_elevator_status = NOT_SENT;
      all_requestsUP[floor_index].assigned_elv = NOT_ASSIGNED;
      all_requestsUP[floor_index].request.timer = 0;
    }
    //!- external requests DOWN
    if(all_requestsDOWN[floor_index].request.timer > ExternalRequestTIMEOUT)
    {
      elevator_Clear_elevator_Request((get_req_assigned_elevator(floor_index,DOWN)),floor_index,DOWN);
      all_requestsDOWN[floor_index].request.active = 1;
      all_requestsDOWN[floor_index].request.position = floor_index;
      all_requestsDOWN[floor_index].detection = NOT_DETECTED;
      all_requestsDOWN[floor_index].request_elevator_status = NOT_SENT;
      all_requestsDOWN[floor_index].assigned_elv = NOT_ASSIGNED;
      all_requestsDOWN[floor_index].request.timer = 0;
    }
  } 
}
void SendCallsToElevatorWithSameCarPosition(void) 
{
  for (uint8_t floor_index = 0;floor_index < NUMBER_FLOOR;floor_index++)
  {
    Elevator_TypeDef elevator_cpy;
    if ((all_requestsUP[floor_index].request.active==1) && (all_requestsUP[floor_index].detection == NOT_DETECTED))
    {
      for(uint8_t ElevatorNumber=0;ElevatorNumber<TOTAL_ELEVATOR_NO;ElevatorNumber++)
      {
        elevator_ctrl_get_elevator_status((Elevator_TypeDef*)&elevator_cpy,ElevatorNumber);
        if((elevator_cpy.status == IN_SERVICE)&& (elevator_cpy.Speed == NOT_MOVE)&&((elevator_cpy.direction==UP)||(elevator_cpy.direction==STOP))&&(elevator_cpy.position == floor_index)) 
        {
          elevator_Set_elevator_Request(ElevatorNumber,floor_index,UP);
          all_requestsUP[floor_index].detection = DETECTED;
          all_requestsUP[floor_index].assigned_elv = ElevatorNumber;
          all_requestsUP[floor_index].request_elevator_status = NOT_SENT;
          break;
        }
      }
    }
    if ((all_requestsDOWN[floor_index].request.active==1) && (all_requestsDOWN[floor_index].detection == NOT_DETECTED))
    {
      for(uint8_t ElevatorNumber=0;ElevatorNumber<TOTAL_ELEVATOR_NO;ElevatorNumber++)
      {
        elevator_ctrl_get_elevator_status((Elevator_TypeDef*)&elevator_cpy,ElevatorNumber);
        if((elevator_cpy.status==IN_SERVICE)&&(elevator_cpy.Speed==NOT_MOVE)&&((elevator_cpy.direction==DOWN)||(elevator_cpy.direction==STOP))&&(elevator_cpy.position==floor_index)) 
        {
          elevator_Set_elevator_Request(ElevatorNumber,floor_index,DOWN);
          all_requestsDOWN[floor_index].detection = DETECTED;
          all_requestsDOWN[floor_index].assigned_elv = ElevatorNumber;
          all_requestsDOWN[floor_index].request_elevator_status = NOT_SENT;
          break;
        }
      }
    }
  }
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        She dispatching strategy the elevators use to decide where to go.
* @param	None
* @retval       None
* @note
*/
void ElevatorDispatching(void)//the dispatching strategy the elevators use to decide where to go
{
  int  WaitingTimes[TOTAL_ELEVATOR_NO];
  uint8_t ElevatorIndex=0;
  for (uint8_t floor_index = 0;floor_index < NUMBER_FLOOR;floor_index++)
  {
    if ((all_requestsUP[floor_index].request.active==1) && (all_requestsUP[floor_index].detection == NOT_DETECTED))
    {
      for (uint8_t ElevatorNumber = 0; ElevatorNumber < TOTAL_ELEVATOR_NO; ElevatorNumber++)
        WaitingTimes[ElevatorNumber] = CalcAverageWaitingTime(ElevatorNumber,floor_index,UP);
      ElevatorIndex=findSmallestElement(WaitingTimes,TOTAL_ELEVATOR_NO);
      elevator_Set_elevator_Request(ElevatorIndex,floor_index,UP);
      all_requestsUP[floor_index].detection = DETECTED;
      all_requestsUP[floor_index].assigned_elv = ElevatorIndex;
      all_requestsUP[floor_index].request_elevator_status = NOT_SENT;
    }
    if ((all_requestsDOWN[floor_index].request.active==1) && (all_requestsDOWN[floor_index].detection == NOT_DETECTED))
    {
      for (uint8_t ElevatorNumber = 0; ElevatorNumber < TOTAL_ELEVATOR_NO; ElevatorNumber++)
        WaitingTimes[ElevatorNumber] = CalcAverageWaitingTime(ElevatorNumber,floor_index,DOWN);
      ElevatorIndex=findSmallestElement(WaitingTimes,TOTAL_ELEVATOR_NO);
      elevator_Set_elevator_Request(ElevatorIndex,floor_index,DOWN);
      all_requestsDOWN[floor_index].detection = DETECTED;
      all_requestsDOWN[floor_index].assigned_elv =ElevatorIndex;
      all_requestsDOWN[floor_index].request_elevator_status = NOT_SENT;
    }
  }
}
//for(int floor=0;floor<=15;floor++){USER_INF_SET_LED(floor, DOWN );USER_INF_SET_LED(floor, UP );HAL_Delay(100);clr_req(floor, DOWN);clr_req(floor, UP);HAL_Delay(100);}
//for(int floor=15;floor>=0;floor--){USER_INF_SET_LED(floor, DOWN );USER_INF_SET_LED(floor, UP );HAL_Delay(100);clr_req(floor, DOWN);clr_req(floor, UP);HAL_Delay(100);}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief      get all requests.
* @param	all_request_copy: is pointer of all requests;
* @retval     None
* @note	all_request_copy must to be pointer to array of all requests, not only one request.
*/
void get_all_req(Request_TypeDef* all_request_copy,Direction_Enm_TypeDef direction)
{
  if(direction==UP)
  {
    for (uint8_t i = 0; i < NUMBER_FLOOR; i++)
    {
      memcpy((Request_TypeDef*)all_request_copy, (Request_TypeDef*)&all_requestsUP[i].request, sizeof(Request_TypeDef));
    }
  }
  else// if(direction==DOWN)
  {
    for (uint8_t i = 0; i < NUMBER_FLOOR; i++)
    {
      memcpy((Request_TypeDef*)all_request_copy, (Request_TypeDef*)&all_requestsDOWN[i].request, sizeof(Request_TypeDef));
    }
  }
}
void LandingCallsTimer(void)
{
  for (uint8_t floor_index = 0;floor_index < NUMBER_FLOOR;floor_index++)
  {
    if(all_requestsUP[floor_index].request.active !=0)
    {
      all_requestsUP[floor_index].request.timer+=USR_INF_CHK_NEW_REQ_TIMEOUT;
    }
    if(all_requestsDOWN[floor_index].request.active !=0)
    {
      all_requestsDOWN[floor_index].request.timer+=USR_INF_CHK_NEW_REQ_TIMEOUT;
    }
  }
}
/** @auther   Islam Bedair <islambedair2@gmail.com>
* @brief      get one request.
* @param      request_copy: is pointer of one request;
* @param      index: the index of require request;
* @retval     None
* @note	      all_request_copy must to be pointer to only one request
*/
void get_one_req(Request_TypeDef* request_copy,uint8_t index,Direction_Enm_TypeDef direction)
{
  if (direction==UP)
    memcpy((Request_TypeDef*)request_copy, (Request_TypeDef*)&all_requestsUP[index], sizeof(Request_TypeDef));
  else if (direction==DOWN)
    memcpy((Request_TypeDef*)request_copy, (Request_TypeDef*)&all_requestsDOWN[index], sizeof(Request_TypeDef));
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        check a specific requesr if active or not
* @param	index: the index of require request;
* @retval       None
*/

uint8_t get_req_active(uint8_t index,Direction_Enm_TypeDef direction)
{
  uint8_t ActiveStatus;
  if (direction==UP)
    ActiveStatus= (all_requestsUP[index].request.active);
  else if (direction==DOWN)
    ActiveStatus= (all_requestsDOWN[index].request.active);
  return ActiveStatus;
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        assign elevator to the request
* @param	index: the index of require request;
* 		elevator: elevator number
* @retval       None
*/
void set_req_assigned_elevator(uint8_t index, Direction_Enm_TypeDef direction,uint8_t elevator)
{
  if (direction==UP)
    all_requestsUP[index].assigned_elv = elevator;
  else if (direction==DOWN)
    all_requestsDOWN[index].assigned_elv = elevator;
}

/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        get the request assigned elevator
* @param	index: the index of require request;
* @retval       Request_Assigned_Elv_Enm_TypeDef
*/

uint8_t get_req_assigned_elevator(uint8_t index, Direction_Enm_TypeDef direction)
{
  uint8_t elevator;
  if(direction==UP)
    elevator=(all_requestsUP[index].assigned_elv);
  else if (direction==DOWN) 
    elevator=(all_requestsDOWN[index].assigned_elv);
  return elevator;
}

/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        indicate if this request was sent or not to elevator
* @param        index: the index of require request;
* 		status: status
* @retval       None
*/

void set_req_elevator_detection(uint8_t index, Direction_Enm_TypeDef direction, Request_Detection_Enm_TypeDef detection)
{
  if(direction==UP)
    all_requestsUP[index].detection = detection;
  else if (direction==DOWN)
    all_requestsDOWN[index].detection = detection;
}

void set_req_elevator_status(uint8_t index, Direction_Enm_TypeDef direction, Request_Elevator_TypeDef status)
{
  if(direction==UP)
    all_requestsUP[index].request_elevator_status = status;
  else if (direction==DOWN)
    all_requestsDOWN[index].request_elevator_status = status;
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        indicate if this request was sent or not to elevator
* @param	index: the index of require request;
* @retval       Request_Elevator_TypeDef
*/

Request_Elevator_TypeDef get_req_elevator_status(uint8_t index, Direction_Enm_TypeDef direction)
{
  Request_Elevator_TypeDef request;
  if(direction==UP)
    request=(all_requestsUP[index].request_elevator_status);
  else if (direction==DOWN) 
    request=(all_requestsDOWN[index].request_elevator_status);
  return request;
}
/*
need to be revised
*/
void clr_req(uint8_t index,Direction_Enm_TypeDef direction)
{
  if (direction == UP)
  {
    USER_INF_CLR_LED(index, UP);
    all_requestsUP[index].request.active = 0;
    all_requestsUP[index].request.timer = 0;
    all_requestsUP[index].assigned_elv = NOT_ASSIGNED;
    all_requestsUP[index].detection = NOT_DETECTED;
    all_requestsUP[index].request_elevator_status = NOT_SENT;
  }
  else if(direction==DOWN)
  {
    USER_INF_CLR_LED(index, DOWN);
    all_requestsDOWN[index].request.active = 0;
    all_requestsDOWN[index].request.timer = 0;
    all_requestsDOWN[index].assigned_elv = NOT_ASSIGNED;
    all_requestsDOWN[index].detection = NOT_DETECTED;
    all_requestsDOWN[index].request_elevator_status = NOT_SENT;
  }
}

/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        Set user interface events.
* @param	None
* @retval       None
* @note
*/

void usr_inf_set_event(User_Inf_Events_DataType event)
{
  user_inf_flags |= USER_INF_NEW_REQ;
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        Clr user interface events.
* @param	None
* @retval       None
* @note
*/
void usr_inf_clr_event(User_Inf_Events_DataType event)
{
  user_inf_flags &= ~USER_INF_NEW_REQ;
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        check user interface events.
* @param	None
* @retval       None
* @note
*/
bool check_event_status(User_Inf_Events_DataType event)
{
  bool result = (bool)((User_Inf_Events_DataType)user_inf_flags & event);
  return (result);
}
/** @auther   Islam Bedair <islambedair2@gmail.com>
* @brief      init the user interface
* @param      None
* @retval     None
* @note
*/
void user_interface_init(void)
{
  for (uint8_t floor = FLOOR_1; floor < TOTAL_FLOOR_NO; floor++)
  {
    all_requestsUP[floor].assigned_elv = NOT_ASSIGNED;
    all_requestsDOWN[floor].assigned_elv = NOT_ASSIGNED;
  }
}
void ClearAllRequests(void)
{
  USER_INF_CLR_ALL();
  for(uint8_t ElevatorNumber=0;ElevatorNumber<TOTAL_ELEVATOR_NO;ElevatorNumber++)
  {
    for (uint8_t floor_index = FLOOR_1; floor_index < NUMBER_FLOOR; floor_index++)
    {
      elevator_Clear_elevator_Request(ElevatorNumber ,floor_index,UP);
      elevator_Clear_elevator_Request(ElevatorNumber ,floor_index,DOWN);
    }
  }
  for (uint8_t floor_index = FLOOR_1; floor_index < NUMBER_FLOOR; floor_index++)
  {
    //!- external requests DOWN
    all_requestsUP[floor_index].request.active = 0;
    all_requestsUP[floor_index].request.position = floor_index;
    all_requestsUP[floor_index].detection = NOT_DETECTED;
    all_requestsUP[floor_index].request_elevator_status = NOT_SENT;
    all_requestsUP[floor_index].assigned_elv = NOT_ASSIGNED;
    all_requestsUP[floor_index].request.timer = 0;
    //!- external requests DOWN
    all_requestsDOWN[floor_index].request.active = 0;
    all_requestsDOWN[floor_index].request.position = floor_index;
    all_requestsDOWN[floor_index].detection = NOT_DETECTED;
    all_requestsDOWN[floor_index].request_elevator_status = NOT_SENT;
    all_requestsDOWN[floor_index].assigned_elv = NOT_ASSIGNED;
    all_requestsDOWN[floor_index].request.timer = 0;
  }
}
#define FloorTime 3
int CalcAverageWaitingTime(int ElevatorCounter, int floor_index,Direction_Enm_TypeDef direction) //calculat the average waiting time 
{
  Elevator_TypeDef elevator_cpy;
  int RegCall = 0;
  int i = 0;
  int TravelingTime = 0;                       //traveling time
  int TotalStopageTime = 0;                    //Stop Time
  int minimum=0;
  int maximum=NUMBER_FLOOR;
  elevator_ctrl_get_elevator_status((Elevator_TypeDef*)&elevator_cpy,ElevatorCounter);
  switch (direction)
  {
  case UP:
    {
      //printf("(all_requests.direction** UP **)\r\n");
      if ((all_requestsUP[floor_index].request.position > elevator_cpy.position))
      {
        /*! Car position is < Request Position*/
        //printf("/*! Car position is < Request Position*/\r\n");
        RegCall = 0;
        if (elevator_cpy.direction == UP)
        {
          //printf("(elevator_cpy.direction == UP)\r\n");
          TravelingTime = (all_requestsUP[floor_index].request.position - elevator_cpy.position) * FloorTime;
          for (i = elevator_cpy.position; i < all_requestsUP[floor_index].request.position; i++)
          {
            if((elevator_cpy.internal_request[i].active != 0) || (elevator_cpy.external_requestUP[i].active != 0))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;           //clear Car Call
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
        else if (elevator_cpy.direction == DOWN)
        {
          //printf("(elevator_cpy.direction == DOWN)\r\n");
          minimum=elevator_cpy.position;
          for (int floor = 0; floor <elevator_cpy.position; floor++)
          {
            if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
            {
              minimum = floor;
              break;
            }
          }
          TravelingTime = ((elevator_cpy.position - minimum) + (all_requestsUP[floor_index].request.position - minimum)) * FloorTime;
          for (i = minimum; i < all_requestsUP[floor_index].request.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || (elevator_cpy.external_requestUP[i].active != 0))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;           //clear Car Call
              RegCall++;
            }
          }
          for (i = minimum; i <= elevator_cpy.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
            {
              elevator_cpy.internal_request[i].active = 0;                            //clear Car Call
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
        else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE))
        {
          //printf("(elevator_cpy.direction == STOP)\r\n");
          TravelingTime = (all_requestsUP[floor_index].request.position - elevator_cpy.position) * FloorTime;
          for (i = elevator_cpy.position; i < all_requestsUP[floor_index].request.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;           //clear Car Call
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
      }                            /*! Car is under Request */
      else if ((all_requestsUP[floor_index].request.position < elevator_cpy.position))
      {
        /*! Car position is > Request Position*/
        //printf("/*! Car position is > Request Position*/\r\n");
        RegCall = 0;
        if ((elevator_cpy.direction == UP))
        {
          maximum=elevator_cpy.position;
          for (int floor = (NUMBER_FLOOR-1); floor > elevator_cpy.position; floor--)
          {
            if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
            {
              maximum = floor;
              break;
            }
          }
          minimum=all_requestsUP[floor_index].request.position;
          for (int floor = 0; floor <(NUMBER_FLOOR); floor++)
          {
            if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
            {
              minimum = floor;
              break;
            }
          }
          if (minimum > all_requestsUP[floor_index].request.position)             
          {
            TravelingTime = ((maximum - elevator_cpy.position) + (maximum - all_requestsUP[floor_index].request.position)) * FloorTime;
            for (i = elevator_cpy.position; i <= maximum; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
                RegCall++;
              }
            }
            for (i = all_requestsUP[floor_index].request.position; i <= maximum; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
                RegCall++;
              }
            }
          }
          else
          {
            TravelingTime = ((maximum - elevator_cpy.position) + (all_requestsUP[floor_index].request.position - minimum) + (maximum - minimum)) * FloorTime;
            for (i = elevator_cpy.position; i <= maximum; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;           //clear Car Call
                RegCall++;
              }
            }
            for (i = minimum; i <= maximum; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
                RegCall++;
              }
            }
            for (i = minimum; i < all_requestsUP[floor_index].request.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
                RegCall++;
              }
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
        else if (elevator_cpy.direction == DOWN)
        {
          minimum=all_requestsUP[floor_index].request.position;
          for (int floor = 0; floor <(NUMBER_FLOOR); floor++)
          {
            if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
            {
              minimum = floor;
              break;
            }
          }
          TravelingTime = ((elevator_cpy.position - minimum) + (all_requestsUP[floor_index].request.position - minimum)) * FloorTime;
          for (i = minimum; i < all_requestsUP[floor_index].request.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          for (i = minimum; i <= elevator_cpy.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
        else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE))
        {
          TravelingTime = (elevator_cpy.position - all_requestsUP[floor_index].request.position) * FloorTime;
          for (i = elevator_cpy.position; i < all_requestsUP[floor_index].request.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
      }
      break;
    }
  case DOWN:
    {
      //printf("(all_requests.direction** DOWN **)\r\n");
      if ((all_requestsDOWN[floor_index].request.position > elevator_cpy.position))
      {      /*! Car position is > Request Position, Request is under Request */
        //printf("*! Car position is < Request Position, Request is under Car */");
        RegCall = 0;
        if (elevator_cpy.direction == UP)
        {
          //printf("(elevator_cpy.direction == UP)\r\n");
          maximum=all_requestsDOWN[floor_index].request.position;
          for (int floor = (NUMBER_FLOOR-1); floor > elevator_cpy.position; floor--)
          {
            if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
            {
              maximum = floor;
              break;
            }
          }
          if(maximum<all_requestsDOWN[floor_index].request.position) maximum=all_requestsDOWN[floor_index].request.position;
          TravelingTime = ((maximum - elevator_cpy.position) + (maximum - all_requestsDOWN[floor_index].request.position)) * FloorTime;
          for (i = elevator_cpy.position; i <= maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;           //clear Car Call
              RegCall++;
            }
          }
          for (i = all_requestsDOWN[floor_index].request.position; i <= maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
        else if (elevator_cpy.direction == DOWN)
        {
          //printf("(elevator_cpy.direction == DOWN)\r\n");
          maximum=all_requestsDOWN[floor_index].request.position;
          for (int floor = (NUMBER_FLOOR-1); floor > elevator_cpy.position; floor--)
          {
            if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
            {
              maximum = floor;
              break;
            }
          }
          if(maximum<all_requestsDOWN[floor_index].request.position) maximum=all_requestsDOWN[floor_index].request.position;//
          minimum=elevator_cpy.position;
          for (int floor = 0; floor <(NUMBER_FLOOR); floor++)
          {
            if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
            {
              minimum = floor;
              break;
            }
          }
          if (all_requestsDOWN[floor_index].request.position >= maximum)
          {
            TravelingTime = ((elevator_cpy.position - minimum) + (all_requestsDOWN[floor_index].request.position - minimum)) * FloorTime;
            for (i = minimum; i <= elevator_cpy.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
                RegCall++;
              }
            }
            for (i = minimum; i < all_requestsDOWN[floor_index].request.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
                RegCall++;
              }
            }
          }
          else
          {
            TravelingTime = ((elevator_cpy.position - minimum) + (maximum - minimum) + (maximum - all_requestsDOWN[floor_index].request.position))*FloorTime;
            for (i = minimum; i <= elevator_cpy.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
                RegCall++;
              }
            }
            for (i=minimum;i<=maximum;i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
                RegCall++;
              }
            }
            for (i = all_requestsDOWN[floor_index].request.position; i <= maximum; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
                RegCall++;
              }
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
        else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE))
        {
          //printf("(elevator_cpy.direction == STOP)\r\n");
          TravelingTime = (all_requestsDOWN[floor_index].request.position - elevator_cpy.position) * FloorTime;
          for (i = elevator_cpy.position; i < all_requestsDOWN[floor_index].request.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
      }
      else if ((all_requestsDOWN[floor_index].request.position < elevator_cpy.position))
      {
        /*! Car position is > Request Position, Car is under Request*/
        //printf("*! Car position is > Request Position, Car is under Request */");
        RegCall = 0;
        if (elevator_cpy.direction == UP)
        {
          //printf("(elevator_cpy.direction == UP)\r\n");
          maximum=elevator_cpy.position;
          for (int floor = (NUMBER_FLOOR-1); floor > elevator_cpy.position; floor--)
          {
            if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
            {
              maximum = floor;
              break;
            }
          }
          if(maximum<elevator_cpy.position) maximum=elevator_cpy.position;
          TravelingTime = ((maximum - elevator_cpy.position) + (maximum - all_requestsDOWN[floor_index].request.position)) * FloorTime;
          for (i = elevator_cpy.position; i <= maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          for (i = all_requestsDOWN[floor_index].request.position; i <= maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
        else if (elevator_cpy.direction == DOWN)
        {
          //printf("(elevator_cpy.direction == DOWN)\r\n");
          TravelingTime = (elevator_cpy.position - all_requestsDOWN[floor_index].request.position) * FloorTime;
          for (i = all_requestsDOWN[floor_index].request.position; i < elevator_cpy.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
        else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE))
        {
          //printf("(elevator_cpy.direction == STOP)\r\n");
          TravelingTime = (elevator_cpy.position - (all_requestsDOWN[floor_index].request.position)) * FloorTime;
          for (i = all_requestsDOWN[floor_index].request.position; i <=elevator_cpy.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[i].active != 0)))////////Car and Landing DOWN
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
      }
      break;
    }
  }   //switch (direction)
  //printf("Elevator Number= %d Maximum = %d and Minimum = %d TravelingTime = %d and TotalStopageTime = %d\r\n", ElevatorCounter,maximum,minimum,TravelingTime,TotalStopageTime);
  return (TravelingTime + TotalStopageTime);
}
uint8_t findSmallestElement(int Array[],int ArraySize)
{   
  uint8_t index=0;
  int smallest=0;
  for(uint8_t i=0;i<ArraySize;i++)
  {
    if(elevator_ctrl_chk_elevator_availability(i) == IN_SERVICE)
    {
      smallest=Array[i];
      index=i;
      break;
    }
  }
  for(uint8_t i=0;i<ArraySize;i++)
  {
    if((Array[i]<smallest)&&(elevator_ctrl_chk_elevator_availability(i) == IN_SERVICE))
    {
      smallest=Array[i];
      index=i;
    }
  }
  return index;
}
void selectionSort(int a[]) 
{
  int min, temp;
  for (int i = 0; i < TOTAL_ELEVATOR_NO - 1; i++) 
  {
    min = i;
    for (int j = i + 1; j < TOTAL_ELEVATOR_NO; j++)
      if (a[j] < a[min])
        min = j;
    temp = a[i];
    a[i] = a[min];
    a[min] = temp;
  }
}

/*
* @}
* */
