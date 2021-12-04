#include <windows.h>
#include <stdio.h>

int main(void) {
	LPCTSTR pipename = L"\\\\.\\pipe\\mynamedpipe";
	HANDLE hNamedPipe;
	LPCVOID szAddress;
	DWORD cbRead, cbWritten;
	wchar_t* msg;

	HANDLE hMutex;
	hMutex = CreateMutex(NULL, TRUE, L"Yana");
	hNamedPipe = CreateNamedPipe(
		pipename,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE |
		PIPE_READMODE_MESSAGE |
		PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		sizeof(DWORD),
		sizeof(DWORD),
		100,
		NULL);

	ConnectNamedPipe(hNamedPipe, NULL);

	ReadFile(
		hNamedPipe,
		&szAddress,
		sizeof(szAddress),
		&cbRead,
		NULL);

	wprintf(L"your string: ");
	msg = (LPCWCHAR*)szAddress;
	wprintf_s(L"%s\n", msg);

	ReleaseMutex(hMutex);

	DisconnectNamedPipe(hNamedPipe);

	CloseHandle(hNamedPipe);

	system("pause");
}