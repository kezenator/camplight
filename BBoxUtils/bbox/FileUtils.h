/**
 * @file
 *
 * Header file for the bbox::FileUtils class.
 */

#ifndef __BBOX__FILE_UTILS_H__
#define __BBOX__FILE_UTILS_H__

#pragma once

#include <list>
#include <string>
#include <vector>

#include <bbox/Error.h>
#include <bbox/FileInfo.h>

namespace bbox {

	/**
	 * Provides simple file utilties for reading, writing and searching
	 * for files.
	 *
	 * As with all bbox classes, all methods deal with UTF-8 strings only.
	 * Use the TextCoding to convert between classes if required.
	 */
	class FileUtils
	{
		// Cannot create instances of this class - static members only
		FileUtils() = delete;
		~FileUtils() = delete;
		FileUtils(const FileUtils &) = delete;
		void operator =(const FileUtils &) = delete;

    public:

        static const std::string FOLDER_SEPERATOR;

        /**
         * Converts a path to Windows folder separators.
         */
        static std::string ToWindowsPath(const std::string &path);

        /**
         * Converts a path to Unix/Linux/URL folder separators.
         */
        static std::string ToUnixPath(const std::string &path);

        /**
         * Converts a path to use the local system folder separators.
         */
        static std::string ToSystemPath(const std::string &path)
        {
#ifdef WIN32
            return ToWindowsPath(path);
#else // not WIN32
            return ToUnixPath(path);
#endif // not WIN32
        }

        /**
         * Gets the current working directory.
         * Returns the contents as a string, and throws
         * an exception on error.
         */
        static std::string GetCurrentWorkingDir();

        /**
         * Gets the current working directory,
         * and returns an error on failure.
         */
        static Error GetCurrentWorkingDir(std::string &result);

        /**
         * Gets the size of the specified file and returns an
         * error on failure.
         */
        static Error GetFileSize(const std::string &file_name, uint64_t &result);

		/**
		 * Reads in the complete contents of a specified text file
		 * and returns the contents as a string. Throws an
		 * exception if an error occurs.
		 *
		 * @param file_name the name of the file to read.
         * @param max_size the maximum size of the file to read.
         * @return the contents of the file.
		 */
		static std::vector<uint8_t> ReadBinaryFileOrThrow(const std::string &file_name, uint64_t max_size);

		/**
		 * Reads in the complete contents of a specified
		 * binary file and stores in the specified parameter.
		 * Returns an error code.
		 *
		 * @param file_name the name of the file to read.
		 * @param contents the string to store the contents in.
         * @param max_size the maximum size of the file to read.
		 * @return an error code, or success if successful.
		 */
        static Error ReadBinaryFile(const std::string &file_name, std::vector<uint8_t> &contents, uint64_t max_size);

		/**
		 * Reads in the complete contents of a specified text file
		 * and returns the contents as a string. Throws an
		 * exception if an error occurs.
		 *
		 * @param file_name the name of the file to read.
		 * @return the contents of the file.
		 */
		static std::string ReadTextFileOrThrow(const std::string &file_name);

		/**
		 * Reads in the complete contents of a specified
		 * text file and stores in the specified parameter.
		 * Returns an error code.
		 *
		 * @param file_name the name of the file to read.
		 * @param contents the string to store the contents in.
		 * @return an error code, or success if successful.
		 */
		static Error ReadTextFile(const std::string &file_name, std::string &contents);

		/**
		 * Writes the specified contents to the file.
         * Throws an exception if an error occurs.
		 *
		 * @param file_name the name of the file to write.
		 * @param contents the contents to write.
		 */
        static void WriteTextFileOrThrow(const std::string &file_name, const std::string &contents);

		/**
		 * Writes in the complete contents of a specified
		 * text file. Returns an error code.
		 *
		 * @param file_name the name of the file to write.
		 * @param contents the contents to write.
		 * @return an error code, or success if successful.
		 */
		static Error WriteTextFile(const std::string &file_name, const std::string &contents);

        /**
         * Writes the specified contents to the file.
         * Throws an exception if an error occurs.
         *
         * @param file_name the name of the file to write.
         * @param contents the contents to write.
         */
        static void WriteBinaryFileOrThrow(const std::string &file_name, const std::vector<uint8_t> &contents);

        /**
         * Writes in the complete contents of a specified
         * text file. Returns an error code.
         *
         * @param file_name the name of the file to write.
         * @param contents the contents to write.
         * @return an error code, or success if successful.
         */
        static Error WriteBinaryFile(const std::string &file_name, const std::vector<uint8_t> &contents);

        /**
         * Finds all files in the specified directory.
         *
         * Throws an exception if an error occurs.
         *
         * @param search the search string, including path and wildcard.
         * @param file_type the type of files to find.
         * @return a list of found files.
         */
        static std::list<FileInfo> ListFolderOrThrow(const std::string &search, FileInfo::EFileType file_type);

        /**
         * Finds all files in the specified directory.
         *
         * @param search the search string, including path and wildcard.
         * @param file_type the type of files to find.
         * @param results the list to fill with found files.
         * @return an error or success.
         */
        static Error ListFolder(const std::string &search, FileInfo::EFileType file_type, std::list<FileInfo> &results);
	};

} // namespace bbox

#endif // __BBOX__FILE_UTILS_H__
