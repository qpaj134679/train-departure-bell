#include "STM_IO.h"


bool STM_IO::__OpenCom() {
	hCom = CreateFileW(COM_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (hCom == INVALID_HANDLE_VALUE) {
		utils::OutputLog(3, "failed to open com because INVALID_HANDLE_VALUE");
		return false;
	}
	DCB bcd{};
	if (!GetCommState(hCom, &bcd)) {
		utils::OutputLog(3, "failed to open com because com state is wrong");
		__CloseCom();
		return false;
	}

	bcd.BaudRate			= BAUD_RATE;
	bcd.fBinary				= TRUE;
	bcd.fParity				= TRUE;
	bcd.fOutxCtsFlow		= FALSE;
	bcd.fOutxDsrFlow		= FALSE;
	bcd.fDtrControl			= DTR_CONTROL_ENABLE;
	bcd.fDsrSensitivity		= FALSE;
	bcd.fTXContinueOnXoff	= FALSE;
	bcd.fOutX				= FALSE;
	bcd.fInX				= FALSE;
	bcd.fErrorChar			= FALSE;
	bcd.fNull				= FALSE;
	bcd.fRtsControl			= RTS_CONTROL_ENABLE;
	bcd.ByteSize			= 8;

	if (!SetCommState(hCom, &bcd)) {
		utils::OutputLog(3, "failed to open com because cannot set com state");
		__CloseCom();
		return false;
	}

	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 1000;
	timeout.ReadTotalTimeoutMultiplier = 10;
	timeout.WriteTotalTimeoutConstant = 1000;
	timeout.WriteTotalTimeoutMultiplier = 10;
	if (!SetCommTimeouts(hCom, &timeout)) {
		utils::OutputLog(3, "failed to open com because cannot set COMMTIMEOUTS");
		__CloseCom();
		return false;
	}
	bConStatus = true;
	utils::OutputLog(3, "open com success");
	return true;
}

void STM_IO::__CloseCom() {
	CloseHandle(hCom);
	bConStatus = false;
	hCom = INVALID_HANDLE_VALUE;

	// set words - "connecting"
	InvalidateRect(hWindow, NULL, TRUE);
	UpdateWindow(hWindow);
}

//
//  FUNCTION: __SendData(send_DB)
//
//  PURPOSE: Send data to stm32.
//
bool STM_IO::__SendData(send_DB sendDB) {
	if (!bConStatus)
		return false;
	DWORD bytesWritten = 0;
	OVERLAPPED overlapped = { 0 };
	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (overlapped.hEvent == NULL)
		return false;

	char data[2] = { sendDB.cData, sendDB.cType };
	bool bRst = WriteFile(hCom, data, 2, &bytesWritten, &overlapped);
	if (!bRst) {
		if (GetLastError() != ERROR_IO_PENDING) {
			utils::OutputLog(2, "SendData: cannot write data to serial:" + std::to_string(GetLastError()));
			return false;
		}
		else
			WaitForSingleObject(overlapped.hEvent, 1000);
	}
	return true;
}

//
//  FUNCTION: send_DB __RecvData()
//
//  PURPOSE: receive data from stm32. if data="FF" means error.
//
send_DB STM_IO::__RecvData() {
	if (!bConStatus)
		return {'F', 'F'};
	DWORD bytesRead = 0;
	OVERLAPPED overlapped = { 0 };
	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (overlapped.hEvent == NULL)
		return { 'F', 'F' };
	char data[2] = { 'F', 'F'};
	bool bRst = ReadFile(hCom, data, 2, &bytesRead, &overlapped);
	if (!bRst) {
		if (GetLastError() != ERROR_IO_PENDING) {
			utils::OutputLog(2, "RecvData: cannot get data from stm32:" + std::to_string(GetLastError()));
			return { 'F', 'F' };
		}
		else
			WaitForSingleObject(overlapped.hEvent, 1000);
	}
	send_DB returnDB = { data[0], data[1] };
	PurgeComm(hCom, PURGE_RXCLEAR | PURGE_TXCLEAR);
	return returnDB;
}

//
//  FUNCTION: send(char, char)
//
//  PURPOSE: construct send_DB data, and push it to the list.
//
void STM_IO::send(char cType, char cData) {
	send_DB tDB = { cType, cData };
	sendMutex.lock();
	lstSend.push_back(tDB);
	sendMutex.unlock();
}

//
//  FUNCTION: GetStatus()
//
//  PURPOSE: return whether is connected to stm32.
//
bool STM_IO::GetStatus() {
	return bConStatus;
}

//
//  FUNCTION: PlaySound()
//
//  PURPOSE: play sound. Audio is determined by the line and station selected by the user.
//
bool STM_IO::PlaySound() {
	std::string sPath = utils::WstringToString(sSoundPath);
	if (sSoundPath.length() == 0 || sound.getStatus() == 2/*some music is playing*/) {
		return false;
	}
	return PlaySound(sPath);
}

//
//  FUNCTION: PlaySound(string)
//
//  PURPOSE: play sound.
//
bool STM_IO::PlaySound(std::string sPath) {
	if (sPath.length() == 0 || sound.getStatus() == 2/*some music is playing*/) {
		return false;
	}

	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile(sPath)) {
		utils::OutputLog(2, "play_sound: cannot load music data from file " + sPath);
		return false;
	}

	sound.setBuffer(buffer);

	sound.play();
	// synchronization
	// while (sound.getStatus() == sf::Sound::Playing)
	// {
	// }
	return true;
}

//
//  FUNCTION: btnClick(wstring)
//
//  PURPOSE: when user pause "start" button, send path and save.
//
bool STM_IO::btnClick(std::wstring sPath) {
	if (bBtnPlay)
		sSoundPath = L"";
	else
		sSoundPath = sPath;
	bBtnPlay = !bBtnPlay;
	return bBtnPlay;
}

STM_IO::STM_IO():
	bConStatus(false), bBtnPlay(false), sSoundPath(L""), stop_thread(false),
	hWindow(nullptr), hIcon(nullptr), hWord(nullptr), hInst(nullptr), hCom(INVALID_HANDLE_VALUE)
{
	mainThread = std::thread([this]() { __run(); });
	mainThread.detach();
}

STM_IO::~STM_IO() {
	stop_thread = true;
	if (hCom != INVALID_HANDLE_VALUE)
		__CloseCom();
}

void STM_IO::__run() {
	while (!stop_thread) {
		if (!bConStatus) {
			// connect
			while (!__OpenCom()) {
				utils::OutputLog(2, "cannot open com");
				Sleep(1000);
			}
			bConStatus = true;
			InvalidateRect(hWindow, NULL, TRUE);
			UpdateWindow(hWindow);
		}
		else {
			// check com status
			DCB bcd{};
			if (!GetCommState(hCom, &bcd)) {
				__CloseCom();
				continue;
			}

			// check recv
			send_DB recvDB = __RecvData();
			if (!(recvDB.cData == 'F' && recvDB.cType == 'F')) {
				if (recvDB.cType == 's') {
					// the message is sent by stm32
					if (recvDB.cData == '0') {
						PlaySound(std::string(OFF_PATH));
					}
					else if (recvDB.cData == '1') {
						PlaySound();
					}
				}
			}

			// check send_list
			send_DB sendDB = { 'F', 'F' };
			sendMutex.lock();
			if (lstSend.size() > 0) {
				// prepare data
				sendDB = lstSend.front();
				lstSend.pop_front();
				sendMutex.unlock();
				COMSTAT comstat;
				DWORD dwErrors;
				ClearCommError(hCom, &dwErrors, &comstat);
				if (sendDB.cData == 'F' && sendDB.cType == 'F')
					continue;

				// send
				if (!__SendData(sendDB))
					utils::OutputLog(2, "Failed to send data, type=" + std::string(1, sendDB.cType) + ", data=" + std::string(1, sendDB.cData));
				else
					utils::OutputLog(3, "send data, type=" + std::string(1, sendDB.cType) + ", data=" + std::string(1, sendDB.cData));
			}
			else
				sendMutex.unlock();
		}
	}
}

void STM_IO::SetWindowHWND(HWND hWnd) {
	hWindow = hWnd;
}
