#include <iostream>
#include <filesystem>
#include <string>
#include <locale>
#include <stdexcept>
#include <codecvt>

#include "../libs/img_handler/file_struct.h"
#include "../libs/img_handler/img_handler.h"
#include "../libs/restore/restore.h"

#include "../libs/vhdx_handler/vhdx_handler.h"

std::string wideToString(const std::wstring &wstr)
{
    std::string str(wstr.length(), ' ');
    size_t convertedChars = 0;
    wcstombs(&str[0], wstr.c_str(), str.size());
    return str;
}

void handleLinuxRestore(std::string backupFileName)
{
    file_structs::File_Layout fileLayout;
    readBackupFileLayout(fileLayout, backupFileName);

    std::filesystem::path curPath = std::filesystem::current_path();
    std::wstring vhdxPath = curPath.wstring() + L"\\test.vhdx";

    CreateVDisk(vhdxPath, fileLayout.disks[0]._geometry.disk_size, fileLayout.disks[0]._geometry.bytes_per_sector);

    std::cout << curPath << std::endl;

    std::wstring diskPath;
    MountVDisk(vhdxPath, diskPath);
    restoreDisk(backupFileName, wideToString(diskPath), fileLayout);
    UpdateDiskProperties(diskPath);
}

int main(int argc, char *argv[])
{
    std::string backupFileName = "Backup-Files/F9D9EADB46DEBD94-FATSingleTest-00-00.mrimg";
    handleLinuxRestore(backupFileName);
    std::cin.get();
}
