#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <filesystem>
#include <chrono>
#include <unordered_set>
#include <vector>

/**
 * @brief Abstract class representing a File
 * It should be understood as file in unix term therefore directory is also a file
 *
 */
class File{
protected:
	size_t index;
	std::filesystem::path path;
	std::size_t size;
	std::filesystem::file_time_type last_mod;

public:
	static size_t next_index;

	File(std::filesystem::path path);

	File(std::filesystem::path path, size_t size, std::filesystem::file_time_type last_mod);

	/**
	 * @brief Constructor for loading file from stream, it is basically inverse of SaveAsData
	 *
	 * @param ifs stream to be laoded from
	 */
	File(std::ifstream & ifs);

	/**
	 * @brief Factory function for creating derivatives of File extension and then data, therefore if there is wrong extension file will not
	 * be identified correctly and will be cast to BinaryFile. If file extension and data mismatch BinaryFile will be used again.
	 *
	 * @param path path to which will be created
	 *
	 * @param files_visited is hash_set of canonical paths to files visited to prevent cycles
	 * @param follow_sym_links flag to follow symbolic links or not
	 * @param force_default flag to force default identification. Current default is Binary_File
	 *
	 * @return shared pointer with derived class
	 */
	static std::shared_ptr<File> CreateFile(const std::filesystem::path & path, std::unordered_set<std::string> & files_visited, bool follow_sym_links = false, bool force_default = false);

	/**
	 * @brief Factory function for creating derivatives of File based on stream, more specifically first size_t of stream
	 *
	 * @param ifs stream to load data from
	 *
	 * @return shared pointer with derived class
	 */
	static std::shared_ptr<File> CreateFile(std::ifstream & ifs);

	/**
	 * @brief Utility function which takes vector of offsets and prints either space or | filled with spaces in order to print nice tree structure
	 * offset = true -> | + spacing offset = false -> ' ' + spacing
	 *
	 * @param os stream to be print to
	 *
	 * @param offsets vector in which its elements define characters to be printed
	 */
	static void PrintOffset(std::ostream & os, const std::vector<bool> & offsets);

	/**
	 * @brief Function for printing file to tree structure
	 *
	 * @param os stream to print to
	 *
	 * @param offsets vector of offsets as defined in PrintOffset function
	 */
	virtual void PrintTree(std::ostream & os, std::vector<bool> & offsets) const;

	/**
	 * @brief Function to print basic information about Print most likely filename
	 *
	 * @param os stream to print to
	 */
	virtual void Print(std::ostream & os) const;

	/**
	 * @brief Function which prints all information about file
	 *
	 * @param os stream to print to
	 */
	virtual void PrintExtended(std::ostream & os) const;

	/**
	 * @brief Function which compares two files against each other and prints if file has been modified MOD if file was add ++ if file was removed --
	 * If they are the same file will not be printed. TODO would be nice to add Tree print with some nice colors, however that is a lot of work and will not
	 * be implemented in semestral work
	 *
	 * @param other file to compare with
	 * @param os stream to print to
	 *
	 * @return true if files are the same else false
	 */
	virtual bool ComparePrint(const File & other, std::ostream & os) const;

	/**
	 * @brief Compares if two file's content is equal
	 *
	 * @param other file to compare against
	 *
	 * @return true if files content is equal else false
	 */
	bool virtual CompareContent (const File & other) const;

	/**
	 * @brief Function which saves files data to stream
	 *
	 * @param ofs stream to save to
	 */
	virtual void SaveAsData(std::ofstream & ofs) const;


	virtual const std::filesystem::path & GetPath() const;
	virtual const size_t & GetIndex() const;
	virtual const size_t & GetSize() const;
	virtual const std::filesystem::path GetFilename() const;
	virtual const std::filesystem::file_time_type & GetModTime() const;
	virtual const std::string & GetType() const = 0;

	virtual ~File(){}
};

/**
 * @brief Functor for comparing files by its filenames
 */
struct FileComparator{
public:
	bool operator () (const std::shared_ptr<File> & cmp1, const std::shared_ptr<File> & cmp2) const;
};
