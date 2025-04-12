/**
 * @file backup_set.cpp
 * @brief Implementation of Macrium Reflect backup set management
 * 
 * This file implements the functionality for managing and processing Macrium Reflect
 * backup sets, including handling incremental backups and their associated data blocks.
 */

#include <iostream>
#include <algorithm>

#include "../file_handler/file_handler.h"
#include "backup_set.h"

/**
 * @brief Comparison function for sorting file history by file number in descending order
 * 
 * @param fhistory1 First file history entry
 * @param fhistory2 Second file history entry
 * @return true if fhistory1's file number is greater than fhistory2's
 */
bool SortByDescFileNumber(file_structs::Partition::File_History& fhistory1, file_structs::Partition::File_History& fhistory2)
{
    return fhistory1.file_number > fhistory2.file_number;
}

/**
 * @brief Finds and stores the layouts and paths of backup files up to the latest full backup
 * 
 * This function processes the file history in the JSON to find all relevant backup files,
 * starting from the most recent and working back to the full backup. It only works when
 * the file path of previous backup files has changed.
 * 
 * @param backupSet The backup set to populate with file information
 * @param partitionLayout The partition layout containing file history
 * @param diskIndex The index of the disk to process
 */
void FindBackupFiles(PartitionBackupSet& backupSet, file_structs::Partition::Partition_Layout& partitionLayout, int diskIndex)
{
    // Sort the files in descending order so we go from most recent backup to oldest
    std::sort(partitionLayout._header.file_history.begin(), partitionLayout._header.file_history.end(), SortByDescFileNumber);
    
    for (auto& fileHistory : partitionLayout._header.file_history) {
        backupSet.filePaths.insert(backupSet.filePaths.begin(), fileHistory.file_name);
        file_structs::File_Layout fileLayout;
        readBackupFileLayout(fileLayout, fileHistory.file_name);

        for (auto& partition : fileLayout.disks[diskIndex].partitions) {
            if (partition._header.partition_number == partitionLayout._header.partition_number) {
                backupSet.partitionLayouts.insert(backupSet.partitionLayouts.begin(), std::make_unique<file_structs::Partition::Partition_Layout>(partition));
                break;
            }
        }

        if (fileLayout._header.delta_index == 0) { return; } // Stop at the full backup
    }
}

/**
 * @brief Creates the initial block-to-file mapping for a backup set
 * 
 * This function opens the first backup file and creates the initial mapping
 * of data blocks to their corresponding backup file.
 * 
 * @param backupSet The backup set to populate with block mappings
 */
void FillInitialBlockFileMap(PartitionBackupSet& backupSet)
{
    BackupFilePtr filePtr = std::make_shared<std::fstream>(openFile(backupSet.filePaths[0]));
    backupSet.backupFilePtrs.push_back(filePtr);
    for (int i = 0; i < backupSet.partitionLayouts[0]->data_block_index.size(); i++) {
        BackupSetBlockIndexElement blockIndexElement;
        blockIndexElement.block = backupSet.partitionLayouts[0]->data_block_index[i];
        blockIndexElement.file = filePtr;
        backupSet.backupSetBlockIndex.push_back(blockIndexElement);
    }
}

/**
 * @brief Adds delta backup information to the block-to-file mapping
 * 
 * This function processes incremental backup files and updates the block mapping
 * to reflect changes in the delta backups.
 * 
 * @param backupSet The backup set containing the block mappings to update
 */
void AddDeltaToBlockFileMap(PartitionBackupSet& backupSet)
{
    for (int i = 1; i < backupSet.partitionLayouts.size(); i++) {
        BackupFilePtr filePtr = std::make_shared<std::fstream>(openFile(backupSet.filePaths[i]));
        backupSet.backupFilePtrs.push_back(filePtr);
        for (auto& deltaBlock : backupSet.partitionLayouts[i]->delta_data_block_index) {
            backupSet.backupSetBlockIndex[deltaBlock.block_index].file = filePtr;
            backupSet.backupSetBlockIndex[deltaBlock.block_index].block = deltaBlock.data_block;
        }
    }
}

/**
 * @brief Closes all open backup files in a backup set
 * 
 * @param backupSet The backup set containing the files to close
 */
void CloseBackupFiles(PartitionBackupSet& backupSet)
{
    for(auto& backupFilePtr : backupSet.backupFilePtrs) {
        closeFile(*backupFilePtr);
    }
}

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
void BuildPartitionBackupSet(PartitionBackupSet& backupSet, file_structs::Partition::Partition_Layout& partitionLayout, int diskIndex)
{
    FindBackupFiles(backupSet, partitionLayout, diskIndex);
    FillInitialBlockFileMap(backupSet);
    AddDeltaToBlockFileMap(backupSet);
}
