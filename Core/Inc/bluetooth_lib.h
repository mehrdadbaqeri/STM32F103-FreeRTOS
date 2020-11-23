#ifndef INC_BLUETOOTH_LIB_H_
#define INC_BLUETOOTH_LIB_H_

#include "main.h"
#include <string.h>

#define MAX_NOF_LEDS 3

typedef enum{
  LED_ON = 0,
  LED_OFF = 1,
  LED_BLINK =2
} LED_CMD;

extern uint8_t ledBlinkRatePerSecond[MAX_NOF_LEDS];
extern char bluetoothReceiveCommand[50];
extern uint8_t bufferIndex;
extern UART_HandleTypeDef huart2;

void URATCommandParser();
void UARTWriteString(char* str);

#endif /* INC_BLUETOOTH_LIB_H_ */
