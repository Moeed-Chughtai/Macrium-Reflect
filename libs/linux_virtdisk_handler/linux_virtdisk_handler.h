#include <string>

void CreateVDisk(std::string path, unsigned long long size, unsigned long sectorSize);

void MountVDisk(std::string path, std::string& diskPath);

void UpdateDiskProperties(std::wstring diskPath);