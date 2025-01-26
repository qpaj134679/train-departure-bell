#pragma once
#include <thread>
#include <vector>
#include <mutex>
#include <deque>
#include <atomic>

#include <SFML/Audio.hpp>
#include "eki.h"
#include "utils.h"


#define BAUD_RATE CBR_115200
#define COM_NAME L"\\\\.\\COM3"

#pragma comment(lib, "SetupAPI.lib")

typedef struct send_DataBase {
	char cType;  // 'w': windows send to stm32; 's': stm32 send to windows
	char cData;  // '1': on, '0': off
}send_DB;

class STM_IO final
{
private:
	// scan send list and recv list
	std::thread mainThread;
	std::deque<send_DB> lstSend;
	std::mutex sendMutex;
	std::atomic<bool> stop_thread;

	bool bConStatus;	// whether is connected to com
	bool bBtnPlay;  // whether press "start" button
	
	std::wstring sSoundPath;  // play sound path
	
	sf::Sound sound;

	HANDLE hCom;
	HWND hIcon, hWord, hWindow;
	HINSTANCE hInst;

	// Serial Port Operation
	bool __OpenCom();
	void __CloseCom();
	bool __SendData(send_DB);
	send_DB __RecvData();

	void __run();

public:
	STM_IO();
	~STM_IO();

	void send(char, char);
	bool GetStatus();

	// play sound
	bool PlaySound();
	bool PlaySound(std::string);

	bool btnClick(std::wstring);
	void SetWindowHWND(HWND);
};

