#include <iostream>
#include <filesystem>
#include <string>
#include "../libs/img_handler/file_struct.h"
#include "../libs/vhdx_handler/vhdx_handler.h"
#include "../libs/img_handler/img_handler.h"


void handleBackup()
{
    // std::string backupFileName = "src/515CE701D2BB4A22-TestMBR-SP-NC-NE-00-00.mrimg";
    // file_structs::File_Layout file_layout;
    // readBackupFileLayout(file_layout, backupFileName);
    // std::cout << file_layout.disks[0].partitions[0].data_blocks[0].file_position << std::endl;

    // std::filesystem::path curPath = std::filesystem::current_path();
    // std::wstring targetPath = curPath.wstring() + L"\\vhdxout\\test.vhdx";

    // CreateVHDX(targetPath, file_layout.disks[0]._geometry.disk_size, file_layout.disks[0]._geometry.bytes_per_sector);
    MountVHDX(L"C:\\Test (New).vhdx");
}

int main(int argc, char* argv[])
{
    handleBackup();
    return 0;
}
