
/**
******************************************************************************
*@file    	Comm.c
*@authors	Eng.Islam Bedair <islambedair2@gmail.com>
*@date    	01/01/2020
*@brief   	This file contains the communication handler .		\n\n
*@Detail
* \b PROJECT   \n&nbsp;&nbsp; 		Duplex_Elevator_Control_System. \n
* \b H/W       \n&nbsp;&nbsp; 		Micro Controller STM8L151M8. 	\n
* \b S/W       \n&nbsp;&nbsp; 		IAR Systems IDE & Compiler.	\n
*****************************************************************************
*/
/**
* @addtogroup 	Application
* @{
*/
/************************************* Headers ****************************************/
#include "Comm.h"
#include "Elevator_Control.h"
/************************************* Global Variables ******************************/
static Comm_State_Machine_TypeDef      comm_current_state_machine;
//static Comm_State_Machine_TypeDef      comm_previous_state_machine;
UART_HandleTypeDef              *comm_active_port = &UART_ELEVATOR_1;
UART_HandleTypeDef              *uart_elevator_next = &UART_ELEVATOR_1;
uint8_t                         comm_active_elevator;
static	uint16_t                comm_waiting_ack_counter[TOTAL_ELEVATOR_NO];
static	uint16_t                comm_change_com_port[TOTAL_ELEVATOR_NO];
static	uint16_t                OutOfServiceCounter;
static  uint16_t                usr_inf_chk_new_req_counter;
static  uint8_t                 comm_flags[TOTAL_ELEVATOR_NO];
static  uint8_t                 TxBufferElevator[TOTAL_ELEVATOR_NO][TX_FRAME_SIZE];
GPIO_TypeDef* GPIOx[4]={GPIOB,GPIOB,GPIOB,GPIOA};
uint16_t GPIO_Pin[4]={GPIO_PIN_13,GPIO_PIN_14,GPIO_PIN_15,GPIO_PIN_8};
/************************************* Functions *************************************/
void ToggleElevatorStatusLed(uint8_t elevator)
{
  HAL_GPIO_TogglePin(GPIOx[elevator],GPIO_Pin[elevator]);
}
void ClearElevatorStatusLed(uint8_t elevator)
{
  HAL_GPIO_WritePin(GPIOx[elevator],GPIO_Pin[elevator],GPIO_PIN_RESET);
}
void SetElevatorStatusLed(uint8_t elevator)
{
  HAL_GPIO_WritePin(GPIOx[elevator],GPIO_Pin[elevator],GPIO_PIN_SET);
}
int getstrlen(const char * str)
{
  const char *eos = str;
  while( *eos++ ) ;
  return( eos - str - 1 );
}
void communication_task(void)
{
  static  uint8_t                 ActiveElevatorNumber;
  //HAL_UART_Transmit(&huart2,&ActiveElevatorNumber,1,1000);
  switch (comm_current_state_machine)
  {
  case COMM_IDLE:
    {
      //HAL_UART_Transmit(&huart2,&ActiveElevatorNumber,1,100);
      /*!-Check if there is any required activity*/
      if (comm_check_event_status(ActiveElevatorNumber,COMM_FRAME_RECEIVED))
      {
        //HAL_GPIO_TogglePin(GPIOx[ActiveElevatorNumber],GPIO_Pin[ActiveElevatorNumber]);
        //HAL_UART_Transmit(&huart2,&ActiveElevatorNumber,1,100);
        comm_clr_event(ActiveElevatorNumber,COMM_FRAME_RECEIVED);             /*!- Clear Received frame event */
        comm_current_state_machine = COMM_GET_DATA;     /*!- change the state machine to the next state*/
        //comm_previous_state_machine = COMM_IDLE;
        comm_change_com_port[ActiveElevatorNumber]=0;
      }
      else if (comm_change_com_port[ActiveElevatorNumber] >= COMM_CHANGE_COM_PORT_TIMEOUT)
      {
        //HAL_UART_Transmit(&huart2,&ActiveElevatorNumber,1,100);
        HAL_GPIO_WritePin(GPIOx[ActiveElevatorNumber],GPIO_Pin[ActiveElevatorNumber],GPIO_PIN_RESET);
        clearPort(ActiveElevatorNumber);    
        /*!- read previous elevator status*/
        Elevator_TypeDef elevator_copy;          /*!-Go to the current state machine routine*/
        elevator_ctrl_get_elevator_status((Elevator_TypeDef*)&elevator_copy, ActiveElevatorNumber);
        elevator_copy.status = OUT_SERVICE;
        elevator_ctrl_set_event(ELEVATOR_CTRL_ELEVETOR_OUT_OF_SERVICE,ActiveElevatorNumber);
        elevator_set_elevator_availability(OUT_SERVICE,ActiveElevatorNumber);
        elevator_ctrl_set_elevator_status((Elevator_TypeDef*)&elevator_copy, ActiveElevatorNumber);
        comm_change_com_port[ActiveElevatorNumber]=0; 
        ActiveElevatorNumber++;if(ActiveElevatorNumber==TOTAL_ELEVATOR_NO)ActiveElevatorNumber=0;
      }
      else 
      {
        ActiveElevatorNumber++;if(ActiveElevatorNumber==TOTAL_ELEVATOR_NO)ActiveElevatorNumber=0;
      }
      break;
    }
  case COMM_WAITING_ACK:
    {
      if (comm_check_event_status(ActiveElevatorNumber,COMM_RECEIVE_ACK))
      {
        //HAL_UART_Transmit(&huart2,&ActiveElevatorNumber,1,1000);
        comm_clr_event(ActiveElevatorNumber,COMM_RECEIVE_ACK);
        elevator_ctrl_set_event(ELEVATOR_CTRL_SEND_ELEVATOR_ORDER_ACK,ActiveElevatorNumber);
        comm_waiting_ack_counter[ActiveElevatorNumber] = 0;
        /*!- set all sent request with sent status*/
        if(elevator_ctrl_chk_elevator_availability(ActiveElevatorNumber)==IN_SERVICE)
        {
          HAL_GPIO_TogglePin(GPIOx[ActiveElevatorNumber],GPIO_Pin[ActiveElevatorNumber]);
          for(uint8_t floor = 0; floor < NUMBER_FLOOR; floor++)
          {
            if((get_req_assigned_elevator(floor,UP) == ActiveElevatorNumber)&&(get_req_elevator_status(floor,UP) == NOT_SENT) &&(get_req_active(floor,UP)==1))
              set_req_elevator_status(floor,UP, SENT);
            if((get_req_assigned_elevator(floor,DOWN) == ActiveElevatorNumber)&&(get_req_elevator_status(floor,DOWN) == NOT_SENT) &&(get_req_active(floor,DOWN)==1))
              set_req_elevator_status(floor,DOWN, SENT);
          }
        }
        else
          HAL_GPIO_WritePin(GPIOx[ActiveElevatorNumber],GPIO_Pin[ActiveElevatorNumber],GPIO_PIN_SET);
      }
      /*!- change the state machine to the next state*/
      comm_current_state_machine = COMM_IDLE;
      //comm_previous_state_machine = COMM_WAITING_ACK;
      ActiveElevatorNumber++;if(ActiveElevatorNumber==TOTAL_ELEVATOR_NO)ActiveElevatorNumber=0;
      break;
    }
  case COMM_SEND_DATA:
    {
      /*!- send the elevator order*/
      comm_set_elevator_data(ActiveElevatorNumber);
      comm_current_state_machine = COMM_WAITING_ACK;     /*!- change the state machine to the next state*/
      //comm_previous_state_machine = COMM_SEND_DATA;
      break;
    }
  case COMM_GET_DATA:
    {
      /*!- Get data*/
      if(comm_get_elevator_data(ActiveElevatorNumber))
      {
        //HAL_UART_Transmit(&huart2,&ActiveElevatorNumber,1,100);
        /*!- change the state machine to the next state*/
        comm_current_state_machine=COMM_SEND_DATA;
        // comm_previous_state_machine=COMM_GET_DATA;
      }
      else
      {
        /*!- change the state machine to the next state*/
        //HAL_UART_Transmit(&huart2,&ActiveElevatorNumber,1,100);
        comm_current_state_machine =COMM_IDLE;
        //comm_previous_state_machine=COMM_GET_DATA;
      }
      break;
    }
  }
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        get elevator status
* @param	None
* @retval       the result of checking
* @note
*/
uint8_t comm_get_elevator_data(uint8_t ActiveElevatorNumber1)
{
  uint8_t floor = 0;
  uint8_t  result ;
  Elevator_TypeDef elevator_copy;
  uint8_t comm_buffer[RX_FRAME_SIZE];
  for(uint8_t index=0;index < RX_FRAME_SIZE;index++)comm_buffer[index]=GetRxBufferElevator(ActiveElevatorNumber1,index);
  if((comm_buffer[RX_CHECK_SUM_EVEN]==comm_chk_sum(comm_buffer,0,RX_FRAME_SIZE-2,EVEN))&&(comm_buffer[RX_CHECK_SUM_ODD ]==comm_chk_sum(comm_buffer,0,RX_FRAME_SIZE-2,ODD)))
  {
    elevator_ctrl_get_elevator_status((Elevator_TypeDef*)&elevator_copy,ActiveElevatorNumber1);
    elevator_copy.direction=(Direction_Enm_TypeDef)(comm_buffer[RX_SPEED_DIRECTION] & COMM_DIR_MASK);  /*!- get Elevator Direction*/ 
    elevator_copy.Speed=(Speed_Enm_TypeDef)(comm_buffer[RX_SPEED_DIRECTION] & COMM_SPD_MASK);          /*!- get Elevator Speed*/
    uint8_t CurruntPosition=comm_buffer[RX_MAX_CURRENT_FLOOR] & COMM_CURRENT_FLOOR_MASK;               //!- get Elevator Location
    elevator_copy.position =CurruntPosition;
    elevator_copy.StoppageTime = comm_buffer[Stoppage_Time];                                           //!- get Elevator Stoppage_Time
    /*!- get Elevator status*/             
    elevator_copy.MaxFloor = (comm_buffer[RX_MAX_CURRENT_FLOOR] & COMM_MAX_FLOOR_NO_MASK) >> 4; //get high nibble (uint8_t)((value & 0x0F)>>4);//get floor number
    /*!- Read 16 internal requests*/   /*!         FROM  0          TO----->        MaxFloor*/
    for (floor = 0; floor <= elevator_copy.MaxFloor; floor++)
      elevator_copy.internal_request[floor].active = ((comm_buffer[RX_DUP_INT_0_7+(floor/8)] >> (floor%8)) & 0x01);      /*!- internal requests (FROM 0 TO 7)*/
    if((comm_buffer[RX_SPEED_DIRECTION] & FORK_ERR)//RSVD	RSVD	RSVD	ForkErr	SpeedStatus1	SpeedStatus0	Car Direction1	Car Direction0     
       ||(comm_buffer[RX_STATUS_1] & (COMM_SLIP | COMM_OVERLOAD | COMM_SAFE_LOCK_ERROR | COMM_LOCK))//LOCK(CAMTr) FORK	Safe_Lock_Error	DNL UPL Overload SLIP NORM
         ||(comm_buffer[RX_STATUS_2] & (COMM_SERV | COMM_FIRE | COMM_FSTT | COMM_PHFL | COMM_O_WT | COMM_CAR_STOP))//CAM Status	PHFL O.WT. FSTT SLWT FIRE  CARSTOP SERV
           ||( (comm_buffer[RX_STATUS_1] & COMM_UPL) && ((comm_buffer[RX_MAX_CURRENT_FLOOR] & COMM_CURRENT_FLOOR_MASK) != elevator_copy.MaxFloor))//
             ||( (comm_buffer[RX_STATUS_1] & COMM_DNL) && ((comm_buffer[RX_MAX_CURRENT_FLOOR] & COMM_CURRENT_FLOOR_MASK) != 0)))
    {
      //printf("OUT_SERVICE:%d\r\n",ActiveElevatorNumber1);
      elevator_copy.status=OUT_SERVICE;
      elevator_ctrl_set_event(ELEVATOR_CTRL_ELEVETOR_OUT_OF_SERVICE,ActiveElevatorNumber1);
      elevator_set_elevator_availability(OUT_SERVICE,ActiveElevatorNumber1);
    }
    else
    {
      elevator_copy.status=IN_SERVICE;
      elevator_ctrl_set_event(ELEVATOR_CTRL_ELEVETOR_IN_SERVICE,ActiveElevatorNumber1);
      elevator_set_elevator_availability(IN_SERVICE,ActiveElevatorNumber1);
      OutOfServiceCounter=0;
    }
    for (floor = 0; floor <NUMBER_FLOOR; floor++)
      {
        // Landing Call UP !- Down Requests	floor_1  to    MaxFloor
        /*!- check if this request already sent or not*/
        if ((elevator_copy.external_requestUP[floor].active!=((comm_buffer[(RX_DUP_EXT_UP_1+(floor/8))]>>(floor%8))&0x01))
            &&(get_req_assigned_elevator(floor,UP)==ActiveElevatorNumber1)
              &&(elevator_copy.status==IN_SERVICE)
                && (get_req_elevator_status(floor,UP)==SENT))
        {
          clr_req(floor,UP);
          //USER_INF_CLR_LED(index, UP);
          elevator_copy.external_requestUP[floor].active = 0;
          //HAL_Delay(2);
        }
        // Landing Call DOWN !- Down Requests	floor_1  to    MaxFloor
        /*!- check if this request already sent or not*/
        if ((elevator_copy.external_requestDOWN[floor].active != ((comm_buffer[(RX_DUP_EXT_DOWN_1+(floor/8))]>>(floor%8))&0x01))
            &&(get_req_assigned_elevator(floor,DOWN) == ActiveElevatorNumber1)
              &&(elevator_copy.status==IN_SERVICE)
                &&(get_req_elevator_status(floor,DOWN) == SENT))
        {
          clr_req(floor,DOWN);
          //USER_INF_CLR_LED(index, DOWN);
          elevator_copy.external_requestDOWN[floor].active = 0; 
          //HAL_UART_Transmit(&huart2,&floor,1,1000);
        }      
      }
    elevator_ctrl_set_elevator_status((Elevator_TypeDef*)&elevator_copy, ActiveElevatorNumber1);
    for(uint8_t comm_passive_elevator=0;comm_passive_elevator< TOTAL_ELEVATOR_NO ;comm_passive_elevator++)
    {  
      //if(comm_passive_elevator==ActiveElevatorNumber1)continue;
      if((elevator_copy.direction!=DOWN)
         &&(elevator_copy.Speed==NOT_MOVE)
           &&(elevator_Get_elevator_Request(comm_passive_elevator,CurruntPosition,UP)==1)
             &&(get_req_assigned_elevator(CurruntPosition,UP)==comm_passive_elevator)
               &&(elevator_ctrl_chk_elevator_availability(comm_passive_elevator)==IN_SERVICE)
                 &&(elevator_ctrl_chk_elevator_availability(ActiveElevatorNumber1)==IN_SERVICE)
                   &&(get_req_elevator_status(CurruntPosition,UP) == SENT))
      { elevator_Clear_elevator_Request(comm_passive_elevator,CurruntPosition,UP); clr_req(CurruntPosition,UP);HAL_Delay(5);}
      else if((elevator_copy.direction!=UP)
              &&(elevator_copy.Speed==NOT_MOVE)
                &&(elevator_Get_elevator_Request(comm_passive_elevator,CurruntPosition,DOWN)==1)
                  &&(get_req_assigned_elevator(CurruntPosition,DOWN)==comm_passive_elevator)
                    &&(elevator_ctrl_chk_elevator_availability(comm_passive_elevator)==IN_SERVICE)
                      &&(elevator_ctrl_chk_elevator_availability(ActiveElevatorNumber1)==IN_SERVICE)
                        &&(get_req_elevator_status(CurruntPosition,DOWN) == SENT))
      { elevator_Clear_elevator_Request( comm_passive_elevator,CurruntPosition,DOWN); clr_req(CurruntPosition,DOWN);}
    }
    comm_set_event(ActiveElevatorNumber1,COMM_SEND_ACK);
    result=true;
  }
  else
  {
    comm_set_event(ActiveElevatorNumber1,COMM_SEND_NACK);
    result=false;
  }
  return(result);
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        set elevator order
* @param	None
* @retval       None
* @note
*/
void comm_set_elevator_data(uint8_t ElevNo)
{ 
  for(uint8_t i=0;i<TX_FRAME_SIZE;i++)TxBufferElevator[ElevNo][i]=0;
  TxBufferElevator[ElevNo][TX_HEADER] = COMM_FRAME_HEADER;
  TxBufferElevator[ElevNo][TX_SOURCE_ID] = COMM_DUPLEX_CTRL_ID;
  TxBufferElevator[ElevNo][TX_DESTINATION_ID] = COMM_ELEVATOR_CTRL_ID;
  /*!- read previous elevator status*/
  for (uint8_t floor = 0;floor < NUMBER_FLOOR;floor++)
  {
    /*!- external Up requests*/
    if(elevator_Get_elevator_Request(ElevNo,floor,UP)==1)
      TxBufferElevator[ElevNo][TX_DUP_EXT_UP_1+(floor/8)]|=(1<<(floor%8));
    /*!- external  Down requests*/
    if(elevator_Get_elevator_Request(ElevNo,floor,DOWN)==1)
      TxBufferElevator[ElevNo][TX_DUP_EXT_DOWN_1+(floor/8)]|=(1<<(floor%8));  
  }
  TxBufferElevator[ElevNo][TX_CHECK_SUM_EVEN] = comm_chk_sum(TxBufferElevator[ElevNo], 3,TX_FRAME_SIZE - 2, EVEN);
  TxBufferElevator[ElevNo][TX_CHECK_SUM_ODD]  = comm_chk_sum(TxBufferElevator[ElevNo], 3,TX_FRAME_SIZE - 2, ODD );
  //HAL_UART_Transmit_IT(comm_get_active_elevator( ElevNo), TxBufferElevator[ElevNo], TX_FRAME_SIZE);
}

void SetTxBufferElevator(uint8_t ElevatorNumber,uint8_t index,uint8_t data)
{
  TxBufferElevator[ElevatorNumber][index]=data;
}
uint8_t GetTxBufferElevator(uint8_t ElevatorNumber,uint8_t index)
{
  return(TxBufferElevator[ElevatorNumber][index]);
}
uint8_t *GetTxBufferAddress(uint8_t ElevatorNumber)
{
  return TxBufferElevator[ElevatorNumber];
}
void RstOutOfServiceCounter(void)
{
  OutOfServiceCounter=0;
}
uint16_t GetOutOfServiceCounter(void)
{
  return OutOfServiceCounter;
}
void Rstusr_inf_chk_new_req_counter(void)
{
  usr_inf_chk_new_req_counter=0;
}
uint16_t Getusr_inf_chk_new_req_counter(void)
{
  return usr_inf_chk_new_req_counter;
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        calculate the check sum.
* @param	*data: pointer to the received data.
* @param	data_size: number of received data.
* @retval       return the calculated check sum.
* @note
*/
uint8_t comm_chk_sum(uint8_t *data,uint8_t StartIndex,uint8_t data_size,Chk_Sum_Typedef chk_sum_t)
{
  uint16_t sum=0 ;
  uint8_t chk_sum=0;
  for(uint8_t i=StartIndex;i<data_size;i++) {sum+=data[i];}
  if(chk_sum_t==EVEN) chk_sum=((uint8_t)(((sum)>>8)&0xFF));
  else chk_sum=((uint8_t)((sum)&0xFF));
  return(chk_sum);
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        Set Communication events.
* @param	None
* @retval       None
* @note
*/
void comm_set_event(uint8_t ActiveElevatorNumber2,Comm_Events_TypeDef event)
{
  switch (event)
  {
  case COMM_SEND_ELEVATOR_ORDER:
    comm_flags[ActiveElevatorNumber2] |= COMM_SEND_ELEVATOR_ORDER;
    break;
  case COMM_SEND_ACK:
    comm_flags[ActiveElevatorNumber2]  |= COMM_SEND_ACK;
    break;
  case COMM_SEND_NACK:
    comm_flags[ActiveElevatorNumber2]  |= COMM_SEND_NACK;
    break;
  case COMM_FRAME_RECEIVED:
    comm_flags[ActiveElevatorNumber2]  |= COMM_FRAME_RECEIVED;
    break;
  case COMM_RECEIVE_ACK:
    comm_flags[ActiveElevatorNumber2]  |= COMM_RECEIVE_ACK;
    break;
  }
}
void TIM3_COMM_SET_NEW_TICK(void)
{
  //COMM_NEW_TICKS://tick every 1 Sec
  for(int ActiveElevatorNumber3=0;ActiveElevatorNumber3< TOTAL_ELEVATOR_NO;ActiveElevatorNumber3++)
  {
    comm_change_com_port[ActiveElevatorNumber3]++;
    comm_waiting_ack_counter[ActiveElevatorNumber3]++;
  }
  usr_inf_chk_new_req_counter++;
  OutOfServiceCounter++;
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        Clr communication events.
* @param	None
* @retval       None
* @note
*/
void comm_clr_event(uint8_t ActiveElevatorNumber4,Comm_Events_TypeDef event)
{
  switch (event)
  {
  case COMM_SEND_ELEVATOR_ORDER:
    comm_flags[ActiveElevatorNumber4]  &= ~COMM_SEND_ELEVATOR_ORDER;
    break;
  case COMM_SEND_ACK:
    comm_flags[ActiveElevatorNumber4]  &= ~COMM_SEND_ACK;
    break;
  case COMM_SEND_NACK:
    comm_flags[ActiveElevatorNumber4]  &= ~COMM_SEND_NACK;
    break;
  case COMM_FRAME_RECEIVED:
    comm_flags[ActiveElevatorNumber4]  &= ~COMM_FRAME_RECEIVED;
    break;
  case COMM_RECEIVE_ACK:
    comm_flags[ActiveElevatorNumber4]  &= ~COMM_RECEIVE_ACK;
    break;
  }
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        check communication events.
* @param	None
* @retval       None
* @note
*/
bool comm_check_event_status(uint8_t ActiveElevatorNumber5,Comm_Events_TypeDef event)
{
  return(comm_flags[ActiveElevatorNumber5] & event);
}

/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        Determine the active port and elevator no.
* @param	active_port: is the receiving data port
* @retval       None
* @note
*/
void comm_set_active_elevator(UART_HandleTypeDef *active_port)
{
  if (active_port == &UART_ELEVATOR_1)
  {
    comm_active_port = &UART_ELEVATOR_1;
    comm_active_elevator = 0;
  }
  else  if (active_port == &UART_ELEVATOR_2)
  {
    comm_active_port = &UART_ELEVATOR_2;
    comm_active_elevator = 1;
  }
  else  if (active_port == &UART_ELEVATOR_3)
  {
    comm_active_port = &UART_ELEVATOR_3;
    comm_active_elevator = 2;
  }
  else  if (active_port == &UART_ELEVATOR_4)
  {
    comm_active_port = &UART_ELEVATOR_4;
    comm_active_elevator = 3;
  }
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        read the active port and elevator no.
* @param	active_port: is the receiving data port
* @retval       None
* @note
*/
UART_HandleTypeDef *comm_get_active_elevator(uint8_t ActiveElevatorNumber7)
{
  UART_HandleTypeDef *comm_active_port;
  if (ActiveElevatorNumber7 == 0)
    comm_active_port = &huart6;
  else  if (ActiveElevatorNumber7 == 1)
    comm_active_port = &huart1;
  else  if (ActiveElevatorNumber7 == 2)
    comm_active_port = &huart4;
  else if (ActiveElevatorNumber7 == 3)
    comm_active_port = &huart5;
  return(comm_active_port);
}
/** @auther	Islam Bedair <islambedair2@gmail.com>
* @brief        read the next_active port and elevator no.
* @param	uart_elevator_next: is the receiving data port
* @retval       None
* @note
*/
UART_HandleTypeDef  *comm_get_next_active_elevator(void)
{
  return(uart_elevator_next);
}
uint8_t GetActiveElevatorNumber(UART_HandleTypeDef *huart)
{
  uint8_t ActiveElevatorNumber8;
  if(huart->Instance==USART6) ActiveElevatorNumber8=0;
  else if(huart->Instance==USART1) ActiveElevatorNumber8=1;
  else if(huart->Instance==USART4) ActiveElevatorNumber8=2;
  else if(huart->Instance==USART5) ActiveElevatorNumber8=3;
  return ActiveElevatorNumber8;
}
/** @auther	Eng.Islam Bedair <islambedair2@gmail.com>
* @brief
* @param        UART_Elevator: the elevator uart port
* @retval       Data that received
* @note
*/
/************************************* Data Type ***************************************/

/*
* @}
* */
