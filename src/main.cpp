#include <iostream>
#include <filesystem>
#include <string>
#include <locale>
#include <stdexcept>
#include <codecvt>

#include "../libs/img_handler/file_struct.h"
#include "../libs/img_handler/img_handler.h"
#include "../libs/restore/restore.h"

#ifdef _WIN32
#include "../libs/vhdx_handler/vhdx_handler.h"
#else
#include "../libs/linux_virtdisk_handler/linux_virtdisk_handler.h"
#endif

std::string wideToString(const std::wstring& wstr) {
    std::string str(wstr.length(), ' ');
    size_t convertedChars = 0;
    wcstombs(&str[0], wstr.c_str(), str.size());
    return str;
}


void handleBackupRestore(std::string backupFileName)
{
    file_structs::File_Layout fileLayout;
    readBackupFileLayout(fileLayout, backupFileName);

    std::filesystem::path curPath = std::filesystem::current_path();
    std::wstring wtargetVHDXPath = curPath.wstring() + L"\\vhdxout\\test.vhdx";

    CreateVDisk(wtargetVHDXPath, fileLayout.disks[0]._geometry.disk_size, fileLayout.disks[0]._geometry.bytes_per_sector);
    
    std::wstring diskPath;
    MountVDisk(wtargetVHDXPath, diskPath);
    restoreDisk(backupFileName, wideToString(diskPath), fileLayout);
    UpdateDiskProperties(diskPath);
}

int main(int argc, char* argv[])
{
    std::string backupFileName = "src/515CE701D2BB4A22-TestMBR-SP-NC-NE-00-00.mrimg";
    handleBackupRestore(backupFileName);
    std::cin.get();
}
