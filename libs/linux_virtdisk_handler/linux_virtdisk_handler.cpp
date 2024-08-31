#include "linux_virtdisk_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

void RunCommandWithOutput(std::string command, std::string& output)
{
    FILE *fp = popen(command.c_str(), "r");
    char c = fgetc(fp);
    do {
        output += c;
        c = fgetc(fp);
    }
    while (c != '\n');
    pclose(fp);
}

void CreateVDisk(std::string path, unsigned long long size, unsigned long sectorSize)
{
    uint32_t count = size / sectorSize;
    std::cout << path<< std::endl;
    system(("sudo dd if=/dev/zero of=" + path + " bs=" + std::to_string(sectorSize) + " count=" + std::to_string(count)).c_str());
    

}

void MountVDisk(std::string path, std::string& diskPath)
{

    system(("sudo mkfs -t ext4 " + path).c_str());    
    RunCommandWithOutput("sudo losetup -f", diskPath);

    system(("sudo losetup " + diskPath + " " + path).c_str());
    system(("sudo chmod 777 " + diskPath).c_str());
}

void UpdateDiskProperties(std::wstring diskPath)
{
    
}