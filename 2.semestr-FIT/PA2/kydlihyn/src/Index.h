#pragma once
#include "File.h"
#include "Directory.h"
#include "Expression.h"

#include <fstream>
#include <memory>
#include <functional>
#include <filesystem>

/**
 * @brief Class representing index of files
 *
 */
class Index{
	bool initialized;
	std::shared_ptr<File> files_structure;

public:
	Index(std::filesystem::path path);

	Index();

	/**
	 * @brief Function for saving index to specified filename
	 *
	 * @param filename name of file where contents of index will be saved to
	 *
	 * @return true if index was successfully saved else false
	 */
	bool Save(const char * filename) const;

	/**
	 * @brief Function for loading index from specified filename
	 *
	 * @param filename name of file from which index will be loaded
	 *
	 * @return true if index was successfully loaded else false
	 */
	bool Load(const char * filename);

	/**
	 * @brief Function creates itself as index of specified file (Filename does not have to be a directory)
	 *
	 * @param path path to file which will be starting point of index
	 * @param follow_sym_links flag to either follow symlink directories or not
	 *
	 * @return true if index was created successffuly else false
	 */
	bool Create(std::filesystem::path path, bool follow_sym_links = false);

	/**
	 * @brief Function compare itself against other index
	 *
	 * @param other index to compare against
	 *
	 * @return true if indexes are the same else false
	 */
	bool Compare(Index & other) const;

	/**
	 * @brief Function searches throught all files indexed
	 *
	 * @param eval Expression to be evaluated with each file
	 * @param f_print print function to be used on found files
	 * @param include_dir flag to include directories to search
	 *
	 * @return true if any file was found else false
	 */
	bool Search(BasicExpr & eval, std::function<void(const File&, std::ostream &)> f_print, bool include_dir = false) const;

	/**
	 * @brief Checks if Index was loaded or created correctly
	 *
	 * @return true if correctly loaded else false
	 */
	bool Initialized() const;

	friend std::ostream & operator << (std::ostream & os, Index & idx);
};
