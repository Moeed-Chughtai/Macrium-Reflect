#include <iostream>
#include <filesystem>
#include <string>
#include "../libs/img_handler/file_struct.h"
#include "../libs/vhdx_handler/vhdx_handler.h"
#include "../libs/img_handler/img_handler.h"


void handleBackupFile(std::string backupFileName)
{
    file_structs::File_Layout fileLayout;
    readBackupFileLayout(fileLayout, backupFileName);
    std::cout << fileLayout.disks[0].partitions[0].data_blocks[0].file_position << std::endl;

    std::filesystem::path curPath = std::filesystem::current_path();
    std::wstring targetVHDXPath = curPath.wstring() + L"\\vhdxout\\test.vhdx";

    CreateVHDX(targetVHDXPath, fileLayout.disks[0]._geometry.disk_size, fileLayout.disks[0]._geometry.bytes_per_sector);
    MountVHDX(targetVHDXPath);
}

int main(int argc, char* argv[])
{
    std::string backupFileName = "src/515CE701D2BB4A22-TestMBR-SP-NC-NE-00-00.mrimg";
    handleBackupFile(backupFileName);
}
