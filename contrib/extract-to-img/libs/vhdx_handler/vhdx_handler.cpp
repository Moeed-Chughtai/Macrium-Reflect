/**
 * @file vhdx_handler.cpp
 * @brief Implementation of Windows VHDX virtual disk management
 * 
 * This file implements the functionality for creating, mounting, and managing
 * VHDX virtual disks on Windows systems using the Windows Virtual Disk Service API.
 * It provides a high-level interface to create and manage virtual disks.
 */

#include <iostream>
#include <filesystem>
#include "vhdx_handler.h"
#include <winioctl.h>
#include <memory>
#include <objbase.h>
#include <virtdisk.h>

// https://learn.microsoft.com/en-us/windows/win32/api/virtdisk/nf-virtdisk-createvirtualdisk

#pragma comment(lib, "VirtDisk.lib") 

/**
 * @brief Microsoft's vendor ID for virtual storage
 * 
 * This GUID identifies Microsoft as the vendor for virtual storage devices.
 * It is used when creating and managing VHDX files.
 */
EXTERN_C const GUID DECLSPEC_SELECTANY VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT =
{ 0xec984aec, 0xa0f9, 0x47e9, { 0x90, 0x1f, 0x71, 0x41, 0x5a, 0x66, 0x34, 0x5b } };

/**
 * @brief Creates a new VHDX virtual disk
 * 
 * This function creates a new VHDX virtual disk file with the specified size
 * and sector size. The disk is created as a fixed-size disk with the default
 * block size. The size is adjusted to be a multiple of both sector size and
 * block size (64KB).
 * 
 * @param path Path where the VHDX file should be created
 * @param size Total size of the virtual disk in bytes
 * @param sectorSize Size of each sector in bytes
 */
void CreateVDisk(std::wstring path, unsigned long long size, unsigned long sectorSize)
{
    std::cout << "Sector size: " << sectorSize << std::endl;
    if (size % sectorSize != 0) {
        size = ((size / sectorSize) + 1) * sectorSize;
    }

    const int blockSize = 65536;
    if (size % blockSize != 0) {
        size = ((size / blockSize) + 2) * blockSize;
    }

    CREATE_VIRTUAL_DISK_PARAMETERS createParams;
    ZeroMemory(&createParams, sizeof(createParams));
    createParams.Version = CREATE_VIRTUAL_DISK_VERSION_2;
    createParams.Version2.MaximumSize = size;
    createParams.Version2.BlockSizeInBytes = CREATE_VIRTUAL_DISK_PARAMETERS_DEFAULT_BLOCK_SIZE;
    createParams.Version2.SectorSizeInBytes = sectorSize;
    createParams.Version2.PhysicalSectorSizeInBytes = sectorSize;

    VIRTUAL_STORAGE_TYPE storageType = {};
    storageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_VHDX;
    storageType.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT;

    HANDLE vhdxHandle = NULL;

    auto res = CreateVirtualDisk(
        &storageType,
        path.data(),
        VIRTUAL_DISK_ACCESS_NONE,
        NULL,
        CREATE_VIRTUAL_DISK_FLAG_NONE,
        0,
        &createParams,
        NULL,
        &vhdxHandle
    );
}

/**
 * @brief Mounts a VHDX virtual disk
 * 
 * This function mounts an existing VHDX virtual disk file and returns the path
 * to the mounted disk. The process involves:
 * 1. Opening the VHDX file
 * 2. Attaching it as a virtual disk
 * 3. Getting the physical path to the mounted disk
 * 
 * @param path Path to the VHDX file to mount
 * @param diskPath Output parameter that receives the path to the mounted disk
 */
void MountVDisk(std::wstring path, std::wstring& diskPath)
{
    HANDLE vhdxHandle;

    VIRTUAL_STORAGE_TYPE storageType = {};
    storageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_VHDX;
    storageType.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT;

    OPEN_VIRTUAL_DISK_PARAMETERS openParams;
    ZeroMemory(&openParams, sizeof(openParams));
    openParams.Version = OPEN_VIRTUAL_DISK_VERSION_2;
    openParams.Version2.GetInfoOnly = FALSE;

    auto openRes = OpenVirtualDisk(
        &storageType,
        path.data(),
        VIRTUAL_DISK_ACCESS_NONE,
        OPEN_VIRTUAL_DISK_FLAG_NO_PARENTS,
        &openParams,
        &vhdxHandle
    );

    std::cout << openRes << std::endl;
    if (vhdxHandle == INVALID_HANDLE_VALUE) {
        std::cout << "Invalid handle" << std::endl;
    }

    ATTACH_VIRTUAL_DISK_PARAMETERS attachParams;
    ZeroMemory(&attachParams, sizeof(attachParams));
    attachParams.Version = ATTACH_VIRTUAL_DISK_VERSION_1;

    auto attachRes = AttachVirtualDisk(
        vhdxHandle,
        NULL,
        ATTACH_VIRTUAL_DISK_FLAG_NONE,
        0,
        &attachParams,
        NULL
    );

    std::cout << attachRes << std::endl;
    
    diskPath.resize(MAX_PATH);
    ULONG diskPathSize = MAX_PATH;
    auto pathRes = GetVirtualDiskPhysicalPath(vhdxHandle, &diskPathSize, &diskPath[0]);
    std::wcout << L"Mount function path: " << diskPath << std::endl;
}

/**
 * @brief Updates the properties of a mounted disk
 * 
 * This function forces Windows to re-read the disk properties after changes
 * have been made. It is typically called after writing data to the disk to
 * ensure Windows recognizes the changes. The function:
 * 1. Opens the disk with read/write access
 * 2. Sends an IOCTL to update disk properties
 * 3. Handles any errors that occur during the process
 * 
 * @param diskPath Path to the mounted disk
 * @throws std::runtime_error if the update operation fails
 */
void UpdateDiskProperties(std::wstring diskPath)
{
    // Initialize a variable to receive the number of bytes returned by DeviceIoControl.
    DWORD bytesWritten = 0;

    // Open the target disk for read/write access.
    HANDLE targetDiskHandle = CreateFileW(diskPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    
    // Send the IOCTL_DISK_UPDATE_PROPERTIES control code to the disk device driver.
    // This control code does not require any input data or output data.
    if (!DeviceIoControl(targetDiskHandle, IOCTL_DISK_UPDATE_PROPERTIES, NULL, 0, NULL, 0, &bytesWritten, NULL))
    {
        // If DeviceIoControl failed
        CloseHandle(targetDiskHandle);
        std::cout << "Failed to update disk properties" << std::endl;
        throw std::runtime_error("Failed to update disk properties.");
    }

    std::cout << bytesWritten << std::endl;

    CloseHandle(targetDiskHandle);
}
