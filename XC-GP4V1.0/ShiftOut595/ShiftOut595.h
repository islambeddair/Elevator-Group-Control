/*
 * ShiftOut595.h
 *
 * Created: 9/23/2020 3:54:41 PM
 *  Author: islam.bedair
 */ 


#ifndef SHIFTOUT595_H_
#define SHIFTOUT595_H_
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Config_File.h"
#include "gpio.h"
#include "ShiftIn165.h"
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */
//////////////////////////////////////////////////////////////////////////////////////
#define myDataPinUp     GPIO_PIN_6      //Pin connected to DataInUp of 74HC595  UP
#define myClockPinUp    GPIO_PIN_0      //Pin connected to SRCLKUp  of 74HC595  UP
#define LatchPinUp      GPIO_PIN_7      //Pin connected to RCLKUp   of 74HC595  UP
//////////////////////////////////////////////////////////////////////////////////////
#define myDataPortUp    GPIOA
#define myClockPortUp   GPIOB
#define LatchPortUp     GPIOA
///////////////////////////////////////////////////////////////////////////////////////
#define myDataPinDN     GPIO_PIN_12     //Pin connected to DataInDN of 74HC595 DOWN
#define myClockPinDN    GPIO_PIN_6      //Pin connected to SRCLKDN  of  74HC595 DOWN
#define LatchPinDN      GPIO_PIN_5      //Pin connected to RCLKDN  of  74HC595 DOWN
///////////////////////////////////////////////////////////////////////////////////////
#define myDataPortDN    GPIOA        
#define myClockPortDN   GPIOB     
#define LatchPortDN     GPIOB
void ShiftOutLandingUp(uint8_t LandingCallUp[],uint8_t MaxFloorNum);
void ShiftOutLandingDN(uint8_t LandingCallDN[],uint8_t MaxFloorNum);

/** @auther	Eng.Islam Bedair <islambedair2@gmail.com>
* @brief  switch the led on.
* @param	floor_no: is the floor number.
* @param	switch_type: is the switch type
* 		@arg UP_SW_LED: Up led
* 		@arg DOWN_SW_LED: Down led
* @retval none
* @note
*/
void USER_INF_SET_LED(uint8_t floor_no,Direction_Enm_TypeDef direction);
/** @auther	Eng.Islam Bedair <islambedair2@gmail.com>
* @brief  switch the led off.
* @param	floor_no: is the floor number.
* @param	switch_type: is the switch type
* 		@arg UP_SW_LED: Up led
* 		@arg DOWN_SW_LED: Down led
* @retval none
* @note
*/
void USER_INF_CLR_LED(uint8_t floor_no,Direction_Enm_TypeDef direction);
void USER_INF_SET_ALL(void);
void USER_INF_CLR_ALL(void);
#ifdef __cplusplus
}
#endif
#endif /* SHIFTOUT595_H_ */