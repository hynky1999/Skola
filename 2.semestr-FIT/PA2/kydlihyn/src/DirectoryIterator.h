#pragma once
#include "File.h"

#include <memory>
#include <set>
#include <vector>

/**
 * @brief Class for emulating iterator as is in std::containers
 *
 */

class DirectoryIterator{
private:
	std::vector<std::shared_ptr<File>> files;
	bool recursive;
	bool includeDir;
public:
	DirectoryIterator();

	DirectoryIterator(std::multiset<std::shared_ptr<File>, FileComparator> & set,
			bool reverse, bool recursive, bool includeDir);
	DirectoryIterator & operator ++ ();

	std::shared_ptr<File> & operator * ();

	bool operator != (const DirectoryIterator & other) const ;
};

/**
 * @brief Class for emulating const iterator as is in std::containers
 */
class DirectoryIteratorConst{
private:
	std::vector<std::shared_ptr<const File>> files;
	bool recursive;
	bool includeDir;
public:
	DirectoryIteratorConst();

	DirectoryIteratorConst(const std::multiset<std::shared_ptr<File>, FileComparator> & set,
			bool reverse, bool recursive, bool includeDir);

	DirectoryIteratorConst & operator ++ ();

	const std::shared_ptr<const File> & operator * () const;

	bool operator != (const DirectoryIteratorConst & other) const ;
};
