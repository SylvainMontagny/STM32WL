
#include "main.h"
#include "app_lorawan.h"
#include "sys_app.h"
#include "i2c.h"
#include "sys_sensors.h"
#include "stm32wlxx_hal_spi.h"
#include "st7789.h"
#include "lcd_printf.h"



#include "lora_app.h"
#include "sys_app.h"
#include "stm32_seq.h"

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_SPI1_Init(void);

SPI_HandleTypeDef hspi1;

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_I2C2_Init();

	LCD_Buffer_Init();

	SystemApp_Init();

	/*Initialize the Sensors */
	EnvSensors_Init();

	// LCD init
	MX_GPIO_Init();
	MX_SPI1_Init();
	ST7789_Init();

	// LCD test
	//ST7789_Fill_Color(LCD_BLACK);
//	lcd_printf(LCD_BLACK, "HELLO");
//	lcd_printf(LCD_RED, "World");
//	lcd_printf(LCD_BLUE, "!");
//	lcd_printf(LCD_GREEN, "iiiiiiiiiiiiiiiiiiii");
//	lcd_printf(LCD_GREEN, "wwwwwwwwwwwwwwwwwwww");
//	lcd_print_buf();
	lcd_printf(LCD_BLUE, "Device turned On");
	lcd_printf(LCD_BLUE, "Init LoRaWAN Stack...");
//	lcd_printf(LCD_BLUE, "----–––––—————");
//	lcd_printf(LCD_BLUE, "123456789012345678901234567890123456789");
	lcd_print_buf();

	LoRaWAN_Init();

	while (1)
	{
		MX_LoRaWAN_Process();
	}
}


void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure LSE Drive Capability
	 */
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
			|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
			|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
	GPIO_InitTypeDef CS_init;
	CS_init.Mode = GPIO_MODE_OUTPUT_PP;
	CS_init.Pin = GPIO_PIN_2;
	CS_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOC, &CS_init);

	GPIO_InitTypeDef RST_init;
	RST_init.Mode = GPIO_MODE_OUTPUT_PP;
	RST_init.Pin = GPIO_PIN_2;
	RST_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &RST_init);

	GPIO_InitTypeDef DC_init;
	DC_init.Mode = GPIO_MODE_OUTPUT_PP;
	DC_init.Pin = GPIO_PIN_10;
	DC_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &DC_init);

/* USER CODE END MX_GPIO_Init_2 */
}

void Error_Handler(void)
{
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
}
