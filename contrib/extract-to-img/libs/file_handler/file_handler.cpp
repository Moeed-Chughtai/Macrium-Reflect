/**
 * @file file_handler.cpp
 * @brief Implementation of basic file I/O operations
 * 
 * This file implements low-level file operations with error handling
 * for Macrium Reflect backup files. It provides consistent file access
 * patterns and detailed error reporting.
 */

#include <fstream>
#include <iostream>

/**
 * @brief Reads data from a file into a buffer
 * 
 * This function reads the specified number of bytes from the file into
 * the provided buffer. It includes error handling for read failures and
 * end-of-file conditions.
 * 
 * @param file Reference to the open file stream
 * @param buffer Pointer to the buffer to store read data
 * @param bytesToRead Number of bytes to read from the file
 * @throws std::runtime_error if read operation fails
 */
void readFile(std::fstream& file, void* buffer, std::streamsize bytesToRead)
{
    file.read(static_cast<char*>(buffer), bytesToRead);
    if (file.fail())
    {
        if (file.eof()) {std::cout << "End of file reached" << std::endl;}
        throw std::runtime_error("Failed to read file.");
    }
}

/**
 * @brief Sets the file pointer to a specific position
 * 
 * This function moves the file pointer to the specified position relative
 * to the given base position. It includes error handling for seek failures.
 * 
 * @param file Reference to the open file stream
 * @param offset Number of bytes to offset from the base position
 * @param base Base position to offset from (std::ios::beg, std::ios::cur, std::ios::end)
 * @throws std::runtime_error if seek operation fails
 */
void setFilePointer(std::fstream& file, std::streamoff offset, std::ios_base::seekdir base)
{
    file.seekg(offset, base);
    if (file.fail())
    {
        std::cout << "Pointer failed to set" << std::endl;
        throw std::runtime_error("Failed to set file pointer.");
    }
}

/**
 * @brief Opens a file for binary read/write access
 * 
 * This function opens a file in binary mode with both read and write access.
 * It includes error handling for open failures.
 * 
 * @param fileName Path to the file to open
 * @return std::fstream Open file stream
 * @throws std::runtime_error if file cannot be opened
 */
std::fstream openFile(std::string fileName)
{
    std::fstream file = std::fstream();
    file.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
    if (file.fail())
    {
        std::cout << "Failed to open file: " + fileName << std::endl;
        throw std::runtime_error("Failed to open file.");
    }
    return file;
}

/**
 * @brief Closes an open file
 * 
 * This function closes the file stream and includes error handling
 * for close failures.
 * 
 * @param file Reference to the file stream to close
 * @throws std::runtime_error if file cannot be closed
 */
void closeFile(std::fstream& file)
{
    file.close();
    if (file.fail())
    {
        std::cout << "Failed to close file" << std::endl;
        throw std::runtime_error("Failed to close file.");
    }
}

/**
 * @brief Writes data from a buffer to a file
 * 
 * This function writes the specified number of bytes from the buffer to
 * the file. It includes error handling for write failures and end-of-file
 * conditions.
 * 
 * @param file Reference to the open file stream
 * @param buffer Pointer to the data to write
 * @param bytesToWrite Number of bytes to write to the file
 * @throws std::runtime_error if write operation fails
 */
void writeToFile(std::fstream& file, void* buffer, std::streamsize bytesToWrite)
{
    file.write(static_cast<char*>(buffer), bytesToWrite);
    if (file.fail())
    {
        if (file.eof()) {std::cout << "End of file reached" << std::endl;}
        std::cout << "Failed to write to file: " << std::endl;
        throw std::runtime_error("Failed to write to file.");
    }
}
