#include <fstream>
#include <iostream>
#include "file_struct.h"
#include "metadata.h"
#include "../file_handler/file_handler.h"

void readFooterData(uint64_t& headerOffset, uint8_t magicBytes[], std::fstream& file)
{
    readFile(file, &headerOffset, sizeof(headerOffset));
    readFile(file, magicBytes, MAGIC_BYTES_VX_SIZE);
}

std::streamoff calculateFooterOffset()
{
    constexpr int64_t magicBytesSize = static_cast<signed int>(MAGIC_BYTES_VX_SIZE);
    constexpr int64_t int64_tSize = static_cast<signed int>(sizeof(int64_t));
    constexpr int64_t offset = -((int64_t)magicBytesSize + int64_tSize);
    std::streamoff fileOffset = offset;
    return fileOffset;
}

std::unique_ptr<unsigned char[]> readMetadataBlock(std::fstream& file, MetadataBlockHeader& header)
{

    std::unique_ptr<unsigned char[]> blockData = std::make_unique<unsigned char[]>(header.BlockLength);

    readFile(file, blockData.get(), header.BlockLength);
    return blockData;
}

// Skips metadata block containing the bitmap and the header of the index metadata block
// File pointer (should be) left at the start of the data block index
void skipPartitionMetadata(std::fstream& file)
{
    std::cout << "Skipping partition metadata" << std::endl;
    MetadataBlockHeader header;
    
    do {
        readFile(file, &header, sizeof(header));

        if (memcmp(header.BlockName, BITMAP_HEADER, BLOCK_NAME_LENGTH) == 0) 
        {
            setFilePointer(file, header.BlockLength, std::ios::cur);
        }
        else if (memcmp(header.BlockName, IDX_HEADER, BLOCK_NAME_LENGTH) == 0)
        {
            std::cout << "Index header found" << std::endl;
        }
    } 
    while (header.Flags.LastBlock == 0);
}

std::string getJSON(std::fstream& file)
{
    MetadataBlockHeader header;
    std::string strJson;

    do
    {
        readFile(file, &header, sizeof(header));

        if (memcmp(header.BlockName, JSON_HEADER, BLOCK_NAME_LENGTH) == 0)
        {
            std::cout << "Found JSON" << std::endl;
            auto blockData = readMetadataBlock(file, header);
            strJson.assign(reinterpret_cast<const char*>(blockData.get()), header.BlockLength);
        }
        else
        {
            setFilePointer(file, header.BlockLength + sizeof(header), std::ios::cur);
        }
    }

    while (header.Flags.LastBlock == 0);
    return strJson;
}

// For now ignoring extended partitions
void readDiskMetadata(std::fstream& file, file_structs::File_Layout& fileLayout, file_structs::Disk::Disk_Layout& disk)
{
    MetadataBlockHeader header;
    readFile(file, &header, sizeof(header));

    if (memcmp(header.BlockName, TRACK_0, BLOCK_NAME_LENGTH) != 0) {
        throw std::runtime_error("Missing track0 data");
    }

    auto blockData = readMetadataBlock(file, header);
    disk.track0.assign(blockData.get(), blockData.get() + header.BlockLength);
    
}

void readDataBlockIndex(std::fstream& file, file_structs::File_Layout& fileLayout)
{

    setFilePointer(file, fileLayout._header.index_file_position, std::ios::beg);
    MetadataBlockHeader header;

    for (auto& disk : fileLayout.disks) {
        readDiskMetadata(file, fileLayout, disk);

        for (auto& partition : disk.partitions) {
            skipPartitionMetadata(file);    //skip over bitmap block and index header

            int32_t blockCount;
            setFilePointer(file, sizeof(blockCount), std::ios::cur); // Skipping blockcount for reserved sectors (FAT32 only), will be 0 for non-fat32 anyways
            readFile(file, &blockCount, sizeof(blockCount));
            partition.data_blocks.resize(blockCount);
            readFile(file, partition.data_blocks.data(), blockCount * sizeof(data_block));

            std::cout << "Read data block index" << std::endl;
        }
    }
}

void readBackupFileLayout(file_structs::File_Layout& layout, std::string backupFileName)
{
    std::fstream file = openFile(backupFileName);

    //Set pointer to the start of the footer
    setFilePointer(file, calculateFooterOffset(), std::ios_base::end);

    uint64_t headerOffset;
    uint8_t magicBytes[MAGIC_BYTES_VX_SIZE];

    readFooterData(headerOffset, magicBytes, file);
    setFilePointer(file, headerOffset, std::ios::beg);

    std::string strJson = getJSON(file);

    nlohmann::json json = nlohmann::json::parse(strJson);
    layout = json;

    readDataBlockIndex(file, layout);
    file.close();
}