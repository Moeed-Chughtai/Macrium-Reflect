#include <iostream>

#include <filesystem>
#include "vhdx_handler.h"
#include <virtdisk.h>

// https://learn.microsoft.com/en-us/windows/win32/api/virtdisk/nf-virtdisk-createvirtualdisk

#pragma comment(lib, "VirtDisk.lib")    //Link to library i think?

EXTERN_C const GUID DECLSPEC_SELECTANY VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT =
{ 0xec984aec, 0xa0f9, 0x47e9, { 0x90, 0x1f, 0x71, 0x41, 0x5a, 0x66, 0x34, 0x5b } };

void CreateVHDX(std::wstring path, ULONGLONG size, ULONG sectorSize)
{
    CREATE_VIRTUAL_DISK_PARAMETERS createParams;
    ZeroMemory(&createParams, sizeof(createParams));
    createParams.Version = CREATE_VIRTUAL_DISK_VERSION_2;
    createParams.Version2.MaximumSize = size;
    createParams.Version2.BlockSizeInBytes = 0;
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

void MountVHDX(std::wstring path)
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
    
    std::wstring diskPath;
    diskPath.resize(MAX_PATH);
    ULONG diskPathSize = MAX_PATH;
    auto pathRes = GetVirtualDiskPhysicalPath(vhdxHandle, &diskPathSize, &diskPath[0]);
    std::wcout << diskPath << std::endl;
    int x;
    std::cin >> x;
}

