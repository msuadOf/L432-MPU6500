/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "spi.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include "usbd_cdc_if.h"
#include "MPU6500.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

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

void mpu6500_delay_ms(int ms)
{
  HAL_Delay(ms);
}
void hal_spi_init()
{
}
void hal_spi_read(unsigned char addr, unsigned char *buffer, int len)
{
  uint8_t a = addr + (1 << 7);

  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, 0);
  HAL_SPI_Transmit(&hspi1, &a, 1, HAL_MAX_DELAY);
  HAL_SPI_Receive(&hspi1, buffer, len, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, 1);
}
void hal_spi_write(unsigned char addr, unsigned char *buffer, int len)
{
  uint8_t a = addr + (0 << 7);

  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, 0);
  HAL_SPI_Transmit(&hspi1, &a, 1, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi1, buffer, len, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, 1);
}

// ??????��???����???
void mget_ms(unsigned long *time) {}
void delay_ms(int ms)
{
  HAL_Delay(ms);
}
// ?��??HAL???????
uint8_t i2c_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data)
{
  int res = 0;
  hal_spi_write(reg_addr, (uint8_t *)data, length);
  return res;
}
uint8_t i2c_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data)
{
  int res = 0;
  hal_spi_read(reg_addr, (uint8_t *)data, length);
  return res;
}

#include <stdarg.h>
void usb_printf(const char *format, ...)
{
  uint8_t UserTxBufferFS[2048];
  va_list args;
  uint32_t length;

  va_start(args, format);
  length = vsnprintf((char *)UserTxBufferFS, APP_TX_DATA_SIZE, (char *)format, args);
  va_end(args);
  CDC_Transmit_FS(UserTxBufferFS, length);
}
void io_send(){
	HAL_GPIO_WritePin(out1_GPIO_Port, out1_Pin, 0);
  //HAL_GPIO_WritePin(out2_GPIO_Port, out2_Pin, 0);
  //HAL_GPIO_WritePin(out3_GPIO_Port, out3_Pin, 0);
	HAL_GPIO_WritePin(check_GPIO_Port, check_Pin, 0);
}
void io_init(){
  HAL_GPIO_WritePin(out1_GPIO_Port, out1_Pin, 1);
  //HAL_GPIO_WritePin(out2_GPIO_Port, out2_Pin, 1);
  //HAL_GPIO_WritePin(out3_GPIO_Port, out3_Pin, 1);
	HAL_GPIO_WritePin(check_GPIO_Port, check_Pin, 1);
}
float g=0;
int att=0;
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
  MX_SPI1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  float gyro_buffer[3], acc_buffer[3];
  
  /* ???MPU6500 */
  MPU6500_Init();

    MPU6500_get_buffer(gyro_buffer, acc_buffer);
     g = sqrtf((acc_buffer[0]) * (acc_buffer[0]) +
                    (acc_buffer[1]) * (acc_buffer[1]) +
                    (acc_buffer[2]) * (acc_buffer[2]));


	io_init();
	HAL_Delay(100);
  
 while (1)
  {
    usb_printf("=============\n");
    /* ???????????? */
    MPU6500_get_buffer(gyro_buffer, acc_buffer);
     g = sqrtf((acc_buffer[0]) * (acc_buffer[0]) +
                    (acc_buffer[1]) * (acc_buffer[1]) +
                    (acc_buffer[2]) * (acc_buffer[2]));
     if (g < 0.6)
     {
       io_send();
			 usb_printf("g<0.6\n");
			 att=1;
    }
		usb_printf("g:%f,%d\n",g,att);
		if(att){
			usb_printf("===== action over ========\n");
		}
//          HAL_GPIO_WritePin(out1_GPIO_Port, out1_Pin, 1);
//      HAL_GPIO_WritePin(out2_GPIO_Port, out2_Pin, 1);
//      HAL_GPIO_WritePin(out3_GPIO_Port, out3_Pin, 1);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //HAL_GPIO_TogglePin(debug_GPIO_Port,debug_Pin);
		
	//HAL_GPIO_TogglePin(check_GPIO_Port, check_Pin);
    //HAL_Delay(500);
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
