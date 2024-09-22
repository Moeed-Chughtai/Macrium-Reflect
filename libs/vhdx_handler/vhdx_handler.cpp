#include <iostream>
#include <filesystem>
#include "vhdx_handler.h"
#include <winioctl.h>
#include <memory>
#include <objbase.h>
#include <virtdisk.h>

// https://learn.microsoft.com/en-us/windows/win32/api/virtdisk/nf-virtdisk-createvirtualdisk

#pragma comment(lib, "VirtDisk.lib")    //Link to library i think?

EXTERN_C const GUID DECLSPEC_SELECTANY VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT =
{ 0xec984aec, 0xa0f9, 0x47e9, { 0x90, 0x1f, 0x71, 0x41, 0x5a, 0x66, 0x34, 0x5b } };

void CreateVDisk(std::wstring path, unsigned long long size, unsigned long sectorSize)
{
    if (size % sectorSize != 0) {
        size = ((size / sectorSize) + 1) * sectorSize;
    }

    // Needed to fix restore for incrementals, for some reason the disk size given in the JSON of the incremental backup is too small
    // size = 536870912;

    std::cout << "Disk size: " << size << std::endl;

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
        // If DeviceIoControl fails, close the handle to the target disk and throw a runtime_error.
        CloseHandle(targetDiskHandle);
        std::cout << "Failed to update disk properties" << std::endl;
        throw std::runtime_error("Failed to update disk properties.");
    }

    std::cout << bytesWritten << std::endl;

    // Close the handle to the target disk.
    CloseHandle(targetDiskHandle);
}