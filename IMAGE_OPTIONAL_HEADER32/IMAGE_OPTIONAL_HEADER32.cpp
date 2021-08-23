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

    // BaseOfFile + (PIMAGE_DOS_HEADER->e_lfanew) + Signature (4bytes) + (IMAGE_FILE_HEADER 크기) == PIMAGE_OPTIONAL_HEADER
    // 이 경우엔 IMAGE_FILE_HEADER의 크기 : 0x14 (20)
    PIMAGE_OPTIONAL_HEADER32 pImage_Optional_Header32 = (PIMAGE_OPTIONAL_HEADER32)((LONG)pImage_Dos_Header + pImage_Dos_Header->e_lfanew + 4 + 0x14);
    cout << uppercase << hex << pImage_Optional_Header32->Magic << endl;
    
    return 0;
}