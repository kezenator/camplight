/**
* @file
*
* Header file for the bbox::FileInfo class.
*/

#ifndef __BBOX__FILE_INFO_H__
#define __BBOX__FILE_INFO_H__

#pragma once

#include <string>
#include <cstdint>

namespace bbox {

    /**
     * Information about a found file.
     *
     * Returned by FileUtils::ListFolder.
     */
    class FileInfo
    {
    public:

        /**
         * The type of a file.
         */
        enum EFileType
        {
            OtherFile,
            RegularFile,
            Folder,
        };

        FileInfo()
            : m_size(0)
            , m_type(OtherFile)
        {
        }

        FileInfo(std::string &&_name,
            std::string &&_full_path,
            std::uint64_t _size,
            EFileType _type);

        ~FileInfo()
        {
        }

        /**
         * Gets the name of the file.
         */
        const std::string &GetName() const
        {
            return m_name;
        }

        /**
         * Gets the full path of the file.
         */
        const std::string &GetFullPath() const
        {
            return m_full_path;
        }

        /**
         * Gets the size of the file.
         *
         * Only valid for regular files.
         */
        std::uint64_t GetSize() const
        {
            return m_size;
        }

        /**
         * Gets the type of the file.
         */
        EFileType GetType() const
        {
            return m_type;
        }

    private:
        std::string m_name;
        std::string m_full_path;
        std::uint64_t m_size;
        EFileType m_type;
    };

} // namespace bbox

#endif // __BBOX__FILE_INFO_H__

