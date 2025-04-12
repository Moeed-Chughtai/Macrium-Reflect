/**
 * @file vhdx_handler.h
 * @brief Windows VHDX virtual disk management functions
 * 
 * This file declares functions for creating, mounting, and managing VHDX virtual disks
 * on Windows systems. These functions provide a high-level interface to the Windows
 * Virtual Disk Service API.
 */

#pragma once

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <string>

/**
 * @brief Creates a new VHDX virtual disk
 * 
 * Creates a new VHDX virtual disk file with the specified size and sector size.
 * The disk is created as a fixed-size disk with the default block size.
 * 
 * @param path Path where the VHDX file should be created
 * @param size Total size of the virtual disk in bytes
 * @param sectorSize Size of each sector in bytes
 */
void CreateVDisk(std::wstring path, unsigned long long size, unsigned long sectorSize);

/**
 * @brief Mounts a VHDX virtual disk
 * 
 * Mounts an existing VHDX virtual disk file and returns the path to the mounted disk.
 * The disk is mounted with read/write access and no parent disks.
 * 
 * @param path Path to the VHDX file to mount
 * @param diskPath Output parameter that receives the path to the mounted disk
 */
void MountVDisk(std::wstring path, std::wstring& diskPath);

/**
 * @brief Updates the properties of a mounted disk
 * 
 * Forces Windows to re-read the disk properties after changes have been made.
 * This is typically called after writing data to the disk to ensure Windows
 * recognizes the changes.
 * 
 * @param diskPath Path to the mounted disk
 */
void UpdateDiskProperties(std::wstring diskPath);
