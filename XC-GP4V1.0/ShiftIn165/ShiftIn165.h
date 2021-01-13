/*
 * ShiftIn165.h
 *
 * Created: 9/23/2020 3:54:41 PM
 *  Author: islam.bedair
 */ 


#ifndef SHIFTIN165_H_
#define SHIFTIN165_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Config_File.h"
#include "gpio.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN 0 */
// the heart of the program
/*
First chip

      GPIO ------> SH/LD 1 o 16 Vcc ------ 3V3
      SPI clock -> CLK   2   15 CLK INH -- Ground
                   E     3   14 D
                   F     4   13 C
                   G     5   12 B
                   H     6   11 A
   Don't connect   /Qh   7   10 SER ------ Ground
   Ground -------- GND   8    9 Qh ------> next SER

   prior SH/LD --> SH/LD 1 o 16 Vcc ------ 3V3
   prior CLK ----> CLK   2   15 CLK INH -- Ground
                   E     3   14 D
                   F     4   13 C
                   G     5   12 B
                   H     6   11 A
   Don't connect   /Qh   7   10 SER <----- prior Qh
   Ground -------- GND   8    9 Qh ------> DataPin165 
*/
//////////////////////////////////////////////////////////////////////////////////////
#define DataPin165Up       GPIO_PIN_12      //Pin connected to DataInUp of 74HC165  UP
#define ClockPin165Up      GPIO_PIN_1      //Pin connected to SRCLKUp  of 74HC165  UP
#define SH_LDPin165Up      GPIO_PIN_2      //Pin connected to RCLKUp   of 74HC165  UP
//////////////////////////////////////////////////////////////////////////////////////
#define ClockPort165Up     GPIOB     
#define DataPort165Up      GPIOB
#define SH_LDPort165Up     GPIOB
//////////////////////////////////////////////////////////////////////////////////////
#define DataPin165DN       GPIO_PIN_7     //Pin connected to DataInDN of 74HC165 DOWN
#define ClockPin165DN      GPIO_PIN_9      //Pin connected to SRCLKDN  of 74HC165 DOWN
#define SH_LDPin165DN      GPIO_PIN_8      //Pin connected to RCLKDN  of  74HC165 DOWN
//////////////////////////////////////////////////////////////////////////////////////
#define DataPort165DN      GPIOB        
#define ClockPort165DN     GPIOB     
#define SH_LDPort165DN     GPIOB
void read_shift_regsUp(uint8_t LandingCallUp[],uint8_t MaxFloorNum);
void read_shift_regsDN(uint8_t LandingCallDN[],uint8_t MaxFloorNum);
/** @auther	Eng.Islam Bedair <islambedair2@gmail.com>
* @brief        read the switch status.
* @param	floor_no: is the floor number.
* @param	switch_type: is the switch type
* 		@arg UP_SW_LED: Up Switch
* 		@arg DOWN_SW_LED: Down Switch
* @retval uint8_t : the pin status.
* @note
*/
uint8_t USER_INF_GET_SW_STATUS(uint8_t floor_no,Floor_Switch_Led_TypeDef switch_type);

#ifdef __cplusplus
}
#endif
#endif  /* SHIFTOUT595_H_ */