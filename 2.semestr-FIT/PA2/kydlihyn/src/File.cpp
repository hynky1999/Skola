#include "File.h"
#include "Directory.h"
#include "OperativeFile.h"
#include "RegularFile.h"
#include "SaveUtilities.h"
#include "Constants.hpp"

#include <chrono>
#include <filesystem>
#include <exception>
#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <ctime>

// Index will start with 0 as first number
size_t File::next_index = 0;

File::File(std::filesystem::path path): path(path), size(0), last_mod(std::filesystem::file_time_type::min()){
	this->index = File::next_index++;
}

File::File(std::filesystem::path path, size_t size, std::filesystem::file_time_type last_mod): path(path), size(size), last_mod(last_mod){
	this->index = File::next_index++;
}

File::File(std::ifstream & ifs){
		LoadPrimitive(ifs, index);
		std::string path_str;
		LoadString(ifs, path_str);
		path = path_str;
		LoadPrimitive(ifs, size);
		LoadTimeStamp(ifs, last_mod);
}

bool isExtension(const std::filesystem::path path, const std::vector<std::filesystem::path> exts){
	for(auto it = exts.cbegin(); it != exts.cend(); ++it){
		if(path == *it)
			return true;
	}
	return false;
}

std::shared_ptr<File> File::CreateFile(const std::filesystem::path & path, std::unordered_set<std::string> & files_visited, bool follow_sym_links, bool force_default){
	// Without cannonical path it is dangerous to continue rather do not index
	std::string abs_path;

	try{
		if(follow_sym_links)
			abs_path = std::filesystem::canonical(path);
		else
			abs_path = std::filesystem::absolute(path);
	}
	catch (std::filesystem::filesystem_error & e){
		std::cerr << FACTORY_CREATE_ERR << " : " << e.path1() << " " << e.code() << ":" << e.what() << std::endl;
		return std::shared_ptr<File>(nullptr);
	}
	// WE dont want to visit the same folder twice
	if(files_visited.count(abs_path) != 0)
		return std::shared_ptr<File>(nullptr);
	else
		files_visited.insert(abs_path);

	try{
		if(force_default)
			return std::make_shared<Binary_File>(path);

		// Check for symlink first to not interpret is as directory
		else if(!follow_sym_links && std::filesystem::is_symlink(path))
			return std::make_shared<Symbolic_Link>(path);

		else if(std::filesystem::is_directory(path))
			return std::make_shared<Directory>(path, files_visited, follow_sym_links);

		else if(std::filesystem::is_socket(path))
			return std::make_shared<Socket_File>(path);

		else if(std::filesystem::is_block_file(path))
			return std::make_shared<Block_File>(path);

		else if(std::filesystem::is_fifo(path))
			return std::make_shared<Fifo_File>(path);

		else if(std::filesystem::is_character_file(path))
			return std::make_shared<Character_File>(path);

		std::filesystem::path ext = path.extension();
		if(isExtension(ext, PNG_EXTS))
			return std::make_shared<PNG_File>(path);

		else if(isExtension(ext, JPG_EXTS))
			return std::make_shared<JPG_File>(path);

		else if(isExtension(ext, TEXT_EXTS))
			return std::make_shared<Text_File>(path);
		else
			return std::make_shared<Binary_File>(path);
	}

	catch (std::filesystem::filesystem_error & e){
		// Filesystem error is not caused by wrong identification of file
		files_visited.erase(abs_path);
		std::cerr << FACTORY_CREATE_ERR << " : " << e.path1() << " " << e.code() << ":" << e.what() << std::endl;
	}

	catch (std::exception & e) {
		files_visited.erase(abs_path);
		if(!force_default){
			return File::CreateFile(path, files_visited, follow_sym_links, true);
		}
		else{
			// Couldn't create file with default variant
			std::cout << is_directory(path) << std::endl;
			std::cerr << FACTORY_CREATE_ERR << " : " << path << e.what() << std::endl;
		}
	}

	return std::shared_ptr<File>(nullptr);
}

std::shared_ptr<File> File::CreateFile(std::ifstream & ifs){
	try{
		char type;
		LoadPrimitive(ifs, type);
		if(type == Directory::type)
			return std::make_shared<Directory>(ifs);

		else if(type == Symbolic_Link::type)
			return std::make_shared<Symbolic_Link>(ifs);

		else if(type == Block_File::type)
			return std::make_shared<Block_File>(ifs);

		else if(type == Fifo_File::type)
			return std::make_shared<Fifo_File>(ifs);

		else if(type == Socket_File::type)
			return std::make_shared<Socket_File>(ifs);

		else if(type == Character_File::type)
			return std::make_shared<Character_File>(ifs);

		else if(type == JPG_File::type)
			return std::make_shared<JPG_File>(ifs);

		else if(type == PNG_File::type)
			return std::make_shared<PNG_File>(ifs);

		else if(type == Text_File::type)
			return std::make_shared<Text_File>(ifs);

		else if(type == Binary_File::type)
			return std::make_shared<Binary_File>(ifs);

		return std::shared_ptr<File>(nullptr);
	}
	catch (std::exception & e) {
		std::cerr << FACTORY_CREATE_ERR << std::endl;
		return std::shared_ptr<File>(nullptr);
	}
}

void File::PrintOffset(std::ostream & os, const std::vector<bool> & offsets){
	if(offsets.size() == 0)
		return;

	auto it = offsets.cbegin();
	for(auto end = --offsets.cend(); it != end; ++it){
		if(*it)
			os << '|';
		else
			os << ' ';
		os << std::string(SPACING, ' ');
	}
	if(*it)
		os << '|';
	else
		os << '\\';
	os << std::string(SPACING, '-');
}

void File::PrintTree(std::ostream & os, std::vector<bool> & offsets) const{
	File::PrintOffset(os, offsets);
	Print(os);
}

void File::Print(std::ostream & os) const{
	os << path.filename() << '\n';
}

void File::PrintExtended(std::ostream & os) const{
	os << "Path: " << std::filesystem::absolute(path) << '\n';
	os << "Index: " << index << '\n';
	os << "Size: " << size << " B\n";
	// This will not work in C++20 since it introduces new chrono time specifically for file time
	// Up to that point we can use system_clock;
	std::time_t t = std::chrono::system_clock::to_time_t(last_mod);
	os << "Last Modification Time: " << std::put_time(std::gmtime(&t), DATE_FMT.c_str()) << '\n';
}

bool File::ComparePrint(const File & other, std::ostream & os) const{
	if(other.GetPath() != path || typeid(*this) != typeid(other)){
		os << std::string(SPACING, ' ')<<  "+ "; Print(os);
		os << std::string(SPACING, ' ')<<  "- "; other.Print(os);
		return false;
	}
	if(!this->CompareContent(other)){
		os << std::string(SPACING, ' ') << "MOD -- "; Print(os);
		return false;
	}
	return true;
}

bool File::CompareContent(const File & other) const{
	return size == other.size && other.last_mod == other.last_mod;
}

void File::SaveAsData(std::ofstream & ofs) const{
	SavePrimitive(ofs, index);
	SaveString(ofs, path.string());
	SavePrimitive(ofs, size);
	SaveTimeStamp(ofs, last_mod);
}

const std::filesystem::path & File::GetPath() const{
	return path;
}

const size_t & File::GetIndex() const{
	return index;
}

const size_t & File::GetSize() const{
	return size;
}

const std::filesystem::path File::GetFilename() const{
	return path.filename();
}

const std::filesystem::file_time_type & File::GetModTime() const{
	return last_mod;
}
//-------------------------------------------------------------------------
bool FileComparator::operator ()(const std::shared_ptr<File> & cmp1, const std::shared_ptr<File> & cmp2) const{
	if(cmp1.get() == nullptr)
		return false;
	if(cmp2.get() == nullptr)
		return false;
	if(cmp1->GetPath() >= cmp2->GetPath())
		return false;

	return true;
}
