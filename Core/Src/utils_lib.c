#include <utils_lib.h>
#include <math.h>
#include <stdio.h>

#include "main.h"

/**
 * Delay in microseconds.
 *
 * No need to check an overflow. Let it just tick :)
 *
 * @param uint32_t us  Number of microseconds to delay for
 */
void DelayUSec (uint32_t us) // microseconds
{
  uint16_t tim1Counter = __HAL_TIM_GET_COUNTER(&htim1);
  // wait for the counter to reach the us input in the parameter
  while ( (__HAL_TIM_GET_COUNTER(&htim1) - tim1Counter) < us);
}
