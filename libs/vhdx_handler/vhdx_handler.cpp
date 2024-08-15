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
    CREATE_VIRTUAL_DISK_PARAMETERS create_params;
    ZeroMemory(&create_params, sizeof(create_params));
    create_params.Version = CREATE_VIRTUAL_DISK_VERSION_2;
    create_params.Version2.MaximumSize = size;
    create_params.Version2.BlockSizeInBytes = 0;
    create_params.Version2.SectorSizeInBytes = sectorSize;
    create_params.Version2.PhysicalSectorSizeInBytes = sectorSize;

    VIRTUAL_STORAGE_TYPE storage_type = {};
    storage_type.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_VHDX;
    storage_type.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT;

    HANDLE vhdxHandle = NULL;

    auto res = CreateVirtualDisk(
        &storage_type,
        path.data(),
        VIRTUAL_DISK_ACCESS_NONE,
        NULL,
        CREATE_VIRTUAL_DISK_FLAG_NONE,
        0,
        &create_params,
        NULL,
        &vhdxHandle
    );
}

