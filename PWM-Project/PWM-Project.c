#include "main.h"
#include "lcd_txt.h"
#include <stdio.h>

TIM_HandleTypeDef htim2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

int main(void)
{
  int32_t DC = 0;          // Same session variable to control duty cycle (0 to 10000)
  uint8_t percentage = 0;  // To calculate CCR1 value as a percentage[cite: 25]
  char msg[16];

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init();

  /* Initialize LCD and start TIM2 in PWM mode on Channel 1 (PA15) */
  lcd_init();
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); //[cite: 27]

  /* Display initial value 0% */
  TIM2->CCR1 = DC; //[cite: 27]
  sprintf(msg, "CCR1: 0%%");
  lcd_puts(0, 0, msg);

  while (1)
  {
    /* 1. Ramp-up loop while the first button (PB0) is pressed and max limit 10000 is not reached */
    while ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == 0) && (DC < 10000)) //[cite: 27]
    {
      DC = DC + 100; //[cite: 27]
      TIM2->CCR1 = DC; //[cite: 27]

      /* Convert CCR1 value to percentage and display it */
      percentage = (DC * 100) / 10000; //[cite: 25]
      sprintf(msg, "CCR1: %d%%", percentage);
      lcd_clear();
      lcd_puts(0, 0, msg);

      HAL_Delay(50); // Step rate while holding the button and debouncing
    }

    /* 2. Ramp-down loop while the second button (PB1) is pressed and 0 is not reached */
    while ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0) && (DC > 0)) //[cite: 27]
    {
      DC = DC - 100; //[cite: 27]
      TIM2->CCR1 = DC; //[cite: 27]

      /* Convert CCR1 value to percentage and display it */
      percentage = (DC * 100) / 10000; //[cite: 25]
      sprintf(msg, "CCR1: %d%%", percentage);
      lcd_clear();
      lcd_puts(0, 0, msg);

      HAL_Delay(50);
    }
  }
}