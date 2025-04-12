/**
 * @file main_linux.cpp
 * @brief Linux implementation of the Macrium Reflect backup file extractor
 * 
 * This file contains the Linux-specific implementation for extracting and mounting
 * Macrium Reflect backup files. It handles the creation of disk images and mounting
 * them as loop devices for data access.
 */

#include <iostream>
#include <filesystem>
#include <string>

#include "../libs/img_handler/file_struct.h"
#include "../libs/img_handler/img_handler.h"
#include "../libs/restore/restore.h"

#include "../libs/linux_virtdisk_handler/linux_virtdisk_handler.h"

/**
 * @brief Handles the restoration process on Linux systems
 * 
 * This function performs the following steps:
 * 1. Reads the backup file layout
 * 2. Creates a raw disk image file
 * 3. Restores the backup to the image file
 * 4. Mounts the image as a loop device
 * 5. Waits for user input before unmounting
 * 
 * @param backupFileName Path to the Macrium Reflect backup file
 */
void handleLinuxRestore(std::string backupFileName)
{
    // Read the backup file structure
    file_structs::File_Layout fileLayout;
    readBackupFileLayout(fileLayout, backupFileName);

    // Create output image file in current directory
    std::filesystem::path curPath = std::filesystem::current_path();
    std::string imgPath = curPath.string() + "/test.img";

    // Create empty image file with correct geometry
    CreateIMG(imgPath, fileLayout.disks[0]._geometry.disk_size, fileLayout.disks[0]._geometry.bytes_per_sector);

    std::cout << "Working directory: " << curPath << std::endl;

    // Restore backup to image file
    std::string loopFilePath;
    restoreDisk(backupFileName, imgPath, fileLayout, 0);
    std::cout << "Restored backup to .img file" << std::endl;

    // Mount the image file
    MountIMG(imgPath, loopFilePath);
    std::cout << "Mounted .img to: " << loopFilePath << std::endl;

    // Wait for user input before unmounting
    std::cout << "Press Enter to unmount and exit..." << std::endl;
    std::cin.get();

    // Cleanup
    UnmountIMG(loopFilePath);
    std::cout << "Unmounted .img" << std::endl;
}

/**
 * @brief Main entry point for Linux implementation
 * 
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return int Exit code (0 for success, 1 for error)
 */
int main(int argc, char *argv[])
{
    if (argc == 1) {
        std::cout << "Error: No backup file specified" << std::endl;
        std::cout << "Usage: " << argv[0] << " <backup_file>" << std::endl;
        return 1;
    }

    std::string backupFileName = argv[1];
    handleLinuxRestore(backupFileName);
    return 0;
}
