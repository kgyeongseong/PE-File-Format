#include <Windows.h>
#include <iostream>

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

    PIMAGE_DOS_HEADER pImage_Dos_Header = (PIMAGE_DOS_HEADER)pBaseOfFile;

    PIMAGE_NT_HEADERS pImage_NT_Headers = (PIMAGE_NT_HEADERS)((LONG)pImage_Dos_Header + pImage_Dos_Header->e_lfanew);
    cout << uppercase << hex << pImage_NT_Headers->Signature << endl;

    return 0;
}