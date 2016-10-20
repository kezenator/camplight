/**
* @file
*
* Implementation for the bbox::http::ResourceFileSet class.
*/

#include <bbox/http/ResourceFileSet.h>

namespace bbox {
    namespace http {

        ResourceFileSet::ResourceFileSet(std::initializer_list<InitEntry> init_list)
        {
            for (const InitEntry &entry : init_list)
            {
                m_files[std::string(entry.m_name)] = std::unique_ptr<ResourceFile>(
                    new ResourceFile(
						entry.m_name,
						entry.m_data,
						entry.m_length,
						entry.m_encoding,
						entry.m_mime_type,
						entry.m_strong_etag
#ifdef _DEBUG
						, entry.m_orig_file_name
#endif
					));
            }
        }

        ResourceFileSet::~ResourceFileSet()
        {
        }

        const ResourceFile *ResourceFileSet::FindFile(const std::string &path) const
        {
            std::string real_path = path;
            if ((real_path.size() != 0)
                && (real_path[0] == '/'))
            {
                real_path = real_path.substr(1);
            }

            auto it = m_files.find(real_path);
            if (it == m_files.end())
                return nullptr;
            else
                return it->second.get();
        }

    } // namespace bbox::http
} // namespace bbox
