#include <iostream>
#include <fstream>
#include "metadata.h"
#include "file_struct.h"
#include <memory>

void readFile(std::fstream& file, void* buffer, std::streamsize bytesToRead)
{
    file.read(static_cast<char*>(buffer), bytesToRead);
    if (file.fail())
    {
        if (file.eof()) {std::cout << "End of file reached" << std::endl;}
        throw std::runtime_error("Failed to read file.");
    }
}

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

void setFilePointer(std::fstream& file, std::streamoff offset, std::ios_base::seekdir base)
{
    file.seekg(offset, base);
    if (file.fail())
    {
        throw std::runtime_error("Failed to set file pointer.");
    }
}

std::fstream openFile(std::string fileName)
{
    std::fstream file = std::fstream();
    file.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
    return file;
}

std::unique_ptr<unsigned char[]> readMetadataBlock(std::fstream& file, MetadataBlockHeader& header)
{

    std::unique_ptr<unsigned char[]> blockData = std::make_unique<unsigned char[]>(header.BlockLength);

    readFile(file, blockData.get(), header.BlockLength);
    return blockData;
}

void skipBitmap(std::fstream& file)
{
    MetadataBlockHeader header;
    
    do {
        readFile(file, &header, sizeof(header));
        setFilePointer(file, header.BlockLength, std::ios::cur);
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
void readDiskMetadata(std::fstream& file, file_structs::File_Layout fileLayout, file_structs::Disk::Disk_Layout disk)
{
    MetadataBlockHeader header;
    readFile(file, &header, sizeof(header));

    if (memcmp(header.BlockName, TRACK_0, BLOCK_NAME_LENGTH) != 0) {
        throw std::runtime_error("Missing track0 data");
    }

    auto blockData = readMetadataBlock(file, header);
    disk.track0.assign(blockData.get(), blockData.get() + header.BlockLength);
}

void readDataBlockIndex(std::fstream& file, file_structs::File_Layout fileLayout)
{
    setFilePointer(file, fileLayout._header.index_file_position, std::ios::beg);
    MetadataBlockHeader header;

    for (auto& disk : fileLayout.disks) {
        readDiskMetadata(file, fileLayout, disk);

        for (auto& partition : disk.partitions) {
            skipBitmap(file);
            
            int32_t blockCount;
            setFilePointer(file, sizeof(blockCount), std::ios::cur); // Skipping blockcount for reserved sectors (FAT32 only)
            readFile(file, &blockCount, sizeof(blockCount));
            std::cout << blockCount << std::endl;
            std::cout << sizeof(data_block) << std::endl;
            partition.data_blocks.resize(blockCount);
            readFile(file, partition.data_blocks.data(), blockCount * sizeof(data_block));

            std::cout << partition.data_blocks[0].file_position << std::endl;
        }
    }
}

void readBackupFile(std::string backupFileName)
{
    std::fstream file = openFile(backupFileName);
    setFilePointer(file, calculateFooterOffset(), std::ios_base::end);

    uint64_t headerOffset;
    uint8_t magicBytes[MAGIC_BYTES_VX_SIZE];

    readFooterData(headerOffset, magicBytes, file);
    setFilePointer(file, headerOffset, std::ios::beg);

    std::string strJson = getJSON(file);

    nlohmann::json json = nlohmann::json::parse(strJson);

    // std::cout << strJson << std::endl;

    // std::cout << json["/disks/0/_geometry/bytes_per_sector"_json_pointer] << std::endl;

    file_structs::File_Layout layout = json;
    // std::cout << layout.disks[0]._geometry.bytes_per_sector << std::endl;

    readDataBlockIndex(file, layout);
}

int main(int argc, char* argv[])
{
    std::string backupFileName = "4CB8A10DEAE082C4-testbackup-00-00.mrimg";
    readBackupFile(backupFileName);
}
