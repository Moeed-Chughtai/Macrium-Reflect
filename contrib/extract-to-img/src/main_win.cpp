/**
 * @file main_win.cpp
 * @brief Windows implementation of the Macrium Reflect backup file extractor
 * 
 * This file contains the Windows-specific implementation for extracting and mounting
 * Macrium Reflect backup files. It handles the creation of VHDX virtual disks and
 * mounting them for data access.
 */

#include <iostream>
#include <filesystem>
#include <string>

#include "../libs/img_handler/file_struct.h"
#include "../libs/img_handler/img_handler.h"
#include "../libs/restore/restore.h"
#include "../libs/vhdx_handler/vhdx_handler.h"

/**
 * @brief Converts a wide string to a standard string
 * 
 * @param wstr Wide string to convert
 * @return std::string Converted standard string
 */
std::string wideToString(const std::wstring &wstr)
{
    std::string str(wstr.length(), ' ');
    size_t convertedChars = 0;
    wcstombs(&str[0], wstr.c_str(), str.size());
    return str;
}

/**
 * @brief Handles the restoration process on Windows systems
 * 
 * This function performs the following steps:
 * 1. Reads the backup file layout
 * 2. Creates a VHDX virtual disk
 * 3. Mounts the VHDX disk
 * 4. Restores the backup to the mounted disk
 * 5. Updates disk properties
 * 
 * @param backupFileName Path to the Macrium Reflect backup file
 */
void handleWinRestore(std::string backupFileName)
{
    // Read the backup file structure
    file_structs::File_Layout fileLayout;
    readBackupFileLayout(fileLayout, backupFileName);

    // Create output VHDX file in current directory
    std::filesystem::path curPath = std::filesystem::current_path();
    std::wstring vhdxPath = curPath.wstring() + L"\\test.vhdx";

    // Create VHDX file with correct geometry
    CreateVDisk(vhdxPath, fileLayout.disks[0]._geometry.disk_size, fileLayout.disks[0]._geometry.bytes_per_sector);

    // Mount the VHDX and restore backup
    std::wstring diskPath;
    MountVDisk(vhdxPath, diskPath);
    restoreDisk(backupFileName, wideToString(diskPath), fileLayout, 0);
    UpdateDiskProperties(diskPath);
}

/**
 * @brief Main entry point for Windows implementation
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
    handleWinRestore(backupFileName);
    return 0;
}
