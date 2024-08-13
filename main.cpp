// #include <iostream>
// #include <fstream>
// #include "metadata.h"
// #include "file_struct.h"
// #pragma pack(pop)
// #include <memory>
#include "vhdx_manager.h"

// void readFile(std::fstream& file, void* buffer, std::streamsize bytesToRead)
// {
//     file.read(static_cast<char*>(buffer), bytesToRead);
//     if (file.fail())
//     {
//         if (file.eof()) {std::cout << "End of file reached" << std::endl;}
//         throw std::runtime_error("Failed to read file.");
//     }
// }

// void readFooterData(uint64_t& headerOffset, uint8_t magicBytes[], std::fstream& file)
// {
//     readFile(file, &headerOffset, sizeof(headerOffset));
//     readFile(file, magicBytes, MAGIC_BYTES_VX_SIZE);
// }

// std::streamoff calculateFooterOffset()
// {
//     constexpr int64_t magicBytesSize = static_cast<signed int>(MAGIC_BYTES_VX_SIZE);
//     constexpr int64_t int64_tSize = static_cast<signed int>(sizeof(int64_t));
//     constexpr int64_t offset = -((int64_t)magicBytesSize + int64_tSize);
//     std::streamoff fileOffset = offset;
//     return fileOffset;
// }

// void setFilePointer(std::fstream& file, std::streamoff offset, std::ios_base::seekdir base)
// {
//     file.seekg(offset, base);
//     if (file.fail())
//     {
//         throw std::runtime_error("Failed to set file pointer.");
//     }
// }

// std::fstream openFile(std::string fileName)
// {
//     std::fstream file = std::fstream();
//     file.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
//     return file;
// }

// std::unique_ptr<unsigned char[]> readMetadataBlock(std::fstream& file, MetadataBlockHeader& header)
// {

//     std::unique_ptr<unsigned char[]> blockData = std::make_unique<unsigned char[]>(header.BlockLength);

//     readFile(file, blockData.get(), header.BlockLength);
//     return blockData;
// }

// Skips metadata block containing the bitmap and the header of the index metadata block
// File pointer (should be) left at the start of the data block index
// void skipPartitionMetadata(std::fstream& file)
// {
//     MetadataBlockHeader header;
    
//     do {
//         readFile(file, &header, sizeof(header));

//         if (memcmp(header.BlockName, BITMAP_HEADER, BLOCK_NAME_LENGTH) == 0) 
//         {
//             setFilePointer(file, header.BlockLength, std::ios::cur);
//         }
//         else if (memcmp(header.BlockName, IDX_HEADER, BLOCK_NAME_LENGTH) == 0)
//         {
//             std::cout << "Index header found" << std::endl;
//         }
//     } 
//     while (header.Flags.LastBlock == 0);
// }

// // Find and return as a string the JSON of the entire backup file
// std::string getJSON(std::fstream& file)
// {
//     MetadataBlockHeader header;
//     std::string strJson;

//     do
//     {
//         readFile(file, &header, sizeof(header));

//         if (memcmp(header.BlockName, JSON_HEADER, BLOCK_NAME_LENGTH) == 0)
//         {

//             std::cout << "Found JSON" << std::endl;

//             auto blockData = readMetadataBlock(file, header);

//             strJson.assign(reinterpret_cast<const char*>(blockData.get()), header.BlockLength);
//         }
//         else
//         {
//             setFilePointer(file, header.BlockLength + sizeof(header), std::ios::cur);
//         }
//     }

//     while (header.Flags.LastBlock == 0);

//     return strJson;
// }

// // For now ignoring extended partitions
// void readDiskMetadata(std::fstream& file, file_structs::File_Layout fileLayout, file_structs::Disk::Disk_Layout disk)
// {
//     MetadataBlockHeader header;
//     readFile(file, &header, sizeof(header));

//     if (memcmp(header.BlockName, TRACK_0, BLOCK_NAME_LENGTH) != 0) {
//         throw std::runtime_error("Missing track0 data");
//     }

//     auto blockData = readMetadataBlock(file, header);
//     disk.track0.assign(blockData.get(), blockData.get() + header.BlockLength);
// }

// void readDataBlockIndex(std::fstream& file, file_structs::File_Layout& fileLayout)
// {
//     setFilePointer(file, fileLayout._header.index_file_position, std::ios::beg);
//     MetadataBlockHeader header;

//     for (auto& disk : fileLayout.disks) {
//         readDiskMetadata(file, fileLayout, disk);

//         for (auto& partition : disk.partitions) {
//             skipPartitionMetadata(file);    //skip over bitmap block and index header

//             int32_t blockCount;
//             setFilePointer(file, sizeof(blockCount), std::ios::cur); // Skipping blockcount for reserved sectors (FAT32 only), will be 0 for non-fat32 anyways
//             readFile(file, &blockCount, sizeof(blockCount));
//             partition.data_blocks.resize(blockCount);
//             readFile(file, partition.data_blocks.data(), blockCount * sizeof(data_block));

//             std::cout << "Read data block index" << std::endl;
//             std::cout << partition.data_blocks[0].file_position << std::endl;
//         }
//     }
// }

// void readBackupFileLayout(file_structs::File_Layout& layout, std::string backupFileName)
// {
//     std::fstream file = openFile(backupFileName);
//     setFilePointer(file, calculateFooterOffset(), std::ios_base::end);

//     uint64_t headerOffset;
//     uint8_t magicBytes[MAGIC_BYTES_VX_SIZE];

//     readFooterData(headerOffset, magicBytes, file);
//     setFilePointer(file, headerOffset, std::ios::beg);

//     std::string strJson = getJSON(file);

//     nlohmann::json json = nlohmann::json::parse(strJson);
//     layout = json;

//     readDataBlockIndex(file, layout);
// }

// void handleBackup()
// {
//     std::string backupFileName = "515CE701D2BB4A22-TestMBR-SP-NC-NE-00-00.mrimg";
//     file_structs::File_Layout file_layout;
//     readBackupFileLayout(file_layout, backupFileName);
//     std::cout << file_layout.disks[0].partitions[0].data_blocks[0].file_position << std::endl;

//     CreateVHDX(L"C:\\Users\\adamp\\Documents\\Programming\\Macrium\\ReflectTest\\vhdxoutput\\test.vhdx", file_layout.disks[0]._geometry.disk_size, file_layout.disks[0]._geometry.bytes_per_sector);
//}

int main(int argc, char* argv[])
{
    //handleBackup();
    Apple();
}
