#pragma once
#include "File.h"

#include <vector>
#include <ostream>
#include <filesystem>
#include <string>

/**
 * @brief Class representing Binary File, its default for all files
 */
class Binary_File: public File{
public:
	static const char type;

	Binary_File(std::filesystem::path path);

	Binary_File(std::ifstream & ifs);

	void Print(std::ostream & os) const override;

	virtual void SaveAsData(std::ofstream & ofs) const override;

	const std::string & GetType() const override;
};

/**
 * @brief Class representing Text File
 *
 */
class Text_File: public File{
private:
	std::vector <std::string> words;

public:
	static const char type;

	Text_File(std::filesystem::path path);

	Text_File(std::ifstream & ifs);

	void Print(std::ostream & os) const override;

	void PrintExtended(std::ostream & os) const override;

	bool CompareContent(const File & other) const override;

	void SaveAsData(std::ofstream & ofs) const override;

	const std::vector< std::string > & GetWords() const;

	const std::string & GetType() const override;
};


/**
 * @brief Abstract Class representing Image Files
 *
 */
class Image_File: public File{
protected:
	uint32_t width, height;
	uint16_t bit_depth;

public:
	Image_File(std::filesystem::path path);

	Image_File(std::ifstream & ifs);

	virtual void Print(std::ostream & os) const override;

	void PrintExtended(std::ostream & os) const override;

	bool CompareContent(const File & other) const override;

	virtual void SaveAsData(std::ofstream & ofs) const override;

	const uint32_t & GetWidth() const;

	const uint32_t & GetHeight() const;

	const uint16_t & GetBitDepth() const;
};


/**
 * @brief Class representing JPG File
 *
 */
class JPG_File: public Image_File{
private:
	uint32_t iso;
	std::string artist;
	std::vector< std::string > description;

public:
	static const char type;

	JPG_File(std::ifstream & ifs);

	JPG_File(std::filesystem::path path);

	void PrintExtended(std::ostream & os) const override;

	bool CompareContent(const File & other) const override;

	void SaveAsData(std::ofstream & ofs) const override;

	const uint32_t & GetIso() const;

	const std::string & GetArtist() const;

	const std::vector < std::string > & GetDescription() const;

	const std::string & GetType() const override;
};

/**
 * @brief Class representing PNG File
 *
 */
class PNG_File: public Image_File{
private:
	int color_type;
	
public:
	static const char type;

	PNG_File(std::ifstream & ifs);

	PNG_File(std::filesystem::path path);

	void PrintExtended(std::ostream & os) const override;

	bool CompareContent(const File & other) const override;

	void SaveAsData(std::ofstream & ofs) const override;

	const std::string & GetType() const override;
};
