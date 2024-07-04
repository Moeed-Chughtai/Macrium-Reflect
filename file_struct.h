#include <string.h>
#include <cstdint>
#include "enums.h"

namespace file_structs
{
    namespace Partition
    {
        struct File_System
        {
            ImageEnums::BitLocker bitlocker_state;
            char drive_letter;
            uint64_t end;
            uint32_t free_clusters;
            uint16_t lcn0_file_number;
            uint64_t lcn0_offset;
            uint64_t mft_offset;
            uint32_t mft_record_size;
            uint32_t partition_index;
            uint32_t sectors_per_cluster;
            std::string shadow_copy;
            uint64_t start;
            uint32_t total_clusters;
            ImageEnums::FileType type;
            std::string volume_guid;
            std::string volume_label;
        };
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(File_System, type, mft_offset, mft_record_size, drive_letter, end, start, free_clusters, lcn0_offset,
                                                        reserved_sectors_byte_length, sectors_per_cluster, total_clusters, volume_guid, volume_label, lcn0_file_number, bitlocker_state, partition_index, shadow_copy);

        struct Geometry
        {
            uint64_t boot_sector_offset;
            uint64_t end;
            uint64_t length;
            uint64_t start;
        };
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PartitionGeometry, start, end, length, boot_sector_offset)

        struct FileHistory
        {
            std::string file_name;
            int32_t file_number;
        };

        struct Header
        {
            uint32_t block_count;
            uint32_t block_size;
            std::vector<FileHistory> file_history;
            uint32_t file_history_count;
            uint64_t partition_file_offset;
            int32_t partition_number;
        };

        struct Table_Entry
        {
            bool active;
            uint32_t boot_sector;
            uint16_t end_cylinder;
            uint8_t end_head;
            uint32_t num_sectors;
            ImageEnums::PartitionType partition_type;
            uint16_t start_cylinder;
            uint8_t start_head;
            uint8_t status;
            uint8_t type;
        };

        struct Partition_Layout
        {
            File_System _file_system;
            Geometry _geometry;
            Header _header;
            Table_Entry _partition_table_entry;
        };
    };

    namespace Disk
    {
        struct Descriptor
        {
            std::string disk_description;
            std::string disk_manufacturer;
            std::string disk_productid;
            std::string disk_revisonno;
            std::string disk_serialno;
        };

        struct Geometry
        {
            uint32_t bytes_per_sector;
            uint64_t cylinders;
            uint64_t disk_size;
            std::string media_type;
            uint32_t sectors_per_track;
            uint32_t tracks_per_cylinder;
        };

        struct Header
        {
            std::string disk_format;
            uint32_t disk_number;
            std::string disk_signature;
            int32_t imaged_partition_count;
        };

        struct DiskLayout
        {
            Descriptor _descriptor;
            Geometry _geometry;
            Header _header;
            std::vector<Partition::Partition_Layout> partitions;
        };
    };

    struct Header
    {
        std::string backup_format;
        std::string backup_guid;
        __time64_t backup_time;
        std::string backup_type;
        __time64_t backupset_time;
        bool delta_index;
        uint16_t file_number;
        uint16_t imaged_disks_count;
        std::string imageid;
        uint16_t increment_number;
        uint64_t index_file_position;
        int32_t json_version;
        std::string netbios_name;
        bool split_file;
    };

    struct Compression
    {
        std::string compression_level;
        std::string compression_method;
    };

    struct Encryption
    {
        bool enable;
        uint32_t key_iterations;
    };

    struct FileLayout
    {
        Compression compression;
        Encryption encryption;
        Header header;
        std::vector<Disk::DiskLayout> disks;
    };
}