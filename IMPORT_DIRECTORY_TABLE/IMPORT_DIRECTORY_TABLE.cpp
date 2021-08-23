#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <iostream>
#include <string.h>

using namespace std;

int main()
{
    HANDLE hFile = CreateFileA("C:\\Users\\root\\Desktop\\Reversing\\PEview.exe",
        GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "CreateFileA error 0X%X" << GetLastError() << endl;
        return 0;
    }

    HANDLE hFileMappingObject = CreateFileMappingA(hFile, 0, PAGE_READONLY, 0, 0, 0);
    if (!hFileMappingObject) {
        cout << "CreateFileMappingA error 0X%X" << GetLastError() << endl;
        return 0;
    }

    char* pBaseOfFile = (char*)MapViewOfFile(hFileMappingObject, FILE_MAP_READ, 0, 0, 0);
    if (!pBaseOfFile) {
        cout << "MapViewOfFile error 0X%X" << GetLastError() << endl;
        return 0;
    }

    // dos header ptr
    PIMAGE_DOS_HEADER idh = (PIMAGE_DOS_HEADER)pBaseOfFile;

    PIMAGE_FILE_HEADER ifh = (PIMAGE_FILE_HEADER)((LONG)pBaseOfFile + idh->e_lfanew + 4);

    // optional header ptr
    PIMAGE_OPTIONAL_HEADER ioh = (PIMAGE_OPTIONAL_HEADER)((LONG)pBaseOfFile + idh->e_lfanew + 24);

    PIMAGE_SECTION_HEADER ish = (PIMAGE_SECTION_HEADER)((LONG)pBaseOfFile + idh->e_lfanew + 24 + ifh->SizeOfOptionalHeader);

    ish = (PIMAGE_SECTION_HEADER)((PBYTE)pBaseOfFile + idh->e_lfanew + 24 + ifh->SizeOfOptionalHeader + 40 * 4);

    PIMAGE_IMPORT_DESCRIPTOR iid;

    iid = (PIMAGE_IMPORT_DESCRIPTOR)((PBYTE)pBaseOfFile + ioh->DataDirectory[1].VirtualAddress
        - ish->VirtualAddress + ish->PointerToRawData);

    int file_offset = ioh->DataDirectory[1].VirtualAddress - ish->VirtualAddress + ish->PointerToRawData;

    printf("*************** [IMPORT Directory Table] ***********************************\n");
    for (; iid->OriginalFirstThunk; iid++) {
        printf("pFile: %04X | Data: %04X | Description: OriginalFirstThunk\n", file_offset, iid->OriginalFirstThunk);
        printf("pFile: %04X | Data: %04X | Description: TimeDataStamp\n", file_offset + 4, iid->TimeDateStamp);
        printf("pFile: %04X | Data: %04X | Description: ForwarderChain\n", file_offset + 8, iid->ForwarderChain);
        printf("pFile: %04X | Data: %04X | Description: Name | Value: ", file_offset + 12, iid->Name);
        // iid->Name은 로드하는 dll의 이름을 가리키고 있는 RVA 주소이므로, RAW로 변경한 후 값을 읽어 dll 이름을 추출함
        printf("%s\n", (char*)((PBYTE)pBaseOfFile + iid->Name - ish->VirtualAddress + ish->PointerToRawData));
        printf("pFile: %04X | Data: %04X | Description: FirstThunk\n", file_offset + 16, iid->FirstThunk);
        printf("------------------------------------------------------------------------\n");

        file_offset += 5;
    }

    return 0;
}