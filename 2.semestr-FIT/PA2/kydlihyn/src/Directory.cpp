#include "Directory.h"
#include "SaveUtilities.h"
#include "Constants.hpp"

#include <iterator>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_set>


const char Directory::type = 0;

Directory::Directory(std::filesystem::path path, std::unordered_set<std::string> & files_visited, bool follow_sym_links): File(path){
	//If file contains no size set it to epoch
	last_mod = last_mod.min();
	// Compute earliest mod time and accumulative of sizes
	for(const auto & file_path : std::filesystem::directory_iterator(path)){
		auto file = File::CreateFile(file_path, files_visited, follow_sym_links);
		if(file.get() != nullptr){
			files.insert(file);
			max(last_mod, file->GetModTime());
			size += file->GetSize();
		}
	}
}

Directory::Directory(std::ifstream & ifs): File(ifs){
	size_t files_size;
	LoadPrimitive(ifs, files_size);
	for(size_t i = 0; i < files_size; ++i){
		auto file = File::CreateFile(ifs);
		if(file.get() == nullptr)
			throw std::filesystem::filesystem_error(FILE_WRITE_ERR, std::make_error_code(std::errc::io_error));

		files.insert(file);
	}
}

void Directory::PrintTree(std::ostream & os, std::vector<bool> & offsets) const{
	File::PrintOffset(os, offsets);
	Print(os);
	if(files.size() == 0)
		return;
	offsets.push_back(true);
	auto file_it = files.cbegin();
	for(auto end = --files.cend(); file_it != end; ++file_it){
		file_it->get()->PrintTree(os, offsets);
	}
	// Change offset value before printing last element to get '\'
	offsets.back() = false;
	file_it->get()->PrintTree(os, offsets);
	offsets.pop_back();
}

void Directory::Print(std::ostream & os) const{
	os << "[DIR] "; File::Print(os);
}

void Directory::PrintExtended(std::ostream & os) const{
	File::PrintExtended(os);
	os << "Files: (" << files.size() << ")\n";
	for(auto it = files.cbegin(); it != files.cend(); ++it)
		(*it)->Print(os);
}

void Directory::SaveAsData(std::ofstream & ofs) const{
	SavePrimitive(ofs, type);
	File::SaveAsData(ofs);
	SavePrimitive(ofs, files.size());
	for(const auto & elem : files)
		elem.get()->SaveAsData(ofs);
}

template<typename Comparator>
// Took inspiration from std::set_intersection
// Because containers are sorted if front(container1) < front(container2) ->front(container1) is not in intersection
// if front(container1) > front(container2) ->front(container2) is not in intersection
// else add it to intersection

void intersection(
	std::multiset<std::shared_ptr<File>>::const_iterator first1,
	std::multiset<std::shared_ptr<File>>::const_iterator last1,
	std::multiset<std::shared_ptr<File>>::const_iterator first2,
	std::multiset<std::shared_ptr<File>>::const_iterator last2,
	std::vector<std::shared_ptr<File>> & difference_this,
	std::vector<std::shared_ptr<File>> & difference_other,
	std::vector<std::shared_ptr<File>> & intersect_this,
	std::vector<std::shared_ptr<File>> & intersect_other,
	Comparator Cmp
	){
	while (first1 != last1 && first2 != last2) {
		if (Cmp(*first1, *first2)) {
			difference_this.push_back(*first1++);
		}
		else {
			if (!Cmp(*first2, *first1)) {
					intersect_this.push_back(*first1++);
					intersect_other.push_back(*first2++);
			}
			else{
				difference_other.push_back(*first2++);
			}
		}
	}
	for(;first1 != last1; ++first1) {
		difference_this.push_back(*first1);
	}
	for(;first2 != last2; ++first2) {
		difference_other.push_back(*first2);
	}
}

bool Directory::ComparePrint(const File & other, std::ostream & os) const{
	if(typeid(other) != typeid(*this) || other.GetPath() != this->GetPath()){
		os << "+ "; Print(os);
		os << "- "; other.Print(os);
		return false;
	}

	const Directory & dir_other = dynamic_cast<const Directory &>(other);
	std::vector<std::shared_ptr<File>> difference_this;
	std::vector<std::shared_ptr<File>> difference_other;
	std::vector<std::shared_ptr<File>> intersect_this;
	std::vector<std::shared_ptr<File>> intersect_other;
	intersection(files.cbegin(), files.cend(), dir_other.files.cbegin(), dir_other.files.cend(),
			difference_this, difference_other, intersect_this, intersect_other, FileComparator());

	os << path <<  ":" << '\n';

	for(const auto & file : difference_this){
		os << std::string(SPACING, ' ') << "+ "; file->Print(os);
	}

	for(const auto & file : difference_other){
		os << std::string(SPACING, ' ') << "- "; file->Print(os);
	}
	bool res = true;
	auto it_this = intersect_this.cbegin();
	auto it_other = intersect_other.cbegin();
	for(; it_this != intersect_this.end(); ++it_this, ++it_other){
			res &= (*it_this)->ComparePrint(**it_other, os);
	}
	res &= !difference_this.empty() & !difference_other.empty();
	return res;
}

bool Directory::CompareContent(const File & other) const{
	const Directory & dir_other = dynamic_cast<const Directory &>(other);
	std::vector<std::shared_ptr<File>> difference_this;
	std::vector<std::shared_ptr<File>> difference_other;
	std::vector<std::shared_ptr<File>> intersect_this;
	std::vector<std::shared_ptr<File>> intersect_other;
	intersection(files.cbegin(), files.cend(), dir_other.files.cbegin(), dir_other.files.cend(),
			difference_this, difference_other, intersect_this, intersect_other, FileComparator());

	bool res = true;
	auto it_this = intersect_this.cbegin();
	auto it_other = intersect_other.cbegin();
	for(; it_this != intersect_this.end(); ++it_this, ++it_other){
			res &= (*it_this)->CompareContent(**it_other);
	}
	res &= !difference_this.empty() & !difference_other.empty();
	return res;
}

const std::string & Directory::GetType() const{
	return DIRECTORY_NAME;
}

DirectoryIterator Directory::begin(bool recursive, bool includeDir){
	return DirectoryIterator(files, false, recursive, includeDir);
}
DirectoryIterator Directory::end(){
	return DirectoryIterator();
}

DirectoryIteratorConst Directory::cbegin(bool recursive, bool includeDir) const {
	return DirectoryIteratorConst(files, false, recursive, includeDir);
}
DirectoryIteratorConst Directory::cend() const{
	return DirectoryIteratorConst();
}
DirectoryIterator Directory::rbegin(bool recursive, bool includeDir){
	return DirectoryIterator(files, true, recursive, includeDir);
}
DirectoryIterator Directory::rend(){
	return DirectoryIterator();
}
DirectoryIteratorConst Directory::crbegin(bool recursive, bool includeDir) const{
	return DirectoryIteratorConst(files, true, recursive, includeDir);
}
DirectoryIteratorConst Directory::crend() const{
	return DirectoryIteratorConst();
}
