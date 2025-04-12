/**
 * @file backup_set.h
 * @brief Defines structures and functions for managing Macrium Reflect backup sets
 * 
 * This file contains the core data structures and function declarations needed to
 * manage and process Macrium Reflect backup sets, including handling incremental
 * backups and their associated data blocks.
 */

#include <map>
#include <vector>
#include <fstream>
#include <string>

#include "../img_handler/img_handler.h"

/**
 * @brief Shared pointer to a backup file stream
 */
typedef std::shared_ptr<std::fstream> BackupFilePtr;

/**
 * @brief Unique pointer to a partition layout structure
 */
typedef std::unique_ptr<file_structs::Partition::Partition_Layout> PartitionLayoutPtr;

/**
 * @brief Type alias for block index
 */
typedef uint32_t BlockIndex;

/**
 * @brief Structure representing a block index element in a backup set
 * 
 * Maps a data block to its corresponding backup file
 */
struct BackupSetBlockIndexElement
{
    DataBlockIndexElement block;  // The data block index element
    BackupFilePtr file;           // Pointer to the backup file containing this block
};

/**
 * @brief Structure representing a complete partition backup set
 * 
 * Contains all information needed to restore a partition from a backup set,
 * including file paths, partition layouts, and block mappings.
 */
struct PartitionBackupSet
{
    std::vector<std::string> filePaths;                    // Paths to all backup files in the set
    std::vector<PartitionLayoutPtr> partitionLayouts;      // Layout information for each backup
    std::vector<BackupSetBlockIndexElement> backupSetBlockIndex;  // Mapping of blocks to files
    std::vector<BackupFilePtr> backupFilePtrs;            // Open file handles for backup files
};

/**
 * @brief Builds a complete partition backup set from a partition layout
 * 
 * This function constructs a PartitionBackupSet by:
 * 1. Finding all relevant backup files
 * 2. Creating the initial block-to-file mapping
 * 3. Adding delta (incremental) backup information
 * 
 * @param backupSet The backup set structure to populate
 * @param partitionLayout The partition layout to build the backup set for
 * @param diskIndex The index of the disk containing the partition
 */
void BuildPartitionBackupSet(PartitionBackupSet& backupSet, file_structs::Partition::Partition_Layout& partitionLayout, int diskIndex);

/**
 * @brief Closes all open backup files in a backup set
 * 
 * @param backupSet The backup set containing the files to close
 */
void CloseBackupFiles(PartitionBackupSet& backupSet);
