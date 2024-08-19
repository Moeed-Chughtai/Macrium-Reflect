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
void restoreDisk(std::string backupFilePath, std::string vhdxPath, file_structs::File_Layout& backupFileLayout){
    std::cout << "Restoring disk" << std::endl;
    std::fstream backupFile = openFile(backupFilePath);
    std::cout << "Opened backup file" << std::endl;
    std::fstream vhdxFile = openFile(vhdxPath);
    std::cout << "Opened vhdx file" << std::endl;

    file_structs::Disk::Disk_Layout disk = backupFileLayout.disks[0];

    writeToFile(vhdxFile, disk.track0.data(), disk.track0.size());

    for (auto& partition : backupFileLayout.disks[0].partitions)
    {
        setFilePointer(vhdxFile, partition._geometry.start + partition._geometry.boot_sector_offset, std::ios::beg);

        // FAT(32 stuff), do later

        int blockIndex = 0;
        for (auto& block : partition.data_blocks)
        {
            auto blockData = readDataBlock(backupFile, backupFileLayout, block);
            std::streamoff offset = partition._file_system.lcn0_offset + (partition._header.block_size * blockIndex);
            setFilePointer(vhdxFile, offset, std::ios::beg);
            writeToFile(vhdxFile, blockData.get(), block.block_length);
            blockIndex++;
        }
    }
    std::cout << "Restored all blocks" << std::endl;
}