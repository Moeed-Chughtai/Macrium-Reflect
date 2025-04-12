/**
 * @file file_handler.h
 * @brief Basic file I/O operations for Macrium Reflect backup files
 * 
 * This file declares low-level file operations used throughout the codebase
 * for reading, writing, and managing backup files. These functions provide
 * error handling and consistent file access patterns.
 */

#include <fstream>

/**
 * @brief Reads data from a file into a buffer
 * 
 * @param file Reference to the open file stream
 * @param buffer Pointer to the buffer to store read data
 * @param bytesToRead Number of bytes to read from the file
 * @throws std::runtime_error if read operation fails
 */
void readFile(std::fstream& file, void* buffer, std::streamsize bytesToRead);

/**
 * @brief Sets the file pointer to a specific position
 * 
 * @param file Reference to the open file stream
 * @param offset Number of bytes to offset from the base position
 * @param base Base position to offset from (std::ios::beg, std::ios::cur, std::ios::end)
 * @throws std::runtime_error if seek operation fails
 */
void setFilePointer(std::fstream& file, std::streamoff offset, std::ios_base::seekdir base);

/**
 * @brief Opens a file for binary read/write access
 * 
 * @param fileName Path to the file to open
 * @return std::fstream Open file stream
 * @throws std::runtime_error if file cannot be opened
 */
std::fstream openFile(std::string fileName);

/**
 * @brief Closes an open file
 * 
 * @param file Reference to the file stream to close
 * @throws std::runtime_error if file cannot be closed
 */
void closeFile(std::fstream& file);

/**
 * @brief Writes data from a buffer to a file
 * 
 * @param file Reference to the open file stream
 * @param buffer Pointer to the data to write
 * @param bytesToWrite Number of bytes to write to the file
 * @throws std::runtime_error if write operation fails
 */
void writeToFile(std::fstream& file, void* buffer, std::streamsize bytesToWrite);
