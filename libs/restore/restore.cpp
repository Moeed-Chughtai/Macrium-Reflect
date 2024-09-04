#include "..//img_handler//file_struct.h"
#include "..//file_handler//file_handler.h"
#include <fstream>
#include <iostream>

std::unique_ptr<unsigned char[]> readDataBlock(std::fstream& backupFile, file_structs::File_Layout backupFileLayout, data_block& block)
{
    std::unique_ptr<unsigned char[]> readBuffer = std::make_unique<unsigned char[]>(block.block_length);
    setFilePointer(backupFile, block.file_position, std::ios::beg);
    readFile(backupFile, readBuffer.get(), block.block_length);
    return readBuffer;
}

// Assumes single disk in file (will need to be changed later)
void restoreDisk(std::string backupFilePath, std::string targetDiskPath, file_structs::File_Layout& backupFileLayout){
    std::fstream backupFile = openFile(backupFilePath);
    std::fstream diskFile = openFile(targetDiskPath);

    file_structs::Disk::Disk_Layout disk = backupFileLayout.disks[0];


    writeToFile(diskFile, disk.track0.data(), (uint32_t)disk.track0.size());
  
    for (auto& partition : backupFileLayout.disks[0].partitions)
    {
        setFilePointer(diskFile, partition._geometry.start + partition._geometry.boot_sector_offset, std::ios::beg);

        // FAT 32 stuff, do later

        int blockIndex = 0;
        std::cout << partition._file_system.lcn0_offset << std::endl;
        for (auto& block : partition.data_blocks)
        {
            auto blockData = readDataBlock(backupFile, backupFileLayout, block);
            if (blockData != nullptr && block.block_length != 0)
            {
                std::streamoff offset = partition._file_system.lcn0_offset + (static_cast<uint64_t>(partition._header.block_size) * blockIndex);
                setFilePointer(diskFile, offset, std::ios::beg);
                writeToFile(diskFile, blockData.get(), block.block_length);
            }
            blockIndex++;
            
        }
    }
    std::cout << "Restored all blocks" << std::endl;
    closeFile(backupFile);
    closeFile(diskFile);
}