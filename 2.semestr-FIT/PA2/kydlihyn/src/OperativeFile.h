#include "File.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

/**
 * @brief Class representing Symbolic Link
 */
class Symbolic_Link: public File{
public:
	static const char type;
	std::shared_ptr<File> linked_file;

	Symbolic_Link(std::filesystem::path path, bool follow_sym_links = false);

	Symbolic_Link(std::ifstream & ifs);

	virtual void Print(std::ostream & os) const override;

	virtual void SaveAsData(std::ofstream & ofs) const override;

	const std::string & GetType() const override;
};

/**
 * @brief Class representing Block file
 */
class Block_File : public File{
public:
	static const char type;

	Block_File(std::filesystem::path path);

	Block_File(std::ifstream & ifs);

	virtual void Print(std::ostream & os) const override;

	virtual void SaveAsData(std::ofstream & ofs) const override;

	const std::string & GetType() const override;

};

/**
 * @brief Class representing Socket file
 */
class Socket_File : public File{
public:
	static const char type;

	Socket_File(std::filesystem::path path);

	Socket_File(std::ifstream & ifs);

	virtual void Print(std::ostream & os) const override;

	virtual void SaveAsData(std::ofstream & ofs) const override;

	const std::string & GetType() const override;
	
};

/**
 * @brief Class representing FIFO file
 */
class Fifo_File : public File{
public:
	static const char type;

	Fifo_File(std::filesystem::path path);

	Fifo_File(std::ifstream & ifs);

	void Print(std::ostream & os) const override;

	void SaveAsData(std::ofstream & ofs) const override;

	const std::string & GetType() const override;
};

/**
 * @brief Class representing Character file
 */
class Character_File : public File{
public:
	static const char type;

	Character_File(std::filesystem::path path);

	Character_File(std::ifstream & ifs);
	void Print(std::ostream & os) const override;

	void SaveAsData(std::ofstream & ofs) const override;

	const std::string & GetType() const override;
};
