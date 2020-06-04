#pragma once
#include "File.h"
#include "DirectoryIterator.h"

#include <iostream>
#include <ostream>
#include <set>
#include <unordered_set>
#include <filesystem>

/**
 * @brief File representing Directory, contains pointers to its content
 *
 */
class Directory : public File{
	std::multiset<std::shared_ptr<File>, FileComparator> files;

public:
	static const char type;

	Directory(std::filesystem::path path, std::unordered_set<std::string> & files_visited, bool follow_sym_links = false);

	Directory(std::ifstream & ifs);

	void PrintTree(std::ostream & os, std::vector<bool> & offsets) const override;

	void Print(std::ostream & os) const override;

	void PrintExtended(std::ostream & os) const override;

	void SaveAsData(std::ofstream & ofs) const override;

	bool ComparePrint(const File & other, std::ostream & os) const override;
	
	bool CompareContent(const File & other) const override;

	const std::string & GetType() const override;

	/**
	 * @brief Functions for returning directory iterators
	 *
	 * @param recursive if true iterator will iterate over content of directories encountered
	 * @param includeDir if true iterator will also iterate over directories
	 *
	 * @return 
	 */
	DirectoryIterator begin(bool recursive = true, bool includeDir = false);

	DirectoryIterator end();

	DirectoryIteratorConst cbegin(bool recursive = true, bool includeDir = false) const;

	DirectoryIteratorConst cend() const;

	DirectoryIterator rbegin(bool recursive = true, bool includeDir = false);

	DirectoryIterator rend();

	DirectoryIteratorConst crbegin(bool recursive = true, bool includeDir = false) const;

	DirectoryIteratorConst crend() const;
};
