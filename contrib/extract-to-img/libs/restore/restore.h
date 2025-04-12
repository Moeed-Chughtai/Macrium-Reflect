/**
 * @file restore.h
 * @brief Header for Macrium Reflect backup restoration functionality
 * 
 * This file declares functions for restoring data from Macrium Reflect backup files
 * to disk images or virtual disks. It handles the extraction of data blocks and
 * writing them to the target storage.
 */

#pragma once

#include <fstream>
#include "file_struct.h"

/**
 * @brief Restores a disk from a Macrium Reflect backup file
 * 
 * This function reads the backup file and writes its contents to the target disk,
 * handling both reserved sectors and data blocks. It supports restoring both
 * full and incremental backups.
 * 
 * @param backupFilePath Path to the Macrium Reflect backup file
 * @param vhdxPath Path to the target disk image or virtual disk
 * @param backupFileLayout Structure containing the backup file layout
 * @param diskIndex Index of the disk to restore in the backup
 */
void restoreDisk(std::string backupFilePath, std::string vhdxPath, file_structs::File_Layout& backupFileLayout, int diskIndex);
