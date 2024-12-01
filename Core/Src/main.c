/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "rng.h"
#include "usart.h"
#include "gpio.h"


CircularBuffer tempBuffer;
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);

/**
 * @brief Initializes the circular buffer.
 *
 * This function sets the head, tail, and count of the circular buffer to zero,
 * effectively clearing any existing data in the buffer.
 *
 * @param buffer Pointer to the CircularBuffer structure to be initialized.
 */
void buffer_init(CircularBuffer *buffer) {
  buffer->head = 0;
  buffer->tail = 0;
  buffer->count = 0;
}

/**
 * @brief Adds a value to the circular buffer.
 *
 * This function adds a new temperature reading to the circular buffer.
 * If the buffer is full, it overwrites the oldest value.
 *
 * @param buffer Pointer to the CircularBuffer structure.
 * @param value The temperature value to be added to the buffer.
 */
void buffer_add(CircularBuffer *buffer, float value) {
  buffer->data[buffer->head] = value; // Add value to buffer
  buffer->head = (buffer->head + 1) % BUFFER_SIZE; // Move head to next position
  if (buffer->count < BUFFER_SIZE) {
    buffer->count++; // Increment count if buffer is not full
  }
}

/**
 * @brief Prints the contents of the circular buffer.
 *
 * This function iterates through the circular buffer and prints each stored
 * temperature value to the console. The buffer is printed in the order of
 * oldest to newest entry.
 *
 * @param buffer Pointer to the CircularBuffer structure.
 */
void buffer_print(CircularBuffer *buffer) {
  printf("Buffer: ");
  for (int i = 0; i < buffer->count; i++) {
    int index = (buffer->head - buffer->count + i + BUFFER_SIZE) % BUFFER_SIZE;
    printf("%.2f ", buffer->data[index]);
  }
  printf("\r\n");
}

/**
 * @brief Simulates temperature readings.
 *
 * This function generates a random temperature value between 20.0 and 30.0 degrees Celsius.
 * It uses the hardware random number generator (RNG) to produce the random value.
 *
 * @return float Simulated temperature value.
 */
float get_simulated_temperature() {
  uint32_t random_value;
  HAL_RNG_GenerateRandomNumber(&hrng, &random_value);
  // Simulate temperature between 20.0 and 30.0 degrees
  return 20.0f + (random_value % 1000) / 100.0f;
}

/**
 * @brief Transmits data over UART.
 *
 * This function sends a specified number of bytes from the provided data buffer
 * over the UART interface. It uses the HAL_UART_Transmit function to perform
 * the transmission and waits until the transmission is complete.
 *
 * @param file Unused parameter, included for compatibility with standard I/O functions.
 * @param data Pointer to the data buffer to be transmitted.
 * @param len Number of bytes to transmit from the data buffer.
 * @return int The number of bytes transmitted.
 */
int _write(int file, char *data, int len) {
  HAL_UART_Transmit(&huart2, (uint8_t *)data, len, HAL_MAX_DELAY);
  return len;
}

/**
  * @brief  Function implementing the LogTemperature thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_vLogTemperatureTask */
void vLogTemperatureTask(void *argument)
{
  for(;;)
  {
	  buffer_add(&tempBuffer, get_simulated_temperature());
	  osDelay(1000);
  }
}

/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void vDisplayTemperatureTask(void *argument)
{
  for(;;)
  {
	  // Print the contents of the circular buffer
	  buffer_print(&tempBuffer);
	  osDelay(5000);
  }
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_RNG_Init();
  MX_USART3_UART_Init();

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  while (1) {}

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM5 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM5) {
    HAL_IncTick();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {}
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

}
#endif /* USE_FULL_ASSERT */
