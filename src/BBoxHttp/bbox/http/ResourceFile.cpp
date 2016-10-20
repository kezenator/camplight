/**
* @file
*
* Implementation for the bbox::http::ResourceFile class.
*/

#include <bbox/http/ResourceFile.h>

namespace bbox {
    namespace http {

        ResourceFile::ResourceFile(const char *name, const uint8_t *data, size_t length, const char *encoding, const char *mime_type, const char *strong_etag
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

        ResourceFile::~ResourceFile()
        {
        }

    } // namespace bbox::http
} // namespace bbox
