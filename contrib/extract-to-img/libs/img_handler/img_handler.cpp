/**
 * @file img_handler.cpp
 * @brief Implementation of Macrium Reflect backup file handling
 * 
 * This file contains the implementation of functions for reading and parsing
 * Macrium Reflect backup files (.mrimgx and .mrbakx). It handles the file
 * structure, metadata, and data block index reading.
 */

#include <fstream>
#include <iostream>
#include "file_struct.h"
#include "metadata.h"
#include "../file_handler/file_handler.h"

/**
 * @brief Reads the footer data from a backup file
 * 
 * The footer contains the header offset and magic bytes that identify
 * the file as a Macrium Reflect backup.
 * 
 * @param headerOffset Output parameter for the header offset
 * @param magicBytes Output parameter for the magic bytes
 * @param file Input file stream
 */
void readFooterData(uint64_t& headerOffset, uint8_t magicBytes[], std::fstream& file)
{
    readFile(file, &headerOffset, sizeof(headerOffset));
    readFile(file, magicBytes, MAGIC_BYTES_VX_SIZE);
}

/**
 * @brief Calculates the offset to the footer from the end of the file
 * 
 * @return std::streamoff Offset to the footer
 */
std::streamoff calculateFooterOffset()
{
    constexpr int64_t magicBytesSize = static_cast<signed int>(MAGIC_BYTES_VX_SIZE);
    constexpr int64_t int64_tSize = static_cast<signed int>(sizeof(int64_t));
    constexpr int64_t offset = -((int64_t)magicBytesSize + int64_tSize);
    std::streamoff fileOffset = offset;
    return fileOffset;
}

/**
 * @brief Reads a metadata block from the file
 * 
 * @param file Input file stream
 * @param header Metadata block header
 * @return std::unique_ptr<unsigned char[]> Pointer to the block data
 */
std::unique_ptr<unsigned char[]> readMetadataBlock(std::fstream& file, MetadataBlockHeader& header)
{
    std::unique_ptr<unsigned char[]> blockData = std::make_unique<unsigned char[]>(header.BlockLength);
    readFile(file, blockData.get(), header.BlockLength);
    return blockData;
}

/**
 * @brief Skips over partition metadata blocks
 * 
 * This function skips the bitmap block and index header metadata,
 * leaving the file pointer at the start of the data block index.
 * 
 * @param file Input file stream
 */
void skipPartitionMetadata(std::fstream& file)
{
    MetadataBlockHeader header;
    
    do {
        readFile(file, &header, sizeof(header));
        if (memcmp(header.BlockName, BITMAP_HEADER, BLOCK_NAME_LENGTH) == 0) 
        {
            setFilePointer(file, header.BlockLength, std::ios::cur);
        }
    } 
    while (header.Flags.LastBlock == 0);
}

/**
 * @brief Reads the JSON metadata from the file
 * 
 * The JSON metadata contains information about the backup, including
 * disk layouts, partitions, and file system information.
 * 
 * @param file Input file stream
 * @return std::string JSON string containing the metadata
 */
std::string readJSON(std::fstream& file)
{
    MetadataBlockHeader header;
    std::string strJson;

    do
    {
        readFile(file, &header, sizeof(header));

        if (memcmp(header.BlockName, JSON_HEADER, BLOCK_NAME_LENGTH) == 0)
        {
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

/**
 * @brief Reads disk metadata from the file
 * 
 * This function reads the track 0 data and other disk-specific metadata.
 * 
 * @param file Input file stream
 * @param fileLayout Output parameter for the file layout
 * @param disk Output parameter for the disk layout
 */
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

/**
 * @brief Reads the data block index from the file
 * 
 * This function reads the index of data blocks for each partition,
 * including reserved sectors and delta blocks for incremental backups.
 * 
 * @param file Input file stream
 * @param fileLayout Output parameter for the file layout
 */
void readDataBlockIndex(std::fstream& file, file_structs::File_Layout& fileLayout)
{
    setFilePointer(file, fileLayout._header.index_file_position, std::ios::beg);
    MetadataBlockHeader header;

    for (auto& disk : fileLayout.disks) {
        readDiskMetadata(file, fileLayout, disk);

        for (auto& partition : disk.partitions) {
            skipPartitionMetadata(file);    // Skip over bitmap block and index header

            int32_t blockCount;
            readFile(file, &blockCount, sizeof(blockCount));

            // If FAT32, read reserved sectors
            if (blockCount != 0) {
                partition.reserved_sectors.resize(blockCount);
                readFile(file, partition.reserved_sectors.data(), blockCount * sizeof(DataBlockIndexElement));
            }

            readFile(file, &blockCount, sizeof(blockCount));

            if (fileLayout._header.delta_index) {
                partition.delta_data_block_index.resize(blockCount);
                readFile(file, partition.delta_data_block_index.data(), blockCount * sizeof(DeltaDataBlockIndexElement));
            }
            else {
                partition.data_block_index.resize(blockCount);
                readFile(file, partition.data_block_index.data(), blockCount * sizeof(DataBlockIndexElement));
            }
        }
    }
}

/**
 * @brief Reads the complete backup file layout
 * 
 * This is the main function that reads and parses a Macrium Reflect backup file.
 * It reads the footer, header, JSON metadata, and data block index to construct
 * a complete representation of the backup file.
 * 
 * @param layout Output parameter for the file layout
 * @param backupFileName Path to the backup file
 */
void readBackupFileLayout(file_structs::File_Layout& layout, std::string backupFileName)
{
    std::fstream file = openFile(backupFileName);

    setFilePointer(file, calculateFooterOffset(), std::ios_base::end);

    uint64_t headerOffset;
    uint8_t magicBytes[MAGIC_BYTES_VX_SIZE];

    readFooterData(headerOffset, magicBytes, file);
    setFilePointer(file, headerOffset, std::ios::beg);

    std::string strJson = readJSON(file);

    nlohmann::json json = nlohmann::json::parse(strJson);
    layout = json;

    readDataBlockIndex(file, layout);
    closeFile(file);
}
