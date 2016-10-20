#include "FileInfo.h"

namespace bbox {
    
    FileInfo::FileInfo(std::string &&_name,
                       std::string &&_full_path,
                       std::uint64_t _size,
                       EFileType _type)
        : m_name(std::move(_name))
        , m_full_path(std::move(_full_path))
        , m_size(_size)
        , m_type(_type)
    {
    }

} // namespace bbox
