#include <Windows.h>
#include <iostream>
#include <iomanip>

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

    PIMAGE_FILE_HEADER pImage_File_Header = (PIMAGE_FILE_HEADER)((LONG)pImage_Dos_Header + pImage_Dos_Header->e_lfanew + 4);

    /*
    * BaseOfFile + (PIMAGE_DOS_HEADER->e_lfanew) + Signature(4bytes) + (IMAGE_FILE_HEADER 크기)
    * + (PIMAGE_FILE_HEADER->SizeOfOptionalHeader) == PIMAGE_SECTION_HEADER
    * 이 경우엔 IMAGE_FILE_HEADER의 크기 : 0x14 (20)
    */
    PIMAGE_SECTION_HEADER pImage_Section_Header = (PIMAGE_SECTION_HEADER)((LONG)pImage_Dos_Header + pImage_Dos_Header->e_lfanew + 4 + 0x14
        + pImage_File_Header->SizeOfOptionalHeader);

    LONG file_offset;

    // Section의 수만큼 반복
    for (int i = 0; i < (pImage_File_Header->NumberOfSections); i++) {
        file_offset = pImage_Dos_Header->e_lfanew + 4 + 0x14 + pImage_File_Header->SizeOfOptionalHeader + 40 * i;

        printf("Section %s\n", pImage_Section_Header->Name);
        // Address of Section Name
        printf("pFile: %04X | Data: %04X | Description: Name\n", file_offset, pImage_Section_Header->Name);
        printf("pFile: %04X | Data: %04X | Description: Virtual Size\n", file_offset + 8, pImage_Section_Header->VirtualAddress);
        printf("pFile: %04X | Data: %04X | Description: SizeOfRawData\n", file_offset + 12, pImage_Section_Header->SizeOfRawData);
        printf("pFile: %04X | Data: %04X | Description: PointerToRawData\n\n", file_offset + 16, pImage_Section_Header->PointerToRawData);

        pImage_Section_Header++;
    }

    return 0;
}