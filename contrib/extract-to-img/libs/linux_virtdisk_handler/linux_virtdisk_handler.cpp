/**
 * @file linux_virtdisk_handler.cpp
 * @brief Implementation of Linux virtual disk management functions
 * 
 * This file implements the functionality for creating, mounting, and managing
 * virtual disk images on Linux systems using the loop device interface.
 */

#include "linux_virtdisk_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/**
 * @brief Runs a shell command and captures its output
 * 
 * Executes a shell command and captures its output line by line.
 * The output is stored in the provided string reference.
 * 
 * @param command The shell command to execute
 * @param output Reference to store the command output
 */
void RunCommandWithOutput(std::string command, std::string &output)
{
    FILE *fp = popen(command.c_str(), "r");
    char c = fgetc(fp);
    do
    {
        output += c;
        c = fgetc(fp);
    } while (c != '\n');
    pclose(fp);
}

/**
 * @brief Creates a new disk image file
 * 
 * Creates a new disk image file filled with zeros using dd command.
 * The image is created with the specified size and sector size.
 * Sets appropriate permissions on the created file.
 * 
 * @param imgPath Path where the image file should be created
 * @param size Total size of the image in bytes
 * @param sectorSize Size of each sector in bytes
 */
void CreateIMG(std::string imgPath, unsigned long long size, unsigned long sectorSize)
{
    uint32_t count = size / sectorSize;
    system(("sudo dd if=/dev/zero of=" + imgPath + " bs=" + std::to_string(sectorSize) + " count=" + std::to_string(count)).c_str());
    system(("sudo chmod 777 " + imgPath).c_str());
}

/**
 * @brief Mounts a disk image using loop device
 * 
 * Mounts an existing disk image file using the loop device interface.
 * The function finds an available loop device and attaches the image to it.
 * 
 * @param imgPath Path to the image file to mount
 * @param loopFilePath Output parameter that receives the path to the loop device
 */
void MountIMG(std::string imgPath, std::string &loopFilePath)
{
    RunCommandWithOutput("sudo losetup -f", loopFilePath);
    system(("sudo losetup -P " + loopFilePath + " " + imgPath).c_str());
}

/**
 * @brief Unmounts a disk image from its loop device
 * 
 * Detaches a disk image from its loop device, making the loop device
 * available for other uses.
 * 
 * @param loopFilePath Path to the loop device to detach
 */
void UnmountIMG(std::string loopFilePath)
{
    system(("sudo losetup -d" + loopFilePath).c_str());
}
