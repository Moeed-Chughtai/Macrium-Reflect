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
    std::cout << path << std::endl;
    system(("sudo dd if=/dev/zero of=" + path + " bs=" + std::to_string(sectorSize) + " count=" + std::to_string(count)).c_str());
    system(("sudo chmod 777 " + path).c_str());
}

void MountVDisk(std::string imgPath, std::string& loopFilePath)
{
    RunCommandWithOutput("sudo losetup -f", loopFilePath);

    system(("sudo losetup " + loopFilePath + " " + imgPath).c_str());
    
}

void UpdateDiskProperties(std::string imgPath)
{
    
}