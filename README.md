# Deperature Bell Windows Program

This program uses VS2022 to build the project. Through the lines and stations selected by the user, the specified bgm is played according to the data passed in by the stm32 program.

## Getting Started

1. Download [SFML](https://www.sfml-dev.org/) Library. This program uses this library to play sounds.

2. Prepare or download Special Deperature bell sounds.

3. Create data file. This file needs to record the line, station, and audio path. The format is as follows:

```
#$START$#
line station sound_path
xxx xxx xxx
#$END$#
```

For example:

```
#$START$#
山手線 東京 D:\1.wav
山手線 神田 D:\2.wav
中央線 東京 D:\3.wav
#$END$#
```

PS: For information on the audio formats supported by the program, please see the SFML documentation on supported formats.

4. Replace the "utils.h" -> define field with the field that suits you.

> CONFIG_PATH 	--- 	Reserved Fields

> LOG_PATH 		---	log file path(wstring)

> OFF_PATH		---	The audio that plays after the user clicks the close button. like "２番線のドアが閉まります。ご注意ください。"

> PATH			---	data file(wstring)

> CODE_PAGE		---	Code page to use in performing the conversion(wstring <-> string). If you don't understand, you can try **CP_ACP.**

5. Replace the "STM_IO.h" -> define field with the field that suits you.

> BAUD_RATE		---	Microcontroller Serial Port Baud Rate

> COM_NAME		---	Device Name for Serical Communication

6. Compile, build, run.

After connecting to the microcontroller, the words will change from 'connecting' to 'connected' in the bottom right corner. Select the line and station, click 'start'. The program will listen for data sent by the microcontroller and play the specified music based on whether the user presses the ON or OFF button.
