#include "OperativeFile.h"
#include "SaveUtilities.h"

const char Symbolic_Link::type = 20;

Symbolic_Link::Symbolic_Link(std::ifstream & ifs): File(ifs){}

Symbolic_Link::Symbolic_Link(std::filesystem::path path, bool follow_sym_links): File(path){}

void Symbolic_Link::SaveAsData(std::ofstream & ofs) const{
	SavePrimitive(ofs, type);
	File::SaveAsData(ofs);
}

void Symbolic_Link::Print(std::ostream & os) const{
		os << "[SYMLINK] "; File::Print(os);
}

const std::string & Symbolic_Link::GetType() const{
	return SYMLINK_NAME;
}

//-----------------------------------------------------------------

const char Block_File::type = 21;

Block_File::Block_File(std::ifstream & ifs): File(ifs){}

Block_File::Block_File(std::filesystem::path path): File(path){}

void Block_File::SaveAsData(std::ofstream & ofs) const{
	SavePrimitive(ofs, type);
	File::SaveAsData(ofs);
}

void Block_File::Print(std::ostream & os) const{
		os << "[BLOCK] "; File::Print(os);
}

const std::string & Block_File::GetType() const{
	return BLOCK_FILE_NAME;
}

//-----------------------------------------------------------------

const char Socket_File::type = 22;

Socket_File::Socket_File(std::ifstream & ifs): File(ifs){}

Socket_File::Socket_File(std::filesystem::path path): File(path){}

void Socket_File::SaveAsData(std::ofstream & ofs) const{
	SavePrimitive(ofs, type);
	File::SaveAsData(ofs);
}

void Socket_File::Print(std::ostream & os) const{
		os << "[SOCK] "; File::Print(os);
}

const std::string & Socket_File::GetType() const{
	return SOCKET_NAME;
}

//-----------------------------------------------------------------

const char Fifo_File::type = 23;

Fifo_File::Fifo_File(std::ifstream & ifs): File(ifs){}

Fifo_File::Fifo_File(std::filesystem::path path): File(path){}

void Fifo_File::SaveAsData(std::ofstream & ofs) const{
	SavePrimitive(ofs, type);
	File::SaveAsData(ofs);
}

void Fifo_File::Print(std::ostream & os) const{
		os << "[FIFO] "; File::Print(os);
}

const std::string & Fifo_File::GetType() const{
	return FIFO_NAME;
}

//-----------------------------------------------------------------
//
const char Character_File::type = 24;

Character_File::Character_File(std::ifstream & ifs): File(ifs){}

Character_File::Character_File(std::filesystem::path path): File(path){
}

void Character_File::SaveAsData(std::ofstream & ofs) const{
	SavePrimitive(ofs, type);
	File::SaveAsData(ofs);
}

void Character_File::Print(std::ostream & os) const{
		os << "[CHAR] "; File::Print(os);
}

const std::string & Character_File::GetType() const{
	return CHARACTER_FILE_NAME;
}
