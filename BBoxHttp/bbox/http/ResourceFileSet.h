/**
 * @file
 *
 * Header for the bbox::http::ResourceFileSet class.
 */

#ifndef __BBOX__RT__HTTP__RESOURCE_FILE_SET_H__
#define __BBOX__RT__HTTP__RESOURCE_FILE_SET_H__

#pragma once

#include <initializer_list>
#include <string>
#include <cstdint>
#include <map>
#include <memory>

#include <bbox/http/ResourceFile.h>

namespace bbox {
    namespace http {

        /**
         * A collection of resource files, and
         * operations to find/access them.
         */
        class ResourceFileSet
        {
            // Not a value type
            ResourceFileSet() = delete;
            ResourceFileSet(const ResourceFileSet &) = delete;
            void operator =(const ResourceFileSet &) = delete;

        public:

            struct InitEntry
            {
                InitEntry(const char *name, const uint8_t *data, size_t length, const char *encoding, const char *mime_type, const char *strong_etag
#ifdef _DEBUG
					, const char *orig_file_name
#endif
				)
                    : m_name(name)
                    , m_data(data)
                    , m_length(length)
                    , m_encoding(encoding)
                    , m_mime_type(mime_type)
                    , m_strong_etag(strong_etag)
#ifdef _DEBUG
					, m_orig_file_name(orig_file_name)
#endif
                {
                }

                const char *m_name;
                const uint8_t *m_data;
                size_t m_length;
                const char *m_encoding;
                const char *m_mime_type;
                const char *m_strong_etag;
#ifdef _DEBUG
				const char *m_orig_file_name;
#endif
            };

            explicit ResourceFileSet(std::initializer_list<InitEntry> init_list);
            ~ResourceFileSet();

            const ResourceFile *FindFile(const std::string &path) const;

        private:
            std::map<std::string, std::unique_ptr<ResourceFile>> m_files;
        };

    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__RT__HTTP__RESOURCE_FILE_SET_H__
