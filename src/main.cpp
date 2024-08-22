#include <iostream>
#include <filesystem>
#include <string>
#include <locale>
#include <stdexcept>
#include <codecvt>

#include "../libs/img_handler/file_struct.h"
#include "../libs/vhdx_handler/vhdx_handler.h"
#include "../libs/img_handler/img_handler.h"
#include "../libs/restore/restore.h"


std::string wideToString(const std::wstring& wstr) {
    std::string str(wstr.length(), ' ');
    size_t convertedChars = 0;
    wcstombs_s(&convertedChars, &str[0], str.size(), wstr.c_str(), _TRUNCATE);
    return str;
}

void handleBackupRestore(std::string backupFileName)
{
    file_structs::File_Layout fileLayout;
    readBackupFileLayout(fileLayout, backupFileName);

    std::filesystem::path curPath = std::filesystem::current_path();
    std::wstring wtargetVHDXPath = curPath.wstring() + L"\\vhdxout\\test.vhdx";

    CreateVHDX(wtargetVHDXPath, fileLayout.disks[0]._geometry.disk_size, fileLayout.disks[0]._geometry.bytes_per_sector);
    
    std::wstring diskPath;
    MountVHDX(wtargetVHDXPath, diskPath);
    restoreDisk(backupFileName, wideToString(diskPath), fileLayout);
    UpdateDiskProperties(diskPath);
}

int main(int argc, char* argv[])
{
    std::string backupFileName = "src/515CE701D2BB4A22-TestMBR-SP-NC-NE-00-00.mrimg";
    handleBackupRestore(backupFileName);
    std::cin.get();
}
