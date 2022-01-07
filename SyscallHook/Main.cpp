#include "Native.hpp"
#include "Syscall Hook.hpp"
//#include <iostream>
//#include <fstream>

//const std::wstring PATH = L"Ovil-21";
const PWCH PATH = L"Ovil-21";
const USHORT LENGTH = 7;

BOOL isIn(PWCH s1, USHORT l1, PWCH s2, USHORT l2) {
	BOOL flag = false;
	for (int i = 0; i < l1; i++) {
		flag = true;
		for (int j = 0; j < l2; i++) {
			if (i + j >= l2)
				return false;
			if (s1[i + j] != s2[j]) {
				flag = false;
				break;
			}
		}
		if (flag)
			return true;
	}
	return false;
}


NTSTATUS ntCreateFileHook(PHANDLE fileHandle, ACCESS_MASK desiredAccess, POBJECT_ATTRIBUTES objectAttributes,
                          PIO_STATUS_BLOCK ioStatusBlock, PLARGE_INTEGER allocationSize, ULONG fileAttributes,
                          ULONG shareAccess, ULONG createDisposition, ULONG createOptions, PVOID eaBuffer,
                          ULONG eaLength) {
	return STATUS_SUCCESS;
//	//DbgPrintEx(0, 0, "NtCreateFile: %ws\n", objectAttributes->ObjectName->Buffer);
	//std::wstring name = std::wstring(objectAttributes->ObjectName->Buffer, objectAttributes->ObjectName->Length);

	if (! isIn(objectAttributes->ObjectName->Buffer, objectAttributes->ObjectName->Length, PATH, LENGTH)) {
		return NtCreateFile(fileHandle, desiredAccess, objectAttributes, ioStatusBlock, allocationSize, fileAttributes,
			shareAccess, createDisposition, createOptions, eaBuffer, eaLength);
	}
	else {
		// std::ofstream f("c:\\\\fool.txt");
		// f << "you can't open this..." << std::endl;
		PHANDLE new_fileHandle{};
		POBJECT_ATTRIBUTES new_ObjectAttributes{};
		UNICODE_STRING new_path{};
		PWCH new_name = L"c:\\\\fool.txt";
		new_path.Buffer = new_name;
		new_path.Length = 8;
		new_path.MaximumLength = 8;
		InitializeObjectAttributes(new_ObjectAttributes, &new_path, OBJ_OPENIF, NULL, NULL);
		PIO_STATUS_BLOCK new_ioStatusBlock{};
		objectAttributes->ObjectName = &new_path;
		NTSTATUS ntstatus = ZwCreateFile(new_fileHandle, FILE_GENERIC_WRITE, new_ObjectAttributes, new_ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, NULL, FILE_OVERWRITE_IF, FILE_NON_DIRECTORY_FILE, NULL, 0);
		if (NT_SUCCESS(ntstatus)) {
			PIO_STATUS_BLOCK IoStatusBlock{};
			PVOID buffer = "lol you can't open this...\n";
			ntstatus = ZwWriteFile(new_fileHandle, NULL, NULL, NULL, ioStatusBlock,
				buffer, 27, NULL, NULL);
			ZwClose(new_fileHandle);
		}
		
		return ZwCreateFile(fileHandle, desiredAccess, objectAttributes, ioStatusBlock, allocationSize, fileAttributes,
			shareAccess, createDisposition, createOptions, eaBuffer, eaLength);
	}
}

NTSTATUS DriverEntry(const PDRIVER_OBJECT driverObject, const PUNICODE_STRING registryPath) {
	// Hook NtCreateFile for this example
	hookSystemCall(reinterpret_cast<std::uintptr_t>(&ntCreateFileHook),
                   reinterpret_cast<std::uintptr_t>(&NtCreateFile));

	return STATUS_SUCCESS;
}