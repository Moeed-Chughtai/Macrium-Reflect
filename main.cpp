#include <iostream>
#include <fstream>
#include "metadata.h"
#include "file_struct.h"
#include <memory>

void readFile(std::fstream &file, void *buffer, std::streamsize bytesToRead)
{
    file.read(static_cast<char *>(buffer), bytesToRead);
    if (file.fail())
    {
        std::cout << file.eofbit << std::endl;
        throw std::runtime_error("Failed to read file.");
    }
}

void readFooterData(uint64_t &headerOffset, uint8_t magicBytes[], std::fstream &file)
{
    readFile(file, &headerOffset, sizeof(headerOffset));
    std::cout << sizeof(headerOffset) << std::endl;
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

void setFilePointer(std::fstream &file, std::streamoff offset, std::ios_base::seekdir base)
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

std::unique_ptr<unsigned char[]> readMetadataBlock(std::fstream &file, MetadataBlockHeader &header)
{

    std::unique_ptr<unsigned char[]> blockData = std::make_unique<unsigned char[]>(header.BlockLength);

    readFile(file, blockData.get(), header.BlockLength);
    return blockData;
}

std::string getJSON(std::fstream &file)
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

            strJson.assign(reinterpret_cast<const char *>(blockData.get()), header.BlockLength);
        }
        else
        {
            setFilePointer(file, header.BlockLength + sizeof(header), std::ios::cur);
        }
    }

    while (header.Flags.LastBlock == 0);

    return strJson;
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
}

int main(int argc, char *argv[])
{
    std::string backupFileName = "4CB8A10DEAE082C4-testbackup-00-00.mrimg";
    readBackupFile(backupFileName);
}
