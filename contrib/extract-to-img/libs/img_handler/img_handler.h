/**
 * @file img_handler.h
 * @brief Macrium Reflect backup file layout handling
 * 
 * This file declares functions for reading and parsing Macrium Reflect
 * backup file layouts, which contain metadata about the backup structure
 * and organization.
 */

#include "file_struct.h"
#include "../file_handler/file_handler.h"

/**
 * @brief Reads and parses the layout information from a backup file
 * 
 * This function reads the backup file's layout information, which includes
 * metadata about the backup structure, partitions, and data organization.
 * 
 * @param layout Reference to the File_Layout structure to populate
 * @param backupFileName Path to the backup file to read
 */
void readBackupFileLayout(file_structs::File_Layout& layout, std::string backupFileName);
