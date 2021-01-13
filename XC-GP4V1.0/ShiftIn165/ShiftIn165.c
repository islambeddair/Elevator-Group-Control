/*
 * ShiftIn165.c
 *
 * Created: 9/23/2020 3:55:23 PM
 *  Author: islam.bedair
 */ 
#include "ShiftIn165.h"
/*
Basics of SN74HC165N
SN74HC165N is a shift register which works on PArallel In and Serial OUT Protocol.
It has 8 input pins where you can connect different sensors etc. and then it has 1 Serial Output Pin, which should be connected to the Microcontroller.
With the help of Clock Pin, we can receive all these parallel 8 inputs serially from a single output into the microcontroller.
We can also connect multiple 74HC165 in parallel to increase the input pins.
Let's say if I have connected 3 shift registers in parallel then the input pins will increase by 8 x 3 = 24.
So, we can control 24 digital sensors by a single 3 Pin.
Let's have a look at  the 74HC595 Pinout:
74HC165 Pinout
I have explained the 74HC165 Pinout in detail below:
The SNx4HC165 has a wide operating voltage range of 2 V to 6 V
Pin A, B, C, D, E, F, G and H are input pins for sensors. These are all digital Pins.
Pin CLK INH is the clock enable.
Pin CLK is the clock.
Pin SER is the serial Pin from where we will connect our Microcontroller.
Pin # 8 should be connected to ground.
Pin # 16 is Vcc which should be from +2V +6V.
*/

void read_shift_regsUp(uint8_t LandingCallUp[],uint8_t MaxFloorNum)
{
  /* Trigger a parallel Load to latch the state of the data lines*/
  HAL_GPIO_WritePin(SH_LDPort165Up,SH_LDPin165Up, GPIO_PIN_RESET); 
  HAL_GPIO_WritePin(SH_LDPort165Up,SH_LDPin165Up, GPIO_PIN_SET);   
  /* Loop to read each bit value from the serial out line of the SN74HC165N.*/
  for(uint8_t bitCount = 0; bitCount<MaxFloorNum; bitCount++)    
  {
    //LandingCallUp[i] = LandingCallUp[i] << 1;
    if(HAL_GPIO_ReadPin(DataPort165Up,DataPin165Up)) 
    {
      //LandingCallUp[i] +=1;
      LandingCallUp[bitCount/8]=LandingCallUp[bitCount/8]|(1<<(7-(bitCount%8)));
      //HAL_UART_Transmit(&huart5," High ",6, 10);
    }
    else 
    {
      LandingCallUp[bitCount/8]=LandingCallUp[bitCount/8]&(~(1<<(7-(bitCount%8))));
      //HAL_UART_Transmit(&huart5," Low  ",6, 10);
    }
    /* Pulse the Clock (rising edge shifts the next bit).*/
    HAL_GPIO_WritePin(ClockPort165Up,ClockPin165Up, GPIO_PIN_SET);// (clockPin, HIGH);
    HAL_GPIO_WritePin(ClockPort165Up,ClockPin165Up, GPIO_PIN_RESET);//(clockPin, LOW);
  }
  //HAL_UART_Transmit(&huart3,LandingCallUp,(MaxFloorNum/8), 10);
  //HAL_UART_Transmit(&huart5,"\n",1, 10);
}
void read_shift_regsDN(uint8_t LandingCallDN[],uint8_t MaxFloorNum)
{
    /* Trigger a parallel Load to latch the state of the data lines*/
    HAL_GPIO_WritePin(SH_LDPort165DN,SH_LDPin165DN, GPIO_PIN_RESET);  
    HAL_GPIO_WritePin(SH_LDPort165DN,SH_LDPin165DN, GPIO_PIN_SET);
    /* Loop to read each bit value from the serial out line of the SN74HC165N.*/
    for(uint8_t bitCount= 0; bitCount<MaxFloorNum; bitCount++)
    {
      //LandingCallDN[i] = LandingCallDN[i] << 1;
       if(HAL_GPIO_ReadPin(DataPort165DN,DataPin165DN)) 
      {
        //LandingCallDN[i] +=1;
        LandingCallDN[bitCount/8]=LandingCallDN[bitCount/8]|(1U<<(bitCount%8));
       // HAL_UART_Transmit(&huart5," High ",6, 10);
      }
      else 
      {
        LandingCallDN[bitCount/8]=LandingCallDN[bitCount/8]&(~(1U<<(bitCount%8)));
        //HAL_UART_Transmit(&huart5," Low  ",6, 10);
      }
      /* Pulse the Clock (rising edge shifts the next bit).*/
      HAL_GPIO_WritePin(ClockPort165DN,ClockPin165DN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(ClockPort165DN,ClockPin165DN, GPIO_PIN_RESET);
  }
  //HAL_UART_Transmit(&huart3,LandingCallDN,(MaxFloorNum/8), 10);
  //HAL_UART_Transmit(&huart5,"\n",1, 10);
}
/** @auther	Eng.Islam Bedair <islambedair2@gmail.com>
* @brief        read the switch status.
* @param	floor_no: is the floor number.
* @param	switch_type: is the switch type
* 		@arg UP_SW_LED: Up Switch
* 		@arg DOWN_SW_LED: Down Switch
* @retval uint8_t : the pin status.
* @note
*/
uint8_t USER_INF_GET_SW_STATUS(uint8_t floor_no,Floor_Switch_Led_TypeDef switch_type)
{
  uint8_t SwitchStatus=1;
  uint8_t ReadLandingCall[4];
    if(switch_type==UP_SW_LED)
      read_shift_regsUp( ReadLandingCall,MaxPossibleFloor);
    else 
      read_shift_regsDN(ReadLandingCall, MaxPossibleFloor);
    if((ReadLandingCall[floor_no/8]>>(floor_no%8))&0x01)
      SwitchStatus=0;
    else
      SwitchStatus=1;
  return SwitchStatus;
}