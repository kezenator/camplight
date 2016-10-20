/**
 * @file
 *
 * Header for the bbox::http::ResourceFile class.
 */

#ifndef __BBOX__RT__HTTP__RESOURCE_FILE_H__
#define __BBOX__RT__HTTP__RESOURCE_FILE_H__

#pragma once

#include <cstdint>
#include <cstdlib>

namespace bbox {
    namespace http {

        // Forward declaration
        class ResourceFileSet;

        /**
         * A resource file in a set.
         */
        class ResourceFile
        {
            friend class ResourceFileSet;

            // Not a value type
            ResourceFile() = delete;
            ResourceFile(const ResourceFile &) = delete;
            void operator =(const ResourceFile &) = delete;

            ResourceFile(
				const char *name,
				const uint8_t *data,
				size_t length,
				const char *encoding,
				const char *mime_type,
				const char *strong_etag
#ifdef _DEBUG
				, const char *orig_file_name
#endif
			);

        public:

            ~ResourceFile();

            const char *GetName() const { return m_name; }
            const uint8_t *GetData() const      { return m_data; }
            size_t GetSize() const              { return m_length; }
            const char *GetEncoding() const     { return m_encoding; }
            const char *GetMimeType() const     { return m_mime_type; }
            const char *GetStrongETag() const   { return m_strong_etag; }
#ifdef _DEBUG
			const char *GetOrigFileName() const { return m_orig_file_name; }
#endif

        private:
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

    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__RT__HTTP__RESOURCE_FILE_H__