/**
 * @file linux_virtdisk_handler.h
 * @brief Linux virtual disk management functions
 * 
 * This file declares functions for creating, mounting, and managing virtual disk
 * images on Linux systems using the loop device interface.
 */

#include <string>

/**
 * @brief Creates a new disk image file
 * 
 * Creates a new disk image file filled with zeros using dd command.
 * The image is created with the specified size and sector size.
 * 
 * @param imgPath Path where the image file should be created
 * @param size Total size of the image in bytes
 * @param sectorSize Size of each sector in bytes
 */
void CreateIMG(std::string imgPath, unsigned long long size, unsigned long sectorSize);

/**
 * @brief Mounts a disk image using loop device
 * 
 * Mounts an existing disk image file using the loop device interface.
 * The function finds an available loop device and attaches the image to it.
 * 
 * @param imgPath Path to the image file to mount
 * @param loopFilePath Output parameter that receives the path to the loop device
 */
void MountIMG(std::string imgPath, std::string &loopFilePath);

/**
 * @brief Unmounts a disk image from its loop device
 * 
 * Detaches a disk image from its loop device, making the loop device
 * available for other uses.
 * 
 * @param loopFilePath Path to the loop device to detach
 */
void UnmountIMG(std::string loopFilePath);
