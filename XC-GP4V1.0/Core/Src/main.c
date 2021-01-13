/* USER CODE BEGIN Header */
/**
******************************************************************************
* @file           : main.c
* @brief          : Main program body
******************************************************************************
* @attention
* <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
* All rights reserved.</center></h2>
*
* This software component is licensed by ST under BSD 3-Clause license,
* the "License"; You may not use this file except in compliance with the
* License. You may obtain a copy of the License at:
*                        opensource.org/licenses/BSD-3-Clause
******************************************************************************
*/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Comm.h"
#include "Config_File.h"
#include "ShiftIn165.h"
#include "ShiftOut595.h"
#include "User_Interface.h"
#include "Elevator_Control.h"
#include "delay.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//uint8_t rxBuffer;
/* USER CODE BEGIN 4 */

/* USER CODE END 4 */
uint8_t RXdata[TOTAL_ELEVATOR_NO];
uint8_t RxBufferElevator[TOTAL_ELEVATOR_NO][RX_FRAME_SIZE];
volatile uint8_t frheader[TOTAL_ELEVATOR_NO]={0},frcount[TOTAL_ELEVATOR_NO]={0};
void SetRxBufferElevator(uint8_t ElevatorNumber,uint8_t index,uint8_t data)
{
  RxBufferElevator[ElevatorNumber][index]=data;
}
uint8_t GetRxBufferElevator(uint8_t ElevatorNumber,uint8_t index)
{
  return(RxBufferElevator[ElevatorNumber][index]);
}
/**
  * @brief This function handles USART2 global interrupt.
*/

void UART_Handle_RX_ISR(UART_HandleTypeDef *huart)
{
  /* Read the received data */
  uint8_t ElevNo;
  if(huart->Instance==USART6)      ElevNo=0;
  else if(huart->Instance==USART1) ElevNo=1;
  else if(huart->Instance==USART4) ElevNo=2;
  else if(huart->Instance==USART5) ElevNo=3;
  //receiving my message
  if (frheader[ElevNo] == 0x03)
  {
    RxBufferElevator[ElevNo][frcount[ElevNo]]=RXdata[ElevNo];
    if (frcount[ElevNo] == RX_FRAME_SIZE)
    {
      comm_set_event(ElevNo,COMM_FRAME_RECEIVED);
      frheader[ElevNo] = 0; frcount[ElevNo] = 0;
      //HAL_UART_Transmit_IT(huart,GetTxBufferAddress(ElevNo),TX_FRAME_SIZE);
    }
    else frcount[ElevNo]++;
  }
  if ((frheader[ElevNo] == 0x02)&&(RXdata[ElevNo] ==COMM_DUPLEX_CTRL_ID)){frheader[ElevNo] = 0x03;}else if (frheader[ElevNo] == 0x02){frheader[ElevNo] = 0;}
  if ((frheader[ElevNo] == 0x01)&&(RXdata[ElevNo]==COMM_ELEVATOR_CTRL_ID)){frheader[ElevNo] = 0x02;}else if (frheader[ElevNo] == 0x01){frheader[ElevNo] = 0;}
  if ((frheader[ElevNo] == 0x00)&&(RXdata[ElevNo] == COMM_FRAME_HEADER)){frheader[ElevNo] = 0x01;}
  else if ((frheader[ElevNo] == 0x00)&&(frcount[ElevNo] == 0x00)&&(RXdata[ElevNo] == COMM_ACK)){comm_set_event(ElevNo,COMM_RECEIVE_ACK);/*if(huart->Instance==USART6)HAL_UART_Transmit(&huart2,&ElevNo,1,1000);*/}
  HAL_UART_Receive_IT (huart, &RXdata[ElevNo],1);
}

void clearPort(uint8_t ElevNo)
{
  frheader[ElevNo] = 0; frcount[ElevNo] = 0;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  //__NOP();//test if we reach this position
  UART_Handle_RX_ISR(huart);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) 
{
  TIM3_COMM_SET_NEW_TICK();
}
void UART_Handle_TX_ISR(UART_HandleTypeDef *huart)
{
  //__NOP();//test if we reach this position
  HAL_UART_Transmit_IT(&huart6,GetTxBufferAddress(0),TX_FRAME_SIZE);
  HAL_UART_Transmit_IT(&huart1,GetTxBufferAddress(1),TX_FRAME_SIZE);
  HAL_UART_Transmit_IT(&huart4,GetTxBufferAddress(2),TX_FRAME_SIZE);
  HAL_UART_Transmit_IT(&huart5,GetTxBufferAddress(3),TX_FRAME_SIZE);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  UART_Handle_TX_ISR(huart);
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
struct __FILE{
  int handle;
  /* Whatever you require here. If the only file you are using is */
  /* standard output using printf() for debugging, no file handling */
  /* is required. */
};
FILE __stdout;
int fputc(int TXdata, FILE *f){
    HAL_UART_Transmit(&huart2, (uint8_t *)&TXdata,1,0xffff);
  return TXdata;
}
int ferror(FILE *f)
{
  /* Your implementation of ferror(). */
  return 0;
}
/* PRINTF REDIRECT to UART END */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART4_UART_Init();
  MX_USART5_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  ClearAllRequests();
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_UART_Receive_IT(&huart6,&RXdata[0],1);// start listening for incomming data
  HAL_UART_Receive_IT(&huart1,&RXdata[1],1);// start listening for incomming data
  HAL_UART_Receive_IT(&huart4,&RXdata[2],1);// start listening for incomming data
  HAL_UART_Receive_IT(&huart5,&RXdata[3],1);// start listening for incomming data
    //printf("XC-GP4 V210102\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    //printf("user_interface_task\r\n");
    user_interface_task();      //!-Handling user interface tasks and determine the best elevator for handling requests 
    //printf("elevator_control_task\r\n");
    elevator_control_task();    //!- read elevator status , send new tasks and follow the old tasks
    //printf("communication_task\r\n");
    communication_task();       //!- handling the communication commend through RS232
    //delay_ms(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
  tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
