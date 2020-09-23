#include "DirectoryIterator.h"
#include "Directory.h"
#include <memory>
#include <typeinfo>
#include <iostream>

DirectoryIterator::DirectoryIterator() {}

DirectoryIterator::DirectoryIterator(std::multiset<std::shared_ptr<File>, FileComparator> & set,
		bool reverse, bool recursive, bool includeDir): recursive(recursive), includeDir(includeDir){
	if(reverse)
		for(auto it = set.cbegin(); it != set.cend(); ++it)
			files.push_back(*it);
	else
		for(auto it = set.crbegin(); it != set.crend(); ++it)
			files.push_back(*it);
	if(!includeDir && files.size() != 0 && typeid(*files.back()) == typeid(Directory)){
		// Skip the directories
		operator ++ ();
	}
}

DirectoryIterator & DirectoryIterator::operator ++ (){
	std::shared_ptr<File> last = files.back(); files.pop_back();
	if(typeid(*last) == typeid(Directory) && recursive){
		// Add directories content
		Directory* last_d = dynamic_cast<Directory*>(last.get());
		for(DirectoryIterator it = last_d->begin(false, true); it != last_d->end(); ++it)
			files.push_back(*it);
	}
	if(!includeDir && files.size() != 0 && typeid(*files.back()) == typeid(Directory)){
		// Skip the directories
		return this-> operator ++ ();
	}
	return *this;
}

std::shared_ptr<File> & DirectoryIterator::operator * (){
	return files.back();
}
bool DirectoryIterator::operator != (const DirectoryIterator & other) const {
	// Both iterators are nullptr
	if(other.files.size() == 0 && this->files.size() == 0)
		return false;
	if(other.files.size() == 0 || this->files.size() == 0)
		return true;

	return other.files.back() != this->files.back();
}
//Const version------------------------------------------------------------------------------------

DirectoryIteratorConst::DirectoryIteratorConst() {}

DirectoryIteratorConst::DirectoryIteratorConst(const std::multiset<std::shared_ptr<File>, FileComparator> & set,
		bool reverse, bool recursive, bool includeDir): recursive(recursive), includeDir(includeDir){
	if(reverse)
		for(auto it = set.cbegin(); it != set.cend(); ++it)
			files.push_back(*it);
	else
		for(auto it = set.crbegin(); it != set.crend(); ++it)
			files.push_back(*it);
	if(!includeDir && files.size() != 0 && typeid(*files.back()) == typeid(Directory)){
		// Skip the directories
		operator ++ ();
	}
}

DirectoryIteratorConst & DirectoryIteratorConst::operator ++ (){
	std::shared_ptr<const File> last = files.back(); files.pop_back();
	if(typeid(*last) == typeid(Directory) && recursive){
		// Add directories content
		const Directory * last_d = dynamic_cast<const Directory*>(last.get());
		for(auto it = last_d->crbegin(false, true); it != last_d->crend(); ++it)
			files.push_back(*it);
	}
	if(!includeDir && files.size() != 0 && typeid(*files.back()) == typeid(Directory)){
		// Skip the directories
		return this-> operator ++ ();
	}
	return *this;
}

const std::shared_ptr<const File> & DirectoryIteratorConst::operator * () const{
	return files.back();
}

bool DirectoryIteratorConst::operator != (const DirectoryIteratorConst & other) const {
	// Both iterators are nullptr
	if(other.files.size() == 0 && this->files.size() == 0)
		return false;
	if(other.files.size() == 0 || this->files.size() == 0)
		return true;

	return other.files.back() != this->files.back();
}
