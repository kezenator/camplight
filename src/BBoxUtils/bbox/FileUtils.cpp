/**
 * @file
 *
 * Implementation file for the bbox::FileUtils class.
 */

#include <bbox/FileUtils.h>
#include <bbox/Exception.h>
#include <bbox/Format.h>
#include <bbox/TextCoding.h>
#include <bbox/Assert.h>

#ifdef WIN32

#include <Windows.h>
#include <bbox/Assert.h>

namespace {

    /**
     * A RAII wrapper for a Win32 HANDLE.
     */
    class Handle
    {
        // Copy operations are not allowed
        // at this time
        Handle(const Handle &other) = delete;
        void operator =(const Handle &other) = delete;

    public:
        Handle()
            : m_handle(INVALID_HANDLE_VALUE)
        {
        }

        explicit Handle(HANDLE handle)
            : m_handle(handle)
        {
        }

        ~Handle()
        {
            ::CloseHandle(m_handle);
        }

        HANDLE ToHandle()
        {
            return m_handle;
        }

        void Assign(HANDLE handle)
        {
            BBOX_ASSERT(m_handle == INVALID_HANDLE_VALUE);

            m_handle = handle;
        }

        HANDLE *GetPointer()
        {
            BBOX_ASSERT(m_handle == INVALID_HANDLE_VALUE);

            return &m_handle;
        }

    private:
        HANDLE m_handle;
    };

} // annonymous namespace 

#else // !WIN32

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <glob.h>

namespace {

    /**
    * A RAII wrapper for a file descriptor.
    */
    class FD
    {
        // Copy operations are not allowed
        // at this time
        FD(const FD&other) = delete;
        void operator =(const FD &other) = delete;

    public:
        FD()
            : m_fd(-1)
        {
        }

        explicit FD(int fd)
            : m_fd(fd)
        {
        }

        ~FD()
        {
            close(m_fd);
        }

    private:
        int m_fd;
    };

} // annonymous namespace 

#endif // !WIN32

namespace bbox {

#ifdef WIN32
    const std::string FileUtils::FOLDER_SEPERATOR("\\");
#else // not WIN32
    const std::string FileUtils::FOLDER_SEPERATOR("/");
#endif

    std::string FileUtils::ToWindowsPath(const std::string &path)
    {
        std::string result(path);
        for (char &ch : result)
        {
            if (ch == '/') ch = '\\';
        }
        return result;
    }

    std::string FileUtils::ToUnixPath(const std::string &path)
    {
        std::string result(path);
        for (char &ch : result)
        {
            if (ch == '\\') ch = '/';
        }
        return result;
    }

    std::string FileUtils::GetCurrentWorkingDir()
    {
        std::string result;

        Error error = GetCurrentWorkingDir(result);

        if (error)
        {
            throw Exception(error, Format("Error getting current working directory"));
        }

        return result;
    }

    Error FileUtils::GetCurrentWorkingDir(std::string &result)
    {
#ifdef WIN32
        result.clear();

        // First, get the length of the buffer required,
        // not including the null terminator

        DWORD req_length = ::GetCurrentDirectoryW(0, NULL);

        if (req_length == 0)
        {
            return Error::Win32_GetLastError();
        }

        // Now, allocate a buffer of this size plus an
        // extra for the null terminator, then
        // get the current directory and convert
        // to an internal UTF-8 string representation.

        std::wstring buffer_wide;
        buffer_wide.resize(req_length + 1);

        DWORD got_length = ::GetCurrentDirectoryW(DWORD(buffer_wide.size()), &buffer_wide[0]);

        if (got_length == 0)
            return Error::Win32_GetLastError();

        BBOX_ASSERT((got_length > 0) && (got_length < buffer_wide.size()));

        buffer_wide.resize(got_length);

        return TextCoding::Win32_UTF16_to_UTF8(buffer_wide, result);
#else // not WIN32
        result.resize(512);
        
        while (true)
        {
            char *gc_result = getcwd(&result[0], result.size());
            
            if (gc_result)
            {
              result.resize(strlen(gc_result));
              return Error::Success;
            }
            else if (errno == ERANGE)
            {
              result.resize(result.size() + 512);
              continue;
            }
            return Error::posix_errno();
        }
#endif
    }

    Error FileUtils::GetFileSize(const std::string &file_name, uint64_t &result)
    {
#ifdef WIN32
        result = 0;

        // First, we need to convert the internal UTF-8 file name
        // into the Win32 UTF-16 format

        std::wstring file_name_wide;
        Error err = TextCoding::Win32_UTF8_to_UTF16(file_name, file_name_wide);
        if (err)
            return err;

        // Now try and get the attributes of the file

        WIN32_FILE_ATTRIBUTE_DATA data;
        if (!GetFileAttributesEx(file_name_wide.c_str(), GetFileExInfoStandard, &data))
        {
            return Error::Win32_GetLastError();
        }

        // All good - store the size and return

        result = data.nFileSizeLow + (uint64_t(data.nFileSizeHigh) << 32);

        return Error::Success;

#else // not WIN32
        struct stat stat_buf;
        int stat_result = stat(file_name.c_str(), &stat_buf);

        if (stat_result == 0)
        {
            result = stat_buf.st_size;
            return bbox::Error();
        }
        return bbox::Error::posix_errno();
#endif
    }

    std::vector<uint8_t> FileUtils::ReadBinaryFileOrThrow(const std::string &file_name, uint64_t max_size)
	{
		std::vector<uint8_t> contents;

		Error error = ReadBinaryFile(file_name, contents, max_size);

		if (error)
		{
			throw Exception(error, Format("Error reading binary file \"%s\"", file_name));
		}

		return contents;
	}

	Error FileUtils::ReadBinaryFile(const std::string &file_name, std::vector<uint8_t> &contents, uint64_t max_size)
	{
#ifdef WIN32

        // First, we need to convert the internal UTF-8 file name
        // into the Win32 UTF-16 format

        std::wstring file_name_wide;
        Error err = TextCoding::Win32_UTF8_to_UTF16(file_name, file_name_wide);
        if (err)
            return err;

        // Now we can open this file and read the bytes
        // using the Win32 API

        Handle handle(::CreateFile(
            file_name_wide.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL));

        if (handle.ToHandle() == INVALID_HANDLE_VALUE)
            return Error::Win32_GetLastError();

        // The file is opened correctly - get the file size

        LARGE_INTEGER file_size_large_int;

        if (!::GetFileSizeEx(handle.ToHandle(), &file_size_large_int))
            return Error::Win32_GetLastError();

        // Check that the received file size fits into a size_t
        // variable.

        size_t file_size = size_t(file_size_large_int.QuadPart);
        DWORD dword_file_size = DWORD(file_size);

        if ((file_size != file_size_large_int.QuadPart)
            || (dword_file_size != file_size)
            || (file_size >= std::string::npos))
        {
            return std::errc::file_too_large;
        }

        // Now, also check that it meets the maximum size limit

        static_assert(sizeof(file_size_large_int.QuadPart) <= sizeof(max_size), "Windows file size is larger than uint64_t");

        if ((file_size_large_int.QuadPart < 0)
            || (uint64_t(file_size_large_int.QuadPart) > max_size))
        {
            return std::errc::file_too_large;
        }

        // Read all the data int

        contents.resize(file_size);

        DWORD offset = 0;
        while (offset < file_size)
        {
            DWORD bytes_read;

            if (!::ReadFile(
                        handle.ToHandle(),
                        &contents[offset],
                        dword_file_size - offset,
                        &bytes_read,
                        NULL))
            {
                return Error::Win32_GetLastError();
            }

            if (bytes_read == 0)
            {
                // No error - but no bytes ready -
                // don't know what's happening - the
                // file size must have changed even though
                // we asked for read-only sharing

                return std::errc::io_error;
            }

            offset += bytes_read;
        }

        // All done!

        return Error::Success;
#else
        int fd = open(ToUnixPath(file_name).c_str(), O_RDONLY);
        if (fd == -1)
        {
            return Error::posix_errno();
        }
        FD fd_raii(fd);

        struct stat stat_buf;
        int stat_result = fstat(fd, &stat_buf);
        if (stat_result != 0)
        {
            return Error::posix_errno();
        }

        contents.resize(stat_buf.st_size);

        ssize_t bytes_read = read(fd, &contents[0], stat_buf.st_size);
        if (bytes_read == -1)
        {
            return Error::posix_errno();
        }
        if (bytes_read != stat_buf.st_size)
        {
            return boost::system::errc::file_too_large;
        }

        return Error::Success;
#endif
	}

	std::string FileUtils::ReadTextFileOrThrow(const std::string &file_name)
	{
		std::string contents;

		Error error = ReadTextFile(file_name, contents);

		if (error)
		{
			throw Exception(error, Format("Error reading text file \"%s\"", file_name));
		}

		return contents;
	}

    Error FileUtils::ReadTextFile(const std::string &file_name, std::string &contents)
    {
        std::vector<uint8_t> bytes;
        Error error = ReadBinaryFile(file_name, bytes, 10 * 1024 * 1024);
        if (error)
            return error;

        return TextCoding::ExternalBytes_to_UTF8(bytes, contents);
    }

    void
    FileUtils::WriteTextFileOrThrow(const std::string &file_name, const std::string &contents)
    {
        Error error = WriteTextFile(file_name, contents);

        if (error)
        {
            throw Exception(error, Format("Error writing text file \"%s\"", file_name));
        }
    }

    Error
    FileUtils::WriteTextFile(const std::string &file_name, const std::string &contents)
    {
#ifdef WIN32

        // First, we need to convert the internal UTF-8 file name
        // into the Win32 UTF-16 format.

        std::wstring file_name_wide;
        Error err = TextCoding::Win32_UTF8_to_UTF16(file_name, file_name_wide);
        if (err)
            return err;

        // Now we can open this file and write the bytes
        // using the Win32 API

        Handle handle(::CreateFile(
            file_name_wide.c_str(),
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL));

        if (handle.ToHandle() == INVALID_HANDLE_VALUE)
            return Error::Win32_GetLastError();

        // Check that the contents length fits into
        // a DWORD - this function is designed for small text files
        // (e.g. configuration files) that fit into memory - not
        // multi-giga-byte data files.

        DWORD file_size = DWORD(contents.size());

        if ((file_size != contents.size())
            || (file_size >= std::string::npos))
        {
            return std::errc::file_too_large;
        }

        // Write all the data out

        DWORD offset = 0;
        while (offset < file_size)
        {
            DWORD bytes_written;

            if (!::WriteFile(
                handle.ToHandle(),
                &contents[offset],
                file_size - offset,
                &bytes_written,
                NULL))
            {
                return Error::Win32_GetLastError();
            }

            if (bytes_written == 0)
            {
                // No error - but no bytes ready -
                // don't know what's happening??

                return std::errc::io_error;
            }

            offset += bytes_written;
        }

        return Error();
#else
        int fd = open(ToUnixPath(file_name).c_str(),
                      O_RDWR | O_CREAT | O_TRUNC,
                      S_IRWXU | S_IRWXG | S_IRWXO);
        if (fd == -1)
        {
            return Error::posix_errno();
        }
        FD fd_raii(fd);

        ssize_t bytes_written = write(fd, &contents[0], contents.size());
        if (bytes_written < 0)
        {
            return Error::posix_errno();
        }
        if (size_t(bytes_written) != contents.size())
        {
            return boost::system::errc::file_too_large;
        }

        return Error::Success;
#endif
    }

    void FileUtils::WriteBinaryFileOrThrow(const std::string &file_name, const std::vector<uint8_t> &contents)
    {
        Error error = WriteBinaryFile(file_name, contents);

        if (error)
        {
            throw Exception(error, Format("Error writing binary file \"%s\"", file_name));
        }
    }

    Error FileUtils::WriteBinaryFile(const std::string &file_name, const std::vector<uint8_t> &contents)
    {
        static_assert(sizeof(contents[0]) == 1, "std::string characters are not size 1");

        std::string contents_as_str;
        contents_as_str.resize(contents.size());

        if (contents_as_str.size() != 0)
        {
            memcpy(&contents_as_str[0], &contents[0], contents.size());
        }

        return WriteTextFile(file_name, contents_as_str);
    }

    std::list<FileInfo> FileUtils::ListFolderOrThrow(const std::string &search, FileInfo::EFileType file_type)
    {
        std::list<FileInfo> results;

        Error err = ListFolder(search, file_type, results);

        if (err)
        {
            throw Exception(err, Format("Error listing folder with search \"%s\"", search));
        }

        return std::move(results);
    }

    Error FileUtils::ListFolder(const std::string &search, FileInfo::EFileType file_type, std::list<FileInfo> &results)
    {
#ifdef WIN32

        // Clear the results list

        results.clear();

        // Generate the path name of the search
        // to be able to generate full path names

        std::string search_path;
        {
            size_t pos = search.rfind('\\');
            if (pos == std::string::npos)
            {
                return Error(std::errc::invalid_argument);
            }
            search_path = search.substr(0, pos);
        }

        // Generate a full UNICODE file name for
        // the path

        Error error;

        std::wstring unc_search;
        error = TextCoding::Win32_UTF8_to_UTF16(Format("\\\\?\\%s", search), unc_search);
        if (error)
            return error;

        // Next, start the search operation

        WIN32_FIND_DATAW find_data;
        _FINDEX_SEARCH_OPS search_op = FindExSearchNameMatch;
        if (file_type == FileInfo::Folder)
            search_op = FindExSearchLimitToDirectories;

        HANDLE handle = ::FindFirstFileExW(
            unc_search.c_str(),
            FindExInfoBasic,
            &find_data,
            search_op,
            NULL,
            FIND_FIRST_EX_LARGE_FETCH);

        if (handle == INVALID_HANDLE_VALUE)
        {
            if (::GetLastError() == ERROR_FILE_NOT_FOUND)
            {
                // There are just no files found in this
                // directory - it's empty

                return Error::Success;
            }

            return Error::Win32_GetLastError();
        }

        do
        {
            // Add this entry to the list of results

            std::string name;
            std::string full_path;
            std::uint64_t size;
            FileInfo::EFileType type;

            // First filter out only the types
            // of files that are requested
            // and also filter out hidden files.

            type = FileInfo::OtherFile;
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                type = FileInfo::Folder;
            else
                type = FileInfo::RegularFile;

            if ((type == file_type)
                && ((find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0))
            {
                error = TextCoding::Win32_UTF16_to_UTF8(find_data.cFileName, name);
                if (error)
                    return error;

                // Make sure we filter out the special names "." and ".."

                if ((name != ".")
                    && (name != ".."))
                {
                    full_path = Format("%s\\%s", search_path, name);

                    size = std::uint64_t(find_data.nFileSizeHigh) << 32
                        | find_data.nFileSizeLow;

                    results.push_back(FileInfo(
                        std::move(name),
                        std::move(full_path),
                        size,
                        type));
                }
            }

            // Find the next file that matches

            if (!::FindNextFileW(handle, &find_data))
            {
                if (::GetLastError() == ERROR_NO_MORE_FILES)
                {
                    // There are no more files to be found -
                    // just break out of the loop
                    break;
                }

                return Error::Win32_GetLastError();
            }

        } while (true);

        // Finally, close the find handle

        ::FindClose(handle);

        return Error::Success;

#else

        std::string unix_search = ToUnixPath(search);

        int glob_flags = GLOB_ERR;

        if (file_type == FileInfo::Folder)
            glob_flags |= GLOB_ONLYDIR;

        glob_t glob_buf;

        int glob_result = glob(
            ToUnixPath(search).c_str(),
            glob_flags,
            nullptr,
            &glob_buf);

        results.clear();

        if (glob_result == GLOB_NOSPACE)
            return boost::system::errc::not_enough_memory;
        else if (glob_result == GLOB_ABORTED)
            return boost::system::errc::io_error;
        else if (glob_result == GLOB_NOMATCH)
            return Error::Success;
        else if (glob_result != 0)
            return boost::system::errc::io_error;
        else
        {
            for (int i = 0; glob_buf.gl_pathv[i]; ++i)
            {
                struct stat stat_buf;
                if (0 == stat(glob_buf.gl_pathv[i], &stat_buf))
                {
                    FileInfo::EFileType found_type = FileInfo::OtherFile;

                    if (S_ISDIR(stat_buf.st_mode))
                        found_type = FileInfo::Folder;
                    else if (S_ISREG(stat_buf.st_mode))
                        found_type = FileInfo::RegularFile;

                    if (found_type == file_type)
                    {
                        std::string full_path(glob_buf.gl_pathv[i]);

                        size_t last_slash_pos = full_path.rfind('/');
                        BBOX_ASSERT(last_slash_pos != std::string::npos);
                        BBOX_ASSERT((last_slash_pos + 1) < full_path.size());
                        std::string name(full_path.substr(last_slash_pos+1));

                        results.push_back(FileInfo(
                            std::move(name),
                            std::move(full_path),
                            stat_buf.st_size,
                            found_type));
                    }
                }
            }

            globfree(&glob_buf);
            return Error::Success;
        }
#endif
    }

}