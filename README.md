# train-departure-bell stm32
This code is based on the STMicroelectronics STM32F103C8T6 microcontroller. The basic debug code was generated using STM32CubeMX, and the PA1 pin was configured to connect with the mechanical switch. Therefore, this branch contains a large number of STM32 modules. The main program code is located in `Core\Src\main.c`. The program determines the content to be sent based on the voltage level of the PA1 pin.
