#include <windows.h>
#include <stdio.h>
#include <TlHelp32.h>

#define MAXLENGTH 20

DWORD GetProcessID(wchar_t* process);

int main(void)
{
	LPCTSTR pipename = L"\\\\.\\pipe\\mynamedpipe";
	HANDLE hProcessB;
	LPVOID szAddress;
	DWORD cbWritten;
	DWORD szSize = sizeof(wchar_t) * MAXLENGTH;
	wchar_t* str = (wchar_t*)malloc(szSize);
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	HANDLE hMutex = NULL;

	printf("Enter your string: ");
	_getws_s(str, MAXLENGTH - 1);

	hProcessB = OpenProcess(
		PROCESS_ALL_ACCESS,
		FALSE,
		GetProcessID(L"Proc_B.exe"));

	szAddress = VirtualAllocEx(
		hProcessB,
		0,
		szSize,
		MEM_RESERVE |
		MEM_COMMIT,
		PAGE_EXECUTE_READWRITE);

	WriteProcessMemory(
		hProcessB,
		szAddress,
		str,
		szSize,
		0);

	while (hPipe == INVALID_HANDLE_VALUE) {
		hPipe = CreateFile(
			pipename,
			GENERIC_READ |
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	}

	WriteFile(
		hPipe,
		&szAddress,
		sizeof(szAddress),
		&cbWritten,
		NULL);

	hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"Yana");

	WaitForSingleObject(hMutex, INFINITE);

	VirtualFreeEx(
		hProcessB,
		szAddress,
		0,
		MEM_RELEASE);

	ReleaseMutex(hMutex);

	CloseHandle(hPipe);

	CloseHandle(hProcessB);

	system("pause");
}


DWORD GetProcessID(wchar_t* process)
{
	PROCESSENTRY32 processEntry;
	DWORD processID = 0;
	HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Snapshot != INVALID_HANDLE_VALUE) {
		processEntry.dwSize = sizeof(processEntry);
		Process32First(Snapshot, &processEntry);

		do {
			if (wcscmp(processEntry.szExeFile, process) == 0) {
				processID = processEntry.th32ProcessID;
				break;
			}
		} while (Process32Next(Snapshot, &processEntry));

		CloseHandle(Snapshot);
	}

	return processID;
}