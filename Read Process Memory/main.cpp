#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <memoryapi.h>
#include <vector>

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

char* wcharToString(const WCHAR* wcharStr)
{
	char charStr[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, wcharStr, -1, charStr, MAX_PATH, nullptr, nullptr);
	return charStr;
}

DWORD getPPID(uint32_t pid)
{
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	HANDLE handleToFind = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(handleToFind, &pe))
	{
		do
		{
			if (pe.th32ProcessID == pid)
			{
				printf("PID: %i; PPID: %i\n", pid, pe.th32ParentProcessID);
				return pe.th32ParentProcessID;
			}
		} while (Process32Next(handleToFind, &pe));
	}
	return -1;
}

DWORD getPID(const char* processName) 
{
	DWORD pid = 0;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32 procEntry{};
	procEntry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnap, &procEntry)) 
	{
		do 
		{
			if (strcmp(procEntry.szExeFile,processName) == 0)
			{
				pid = procEntry.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnap, &procEntry));
	}

	CloseHandle(hSnap);

	return pid;
}

vector<DWORD> get_childPID(DWORD ppid) 
{
	vector<DWORD> pids;
	HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hp, &pe)) 
	{
		do 
		{
			if (pe.th32ParentProcessID == ppid) 
			{
				pids.push_back(pe.th32ProcessID);
			}
		} while (Process32Next(hp, &pe));
	}
	CloseHandle(hp);
	return pids;
}

unsigned long scanVirtualPages(HANDLE handleToProcess)
{
	SYSTEM_INFO sSysInfo = {};
	GetSystemInfo(&sSysInfo);
	MEMORY_BASIC_INFORMATION memInfo;

	void* currentScanAddress = 0;
	unsigned long copyAddress = 0;

	while (true)
	{
		SIZE_T readBytes =
			VirtualQueryEx(handleToProcess, currentScanAddress, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

		if (!readBytes)
		{
			return 0;
		}

		currentScanAddress = (char*)memInfo.BaseAddress + memInfo.RegionSize;

		if (memInfo.State == MEM_COMMIT)
		{

			if (memInfo.Protect == PAGE_READWRITE)
			{
				cout << "Found READWRITE page(s) at base address: " << memInfo.BaseAddress << " Size: "
					<< memInfo.RegionSize << " = pages count: " << memInfo.RegionSize / sSysInfo.dwPageSize << "\n";
			}

		}
	}
	return copyAddress;
}

int main()
{
	DWORD pid=0;
	DWORD childpid = 0;
	unsigned long PositionYAddr; 
	const char* gameName = "client.exe";
	int i = 0;
	while (pid == 0) 
	{
		wcout << "Waiting for " << gameName << " to run...                                    Tries: " << i;
		i++;
		pid = getPID(gameName);
		vector<DWORD> pidVec = get_childPID(pid);
		cout << " | Child Processes: ";
		for (int i = 0; i < pidVec.size(); i++) 
		{
			cout << " " << pidVec[i];
		}
		cout << '\n';
		Sleep(1000);
	}
	if (pid)
	{
		cout << "Found the game process!\n";

		HANDLE handleToProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

		if (handleToProcess)
		{ 
			PositionYAddr = scanVirtualPages(handleToProcess);

			Sleep(1000);
			//char dllName[] = "KPCinject.dll";
			char dllName[] = "Read Process Memory.dll";
			char dllPath[MAX_PATH] = { 0 };

			GetFullPathNameA(dllName, MAX_PATH, dllPath, NULL);

			LPVOID pszLibFileRemote = VirtualAllocEx(handleToProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			WriteProcessMemory(handleToProcess, pszLibFileRemote, dllPath, strlen(dllPath) + 1, NULL);
			
			HANDLE handleThread = CreateRemoteThread(handleToProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, pszLibFileRemote, NULL, NULL);

			WaitForSingleObject(handleThread, INFINITE);
			CloseHandle(handleThread);
			VirtualFreeEx(handleToProcess, dllPath, 0, MEM_RELEASE);
			CloseHandle(handleToProcess);
		}
		else
		{
			cout << "Failed to open process.\n";
		}

	}
	else
	{
		cout << "couldn't find victim process :/\n";
	}

	cin.ignore();
	cin.get();
	return 0;
}