#include "Index.h"
#include "Constants.hpp"

#include <unordered_set>
#include <iostream>
#include <ostream>
#include <functional>

Index::Index(){
	initialized = false;
}

Index::Index(std::filesystem::path path){
	Create(path);
}


std::ostream & operator << (std::ostream & os, Index & idx){
	std::vector<bool> offsets;
	idx.files_structure->PrintTree(os, offsets);
	return os;
}

bool Index::Save(const char * filename) const{
	std::ofstream ofs(filename, std::ios::binary);
	if(!ofs.is_open()){
		std::cerr << OPEN_ERR << " " << filename << std::endl;
		return false;
	}

	files_structure->SaveAsData(ofs);
	if(!ofs.good()){
		std::cerr << SAVE_ERR << " to " << filename << std::endl;
		return false;
	}
	std::cout << SAVE_SUC << " to " << filename << std::endl;
	return true;
}

bool Index::Load(const char * filename){
	std::ifstream ifs(filename, std::ios::binary);
	if(!ifs.is_open()){
		std::cerr << OPEN_ERR << " " << filename << std::endl;
		return false;
	}
	files_structure = File::CreateFile(ifs);
	if(files_structure.get() == nullptr){
		std::cerr << LOAD_ERR << " from " << filename << std::endl;
		return false;
	}
	std::cout << LOAD_SUC << " from " << filename <<  std::endl;
	initialized = true;
	return true;
}

bool Index::Create(std::filesystem::path path, bool follow_sym_links){
	std::unordered_set<std::string> files_visited;
	files_structure = File::CreateFile(path, files_visited, follow_sym_links);
	if(files_structure == nullptr){
		std::cerr << CREATE_ERR << " " << path << std::endl;
		return false;
	}
	initialized = true;
	std::cout << CREATE_SUC << " " << path << std::endl;
	return true;
}

bool Index::Compare(Index & other) const{
	std::cout << "Changes: " << '\n';
	std::cout << std::string(sep_line_length, sep_line_char) << '\n';
	bool res = files_structure->ComparePrint(*other.files_structure, std::cout);
	std::cout << std::string(sep_line_length, sep_line_char) << std::endl;
	return res;
}

bool Index::Search(BasicExpr & eval, std::function<void(const File&, std::ostream & )> f_print, bool include_dir) const{
	if(files_structure == nullptr){
		std::cerr << NO_FILES_ERR << std::endl;
		return false;
	}
	std::vector<std::shared_ptr<const File>> found;
	Directory* dir = dynamic_cast<Directory*>(files_structure.get());
	// Interpret index as single file
	if(dir == nullptr){
		if(eval.Evaluate(*files_structure))
			found.push_back(files_structure);
	}
	// Interpret index as directory
	else{
		for(auto it = dir->cbegin(true, include_dir); it != dir->cend(); ++it){
			if(eval.Evaluate(**it))
				found.push_back(*it);
		}
	}
	if(found.empty()){
		std::cout << "No files found" << std::endl;
		return false;
	}
	else{
		std::cout << "Files found:" << std::endl;
		for(auto it = found.cbegin(); it != found.cend(); ++it){
			if(f_print){
				f_print(**it, std::cout);
				std::cout << std::string(sep_line_length, sep_line_char) << '\n';
			}
		}
		std::cout << "Total " << found.size() << " files" << std::endl;
	}
	return true;
}

bool Index::Initialized() const{
	return initialized;
}
