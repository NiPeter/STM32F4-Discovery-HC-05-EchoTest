/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     

#include "gpio.h"
#include "usart.h"

#include <Communicator/Serial/HC05/HC05.hpp>

/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId rxTaskHandle;
osThreadId txTaskHandle;
osThreadId ledTaskHandle;
osSemaphoreId txSemHandle;
osSemaphoreId rxSemHandle;

/* USER CODE BEGIN Variables */

HC05			Bluetooth(&huart2);

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void StartRxTask(void const * argument);
void StartTxTask(void const * argument);
void StartLedTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of txSem */
  osSemaphoreDef(txSem);
  txSemHandle = osSemaphoreCreate(osSemaphore(txSem), 15);

  /* definition and creation of rxSem */
  osSemaphoreDef(rxSem);
  rxSemHandle = osSemaphoreCreate(osSemaphore(rxSem), 15);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of rxTask */
  osThreadDef(rxTask, StartRxTask, osPriorityRealtime, 0, 128);
  rxTaskHandle = osThreadCreate(osThread(rxTask), NULL);

  /* definition and creation of txTask */
  osThreadDef(txTask, StartTxTask, osPriorityRealtime, 0, 128);
  txTaskHandle = osThreadCreate(osThread(txTask), NULL);

  /* definition and creation of ledTask */
  osThreadDef(ledTask, StartLedTask, osPriorityNormal, 0, 128);
  ledTaskHandle = osThreadCreate(osThread(ledTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */

	Bluetooth.begin();

  /* Infinite loop */
  for(;;)
  {

	  if(Bluetooth.isAvailable() == true){
		  char c = Bluetooth.readChar();
		  Bluetooth.writeChar(c);
	  }

  }
  /* USER CODE END StartDefaultTask */
}

/* StartRxTask function */
void StartRxTask(void const * argument)
{
  /* USER CODE BEGIN StartRxTask */
	osSemaphoreWait(rxSemHandle,osWaitForever);
	/* Infinite loop */
	for(;;)
	{
		osSemaphoreWait(rxSemHandle,osWaitForever);
		Bluetooth.processRxISR();
	}
  /* USER CODE END StartRxTask */
}

/* StartTxTask function */
void StartTxTask(void const * argument)
{
  /* USER CODE BEGIN StartTxTask */
	osSemaphoreWait(txSemHandle,osWaitForever);
	/* Infinite loop */
	for(;;)
	{
		osSemaphoreWait(txSemHandle,osWaitForever);
		Bluetooth.processTxISR();
	}
  /* USER CODE END StartTxTask */
}

/* StartLedTask function */
void StartLedTask(void const * argument)
{
  /* USER CODE BEGIN StartLedTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartLedTask */
}

/* USER CODE BEGIN Application */

/**
 *
 * @param huart
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){

	if(huart->Instance == Bluetooth.getUARTInstance())
		osSemaphoreRelease(txSemHandle);

}
/********************************************************/



/*
 *
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if(huart->Instance == Bluetooth.getUARTInstance())
		osSemaphoreRelease(rxSemHandle);

}
/********************************************************/

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
