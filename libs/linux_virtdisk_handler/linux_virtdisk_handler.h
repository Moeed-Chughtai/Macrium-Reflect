#include <string>

void CreateVDisk(std::wstring path, unsigned long long size, unsigned long sectorSize);

void MountVDisk(std::wstring path, std::wstring& diskPath);

void UpdateDiskProperties(std::wstring diskPath);