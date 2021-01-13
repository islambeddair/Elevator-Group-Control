/*
 * ShiftOut595.c
 *
 * Created: 9/23/2020 3:55:23 PM
 *  Author: islam.bedair
 */ 
#include "ShiftOut595.h"
void ShiftOutLandingUp(uint8_t LandingCallUp[],uint8_t MaxFloorNum)
{
  //ground latchPin and hold low for as long as you are transmitting
  HAL_GPIO_WritePin(LatchPortUp,LatchPinUp,GPIO_PIN_RESET);
    //internal function setup
   for(int bitCount = MaxFloorNum-1; bitCount>=0; bitCount--)//for (uint8_t bitCount = 0; bitCount<MaxFloorNum; bitCount++)
    {
      HAL_GPIO_WritePin(myDataPortUp,myDataPinUp,(GPIO_PinState)((LandingCallUp[(bitCount/8)]>>((bitCount%8)))&0x01));// This shifts 8 bits out LSB first,MSB 0b01100011 LSB
      HAL_GPIO_WritePin(myClockPortUp,myClockPinUp,GPIO_PIN_SET);//on the rising edge of the clock,
      HAL_GPIO_WritePin(myClockPortUp,myClockPinUp,GPIO_PIN_RESET); //clock idles low
    }
  //return the latch pin high to signal chip that it and no longer needs to listen for information
  HAL_GPIO_WritePin(LatchPortUp,LatchPinUp,GPIO_PIN_SET);
}
void ShiftOutLandingDN(uint8_t LandingCallDN[],uint8_t MaxFloorNum)
{
  //ground latchPin and hold low for as long as you are transmitting
  HAL_GPIO_WritePin(LatchPortDN,LatchPinDN,GPIO_PIN_RESET);
  //for (uint8_t i = 0;  i<byteCount; i++)
  {
    //  MSB 0b01100011 LSB
    // This shifts 8 bits out LSB first, 
    //on the rising edge of the clock,
    //clock idles low
    //internal function setup
    for(int bitCount = MaxFloorNum-1; bitCount>=0; bitCount--)//for(uint8_t bitCount = 0; bitCount<MaxFloorNum; bitCount++)//
    {
      HAL_GPIO_WritePin(myDataPortDN,myDataPinDN,(GPIO_PinState)((LandingCallDN[(bitCount/8)]>>(7-(bitCount%8)))&0x01));
      HAL_GPIO_WritePin(myClockPortDN,myClockPinDN,GPIO_PIN_SET);
      HAL_GPIO_WritePin(myClockPortDN,myClockPinDN,GPIO_PIN_RESET);
    }
  }
  //return the latch pin high to signal chip that it and no longer needs to listen for information
  HAL_GPIO_WritePin(LatchPortDN,LatchPinDN,GPIO_PIN_SET);
}
/** @auther	Eng.Islam Bedair <islambedair2@gmail.com>
* @brief  switch the led on.
* @param	floor_no: is the floor number.
* @param	direction: is the switch type
* 		@arg UP_SW_LED: Up led
* 		@arg DOWN_SW_LED: Down led
* @retval none
* @note
*/
void USER_INF_SET_LED(uint8_t floor_no,Direction_Enm_TypeDef direction )
{
  uint8_t LandingCall[4];
  for(int i=0;i<4;i++){LandingCall[i]=0xFF;}
  if(direction==UP)
  {
    read_shift_regsUp(LandingCall,MaxPossibleFloor);
    LandingCall[floor_no/8]&=~(1<<(floor_no%8));
    ShiftOutLandingUp(LandingCall,MaxPossibleFloor);
  }
  else if(direction==DOWN)
  {
    read_shift_regsDN(LandingCall, MaxPossibleFloor);
    LandingCall[floor_no/8]&=~(1<<(floor_no%8));
    ShiftOutLandingDN(LandingCall,MaxPossibleFloor);
  }
}
/** @auther	Eng.Islam Bedair <islambedair2@gmail.com>
* @brief  switch the led off.
* @param	floor_no: is the floor number.
* @param	direction: is the switch type
* 		@arg UP_SW_LED: Up led
* 		@arg DOWN_SW_LED: Down led
* @retval none
* @note
*/
void USER_INF_CLR_LED(uint8_t floor_no,Direction_Enm_TypeDef direction)
{
  uint8_t LandingCall[4];
  for(int i=0;i<4;i++){LandingCall[i]=0xFF;}
  if(direction==UP)
  {
    read_shift_regsUp(LandingCall,MaxPossibleFloor);
    LandingCall[(floor_no/8)]|=(1<<(floor_no%8));
    ShiftOutLandingUp(LandingCall,MaxPossibleFloor);
  }
  else if(direction==DOWN)
  {
    read_shift_regsDN(LandingCall, MaxPossibleFloor);
    LandingCall[(floor_no/8)]|=(1<<(floor_no%8));
    ShiftOutLandingDN(LandingCall,MaxPossibleFloor);
  }
}
void USER_INF_SET_ALL(void)
{
   //ground latchPin and hold low for as long as you are transmitting
  HAL_GPIO_WritePin(LatchPortUp,LatchPinUp,GPIO_PIN_RESET);
    //internal function setup
   for(int bitCount = (MaxPossibleFloor-1); bitCount>=0; bitCount--)//for (uint8_t bitCount = 0; bitCount<MaxFloorNum; bitCount++)
    {
      HAL_GPIO_WritePin(myDataPortUp,myDataPinUp,GPIO_PIN_RESET);// This shifts 8 bits out LSB first,MSB 0b01100011 LSB
      HAL_GPIO_WritePin(myClockPortUp,myClockPinUp,GPIO_PIN_SET);//on the rising edge of the clock,
      HAL_GPIO_WritePin(myClockPortUp,myClockPinUp,GPIO_PIN_RESET); //clock idles low
    }
  //return the latch pin high to signal chip that it and no longer needs to listen for information
  HAL_GPIO_WritePin(LatchPortUp,LatchPinUp,GPIO_PIN_SET);
  //ground latchPin and hold low for as long as you are transmitting
  HAL_GPIO_WritePin(LatchPortDN,LatchPinDN,GPIO_PIN_RESET);
  //for (uint8_t i = 0;  i<byteCount; i++)
  {
    //  MSB 0b01100011 LSB
    // This shifts 8 bits out LSB first, 
    //on the rising edge of the clock,
    //clock idles low
    //internal function setup
    for(int bitCount = (MaxPossibleFloor-1); bitCount>=0; bitCount--)//for(uint8_t bitCount = 0; bitCount<MaxFloorNum; bitCount++)//
    {
      HAL_GPIO_WritePin(myDataPortDN,myDataPinDN,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(myClockPortDN,myClockPinDN,GPIO_PIN_SET);
      HAL_GPIO_WritePin(myClockPortDN,myClockPinDN,GPIO_PIN_RESET);
    }
  }
  //return the latch pin high to signal chip that it and no longer needs to listen for information
  HAL_GPIO_WritePin(LatchPortDN,LatchPinDN,GPIO_PIN_SET); 
}
void USER_INF_CLR_ALL(void)
{
  //ground latchPin and hold low for as long as you are transmitting
  HAL_GPIO_WritePin(LatchPortUp,LatchPinUp,GPIO_PIN_RESET);
    //internal function setup
   for(int bitCount = (MaxPossibleFloor-1); bitCount>=0; bitCount--)//for (uint8_t bitCount = 0; bitCount<MaxFloorNum; bitCount++)
    {
      HAL_GPIO_WritePin(myDataPortUp,myDataPinUp,GPIO_PIN_SET);// This shifts 8 bits out LSB first,MSB 0b01100011 LSB
      HAL_GPIO_WritePin(myClockPortUp,myClockPinUp,GPIO_PIN_SET);//on the rising edge of the clock,
      HAL_GPIO_WritePin(myClockPortUp,myClockPinUp,GPIO_PIN_RESET); //clock idles low
    }
  //return the latch pin high to signal chip that it and no longer needs to listen for information
  HAL_GPIO_WritePin(LatchPortUp,LatchPinUp,GPIO_PIN_SET);
  //ground latchPin and hold low for as long as you are transmitting
  HAL_GPIO_WritePin(LatchPortDN,LatchPinDN,GPIO_PIN_RESET);
  //for (uint8_t i = 0;  i<byteCount; i++)
  {
    //  MSB 0b01100011 LSB
    // This shifts 8 bits out LSB first, 
    //on the rising edge of the clock,
    //clock idles low
    //internal function setup
    for(int bitCount = (MaxPossibleFloor-1); bitCount>=0; bitCount--)//for(uint8_t bitCount = 0; bitCount<MaxFloorNum; bitCount++)//
    {
      HAL_GPIO_WritePin(myDataPortDN,myDataPinDN,GPIO_PIN_SET);
      HAL_GPIO_WritePin(myClockPortDN,myClockPinDN,GPIO_PIN_SET);
      HAL_GPIO_WritePin(myClockPortDN,myClockPinDN,GPIO_PIN_RESET);
    }
  }
  //return the latch pin high to signal chip that it and no longer needs to listen for information
  HAL_GPIO_WritePin(LatchPortDN,LatchPinDN,GPIO_PIN_SET);    
}