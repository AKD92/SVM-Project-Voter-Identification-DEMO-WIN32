


#include <stdio.h>
#include <windows.h>


#define PORT_FORMAT "\\\\.\\%s"




static HANDLE hCommSerial = INVALID_HANDLE_VALUE;





int serial_begin(char *sComPort, unsigned int baudrate) {
    
	char strPort[30];
	DCB dcbSerial;
	COMMTIMEOUTS tmtSerial;
	unsigned int fRes;
	HANDLE hComm;
    
	sprintf(strPort, PORT_FORMAT, sComPort);
	hComm = INVALID_HANDLE_VALUE;
	hComm = CreateFile(strPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (hComm == INVALID_HANDLE_VALUE) {
		return 1;
	}
//	else if (hComm == ERROR_FILE_NOT_FOUND) {
//        return 1;
//	}
    
	PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    
	dcbSerial.DCBlength = sizeof(DCB);
	fRes = GetCommState(hComm, &dcbSerial);
	if (fRes == 0) {
		CloseHandle(hComm);
		return 1;
	}
    
	dcbSerial.BaudRate = baudrate;
	dcbSerial.ByteSize = 8;
	dcbSerial.StopBits = ONESTOPBIT;
	dcbSerial.Parity = NOPARITY;
	fRes = SetCommState(hComm, &dcbSerial);
	if (fRes == 0) {
		CloseHandle(hComm);
		return 2;
	}
    
	tmtSerial.ReadIntervalTimeout = 0;
	tmtSerial.ReadTotalTimeoutConstant = 0;
	tmtSerial.ReadTotalTimeoutMultiplier = 0;
	tmtSerial.WriteTotalTimeoutConstant = 0;
	tmtSerial.WriteTotalTimeoutMultiplier = 0;
	fRes = SetCommTimeouts(hComm, &tmtSerial);
	if (fRes == 0) {
		CloseHandle(hComm);
		return 3;
	}
    
	hCommSerial = hComm;
	return 0;
}




unsigned int serial_read(unsigned char *bufData, unsigned int len) {
    
	unsigned int readCount;
	unsigned int fRes, index;
    
	if (bufData == 0 || hCommSerial == INVALID_HANDLE_VALUE) {
        return 0;
	}
    
	index = readCount = 0;
	while (index < len) {
		fRes = ReadFile(hCommSerial, bufData + index, 1, (DWORD *) &readCount, 0);
		if (readCount == 0 || fRes == 0) {
			break;
		}
		index = index + 1;
	}
    return (index);
}




unsigned int serial_write(unsigned char *bufData, unsigned int len) {
    
	unsigned int writeCount;
	unsigned int fRes, index;
    
	if (bufData == 0 || hCommSerial == INVALID_HANDLE_VALUE) {
        return 0;
	}
    
	index = writeCount = 0;
	while (index < len) {
		fRes = WriteFile(hCommSerial, bufData + index, 1, (DWORD *) &writeCount, 0);
		if (writeCount == 0 || fRes == 0) {
			break;
		}
		index = index + 1;
	}
	FlushFileBuffers(hCommSerial);
	return (index);
}




void serial_end(void) {
    
	CloseHandle(hCommSerial);
	hCommSerial = INVALID_HANDLE_VALUE;
	return;
}



