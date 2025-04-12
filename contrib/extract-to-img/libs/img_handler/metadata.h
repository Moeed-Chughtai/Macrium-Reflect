/**
 * @file metadata.h
 * @brief Macrium Reflect backup file metadata structures
 * 
 * This file defines the structures and constants used to parse and handle
 * Macrium Reflect backup file metadata, including block headers and magic
 * numbers.
 */

#include <cstdint>

/**
 * @brief Magic bytes identifying a Macrium Reflect backup file
 */
const char MAGIC_BYTES_VX[] = {"MACRIUM_FILE"};

/**
 * @brief Size of the magic bytes array (excluding null terminator)
 */
const unsigned long MAGIC_BYTES_VX_SIZE = (unsigned long)(sizeof(MAGIC_BYTES_VX) - 1);

/**
 * @brief Flags indicating metadata block properties
 */
struct MetadataHeaderFlags
{
    unsigned char LastBlock : 1;     ///< Indicates if this is the last block
    unsigned char Compression : 1;   ///< Indicates if the block is compressed
    unsigned char Encryption : 1;    ///< Indicates if the block is encrypted
    unsigned char Unused : 5;        ///< Reserved for future use
};

/**
 * @brief Header structure for metadata blocks
 * 
 * Each metadata block in a Macrium Reflect backup file begins with this
 * header structure, which describes the block's type, size, and properties.
 */
struct MetadataBlockHeader
{
    char BlockName[8] = {0};         ///< 8-byte block identifier
    uint32_t BlockLength = 0;        ///< Length of the block data
    unsigned char Hash[16] = {0};    ///< Hash of the block data
    MetadataHeaderFlags Flags;       ///< Block property flags
    char Padding[3] = {0};           ///< Padding to align structure
};

/**
 * @brief Block type identifiers
 * 
 * These constants identify different types of metadata blocks in the
 * backup file. Each corresponds to a specific type of data or metadata.
 */
const char* const JSON_HEADER   = "$JSON   ";  ///< File header JSON data
const char* const BITMAP_HEADER = "$BITMAP ";  ///< Partition data
const char* const FAT_HEADER    = "$FAT    ";  ///< FAT32 FAT data
const char* const CBT_HEADER    = "$CBT    ";  ///< Changed Block Tracking data
const char* const MFT_HEADER    = "$MFT    ";  ///< Master File Table data
const char* const TRACK_0       = "$TRACK0 ";  ///< 1st 1MB of data on the disk
const char* const IDX_HEADER    = "$INDEX  ";  ///< Reserved sectors and data block index
const char* const EXT_PAR_TABLE = "$EPT    ";  ///< Extended Partition Table data

/**
 * @brief Length of block name strings
 */
#define BLOCK_NAME_LENGTH 8
