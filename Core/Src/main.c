/*
 * LAB Name: STM32 I2C LCD Example
 * Author: Khaled Magdy
 * For More Info Visit: www.DeepBlueMbedded.com
*/
#include "main.h"
#include <stdio.h>
#include "../../ECUAL/I2C_LCD/I2C_LCD.h"
#include "../../ECUAL/MQ2/MQ2.h"
#include "../../ECUAL/BUZZER/BUZZER.h"

#define MyI2C_LCD I2C_LCD_1
#define LCD_COLS  16
#define LABEL_COLS 10                      // "Air Toxic" + a space
#define BAR_COLS  (LCD_COLS - LABEL_COLS)  // 6 characters x 5 pixel columns = 30 steps
#define BAR_CHAR_FIRST 1                   // Custom chars 1-6 = bar cell with 0-5 columns filled (0 can't go in a C string)
#define LOOP_PERIOD_MS 250
#define ALARM_TONE_HZ  2500   // Passive buzzers are usually loudest around 2-4 kHz

ADC_HandleTypeDef hadc1;
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;
COM_InitTypeDef BspCOMInit;

// Indexed by MQ2_State_t, shared by the LCD and the terminal output
static const char* const StateNames[] = {"Warming up", "Air OK", "Air Toxic", "Air Very Toxic"};

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);

// Loads the bar cells into the LCD: a frame line on top and bottom, filled from the left
static void LCD_CreateBarChars(void)
{
	uint8_t CharMap[8];

	for (uint8_t Filled = 0; Filled <= 5; Filled++)
	{
		uint8_t Fill = (0x1F << (5 - Filled)) & 0x1F;
		CharMap[0] = 0x1F;
		for (uint8_t Row = 1; Row < 7; Row++)
		{
			CharMap[Row] = Fill;
		}
		CharMap[7] = 0x1F;
		I2C_LCD_CreateCustomChar(MyI2C_LCD, BAR_CHAR_FIRST + Filled, CharMap);
	}
}

// Writes Cells bar characters to Dest, filled to Percent (full only at 100%)
static void LCD_BuildBar(char* Dest, uint8_t Cells, uint8_t Percent)
{
	uint16_t FilledPx = ((uint16_t)Percent * Cells * 5) / 100;

	for (uint8_t i = 0; i < Cells; i++)
	{
		uint16_t CellPx = (FilledPx > i * 5) ? FilledPx - i * 5 : 0;
		if (CellPx > 5)
		{
			CellPx = 5;
		}
		Dest[i] = (char)(BAR_CHAR_FIRST + CellPx);
	}
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();

  /* Initialize COM1 port (115200, 8 bits, 1 stop bit, no parity)
   * COM1 = LPUART1 on PA2/PA3, wired to the ST-LINK USB Virtual COM Port.
   * The BSP sends printf() output here. */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  // Send each printf() right away instead of holding it in a buffer
  setvbuf(stdout, NULL, _IONBF, 0);
  printf("\r\nBIU Racing smoke sensor started\r\n");

  BUZZER_Init(&htim2, TIM_CHANNEL_2);
  // Short beep at power-up to show the buzzer works
  BUZZER_Tone(ALARM_TONE_HZ);
  HAL_Delay(100);
  BUZZER_Off();

  I2C_LCD_Init(MyI2C_LCD);
  LCD_CreateBarChars();
  I2C_LCD_SetCursor(MyI2C_LCD, 0, 0);
  I2C_LCD_WriteString(MyI2C_LCD, "BIU Racing");

  MQ2_Init(&hadc1);

  char Line[LCD_COLS + 1];
  uint8_t AlarmBlink = 0;

  while (1)
  {
	MQ2_State_t State = MQ2_Update();
	uint32_t mV = MQ2_GetMillivolts();
	uint8_t Percent = MQ2_GetAlarmPercent();

	printf("%s, MQ2: %lu mV, %u%% of toxic level\r\n", StateNames[State], mV, Percent);

	// Every line is padded to the full LCD width so it overwrites the previous one
	if (State == MQ2_WARMING_UP)
	{
		snprintf(Line, sizeof(Line), "%-12s%3lus", StateNames[State], MQ2_GetWarmupSecondsLeft());
	}
	else
	{
		// Label on the left, bar to the right: full bar = Air Toxic threshold
		snprintf(Line, sizeof(Line), "%-*s", LABEL_COLS, StateNames[State]);
		LCD_BuildBar(&Line[LABEL_COLS], BAR_COLS, Percent);
		Line[LCD_COLS] = '\0';
	}
	I2C_LCD_SetCursor(MyI2C_LCD, 0, 1);
	I2C_LCD_WriteString(MyI2C_LCD, Line);

	// While smoke is detected, flash the backlight and beep together every other loop
	AlarmBlink = (State == MQ2_TOXIC || State == MQ2_VERY_TOXIC) ? !AlarmBlink : 0;
	if (State != MQ2_TOXIC || State != MQ2_VERY_TOXIC || AlarmBlink)
	{
		I2C_LCD_Backlight(MyI2C_LCD);
	}
	else
	{
		I2C_LCD_NoBacklight(MyI2C_LCD);
	}
	if (AlarmBlink)
	{
		BUZZER_Tone(ALARM_TONE_HZ);
	}
	else
	{
		BUZZER_Off();
	}

	HAL_Delay(LOOP_PERIOD_MS);
  }
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  * HSI 16 MHz / PLLM 4 * PLLN 85 / PLLR 2 = 170 MHz
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;  /* 170 MHz / 4 = 42.5 MHz (max 60 MHz) */
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.GainCompensation = 0;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  * PA0 (Arduino A0) = ADC1_IN1. The longest sampling time suits the
  * high source impedance of the sensor's voltage divider.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
  /* Calibrate once while the ADC is still disabled */
  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x40B285C2;  /* 100 kHz with 170 MHz PCLK1 */
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 169;  /* 170 MHz / (169 + 1) = 1 MHz tick, see BUZZER_TIMER_TICK_HZ */
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 399;     /* 1 MHz / (399 + 1) = 2.5 kHz, changed by BUZZER_Tone() */
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;         /* 0% duty = silent */
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
