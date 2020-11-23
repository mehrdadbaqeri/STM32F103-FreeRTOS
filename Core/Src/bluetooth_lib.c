
#include <bluetooth_lib.h>

static uint8_t StringCompare (char str1[], char str2[], uint16_t length);
static void StringToLowerCase (char *str);
static void StringCommandSpliter (char **cmdParams);
static void SendInvalidCommandMsg();
static void WriteLedPin(LED_IDX ledIdx, FlagStatus flagStat);

char bluetoothReceiveCommand[50];
uint8_t bufferIndex = 0;
uint8_t ledBlinkRatePerSecond[MAX_NOF_LEDS] = {0, 0, 0};

/**
 * Compares the two given strings.
 *
 * @param char* str1 The first string.
 * @param char* str2 The second string.
 *
 * @return uint8_t
 */
static uint8_t StringCompare (char str1[], char str2[], uint16_t length)
{
  uint8_t comVAR = 0, i;
  StringToLowerCase (str1);
  StringToLowerCase (str2);
  for (i = 0; i < length; i++)
    {
      if (str1[i] == str2[i])
	comVAR++;
      else
	comVAR = 0;
    }

  if (comVAR == length)
    return 1;
  else
    return 0;
}

/**
 * Converts the given string to lower case.
 *
 * @param char* str The given string.
 */
inline static void StringToLowerCase (char *str)
{
  int i = 0;
  while (str[i] != '\0')
    {
      if (str[i] >= 'A' && str[i] <= 'Z')
	{
	  str[i] = str[i] + 32;
	}
      ++i;
    }
}

/**
 * Splits the received string command into the command and its parameters using
 * space delimiter.
 *
 * @param char** cmdParams An output parameter as a list of string to return the
 * command and its parameter. The first element is always the command which is
 * followed by maximum 4 parameters.
 */
static void StringCommandSpliter (char **cmdParams)
{
  uint8_t i = 0;
  char *cmdLine = strtok (bluetoothReceiveCommand, " ");
  while (cmdLine != NULL)
    {
      cmdParams[i++] = cmdLine;
      HAL_UART_Transmit(&huart2, (uint8_t*)cmdLine, strlen(cmdLine), 500);
      cmdLine = strtok (NULL, " ");
    }
}

/*
 * Send not valid message to UART
 */
static void SendInvalidCommandMsg(){
  strcat (bluetoothReceiveCommand, " is not a valid command!!!\n");
  UARTWriteString(bluetoothReceiveCommand);
}

/*
 * Write LED pin status
 *
 * @param ledIdx The LED index
 * @param flagStatus The LED status to write
 */
static void WriteLedPin(LED_IDX ledIdx, FlagStatus flagStat){
  ledBlinkRatePerSecond[ledIdx] = 0;
  if( ledIdx == LED_ONBOARD){
    HAL_GPIO_WritePin(LED_OnBoard_GPIO_Port, LED_OnBoard_Pin, flagStat);
  }
  else if(ledIdx == LED_GREEN){
    HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, flagStat);
  }
  else if(ledIdx == LED_RED){
    HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, flagStat);
  }
  else
    SendInvalidCommandMsg();
}

/**
 * Parse the Bluetooth UART string commands
 *
 *@Note: The command should look like:
 *[component e.g. led] [cmd like on/off] [component index e.g. 1] [extra params]
 */
void URATCommandParser ()
{
  char *cmd[5] = { "", "", "", "", "" };
  StringCommandSpliter (cmd);
  if(StringCompare((char*) cmd[0], (char*)"led", strlen("led"))){
    uint8_t ledIdx = atoi(cmd[2]);
    if(ledIdx > MAX_NOF_LEDS || ledIdx < 0){
      SendInvalidCommandMsg();
    }
    else {
      if(StringCompare((char*) cmd[1], (char*)"on", strlen("on")))
      {
	WriteLedPin(ledIdx, RESET);
      }
      else if(StringCompare((char*)cmd[1], (char*)"off", strlen("off")))
      {
	WriteLedPin(ledIdx, SET);
      }
      else if(StringCompare((char*)cmd[1], (char*)"blink", strlen("blink")))
      {
	ledBlinkRatePerSecond[ledIdx] = atoi(cmd[3]);
      }
      else
      {
	SendInvalidCommandMsg();
      }
    }
  }
  memset (bluetoothReceiveCommand, 0, sizeof(bluetoothReceiveCommand));
  bufferIndex = 0;
}

/*
 * Write the given string to the bluetooth UART channel.
 *
 * @param char* str The string to write into the bluetooth UART channel.
 */
void UARTWriteString(char* str){
  HAL_UART_Transmit (&huart2,(uint8_t*)str, strlen(str), 500);
}
