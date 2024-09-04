#include <iostream>
#include <filesystem>
#include <string>
#include <locale>
#include <stdexcept>
#include <codecvt>

#include "../libs/img_handler/file_struct.h"
#include "../libs/img_handler/img_handler.h"
#include "../libs/restore/restore.h"

#include "../libs/linux_virtdisk_handler/linux_virtdisk_handler.h"

void handleLinuxRestore(std::string backupFileName)
{
    file_structs::File_Layout fileLayout;
    readBackupFileLayout(fileLayout, backupFileName);

    std::filesystem::path curPath = std::filesystem::current_path();
    std::string imgPath = curPath.string() + "/test.img";

    CreateIMG(imgPath, fileLayout.disks[0]._geometry.disk_size, fileLayout.disks[0]._geometry.bytes_per_sector);

    std::cout << curPath << std::endl;

    std::string loopFilePath;
    restoreDisk(backupFileName, imgPath, fileLayout);
    std::cout << "Restored to .img file" << std::endl;
    MountIMG(imgPath, loopFilePath);
    std::cin.get();
    UnmountIMG(loopFilePath);
}

int main(int argc, char *argv[])
{
    std::string backupFileName = "src/515CE701D2BB4A22-TestMBR-SP-NC-NE-00-00.mrimg";
    handleLinuxRestore(backupFileName);
}
