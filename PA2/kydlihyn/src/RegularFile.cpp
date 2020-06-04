#include "RegularFile.h"
#include "Constants.hpp"
#include "SaveUtilities.h"
#include "PngInputCallback.h"

#include <exception>
#include <png.h>
#include <exiv2/exiv2.hpp>
#include <stdexcept>
#include <string>
#include <system_error>

const char Binary_File::type = 1;

Binary_File::Binary_File(std::ifstream & ifs): File(ifs){}

Binary_File::Binary_File(std::filesystem::path path): File(path, std::filesystem::file_size(path), std::filesystem::last_write_time(path)){}

void Binary_File::SaveAsData(std::ofstream & ofs) const{
	SavePrimitive(ofs, type);
	File::SaveAsData(ofs);
}

void Binary_File::Print(std::ostream & os) const{
		os << "[BIN] "; File::Print(os);
}

const std::string & Binary_File::GetType() const{
	return BINARY_FILE_NAME;
}

//-----------------------------------------------------------------
const char Text_File::type = 2;

Text_File::Text_File(std::ifstream & ifs): File(ifs){
	std::vector<std::string>::size_type words_size;
	LoadPrimitive(ifs, words_size);
	LoadContainer(ifs, words);
}

Text_File::Text_File(std::filesystem::path path): File(path, std::filesystem::file_size(path), std::filesystem::last_write_time(path)){
	std::ifstream ifs(path);
	std::string word;
	while(ifs.good()){
		ifs >> word;
		if(ifs.good()){
			words.push_back(word);
		}
	}
	if(!ifs.eof())
		throw std::filesystem::filesystem_error(FILE_READ_ERR, path, std::make_error_code(std::errc::io_error));
}

void Text_File::Print(std::ostream & os) const{
		os << "[TXT] "; File::Print(os);
}

void Text_File::PrintExtended(std::ostream & os) const{
	File::PrintExtended(os);
	os << "WORDS (" << words.size() << ")\n";
	if(words.size() > MAX_WORDS || words.size() == 0)
		return;
	auto it = words.begin();
	for(auto end = --words.end(); it != end; ++it)
		os << *it << ", ";
	os << *it << '\n';
}

bool Text_File::CompareContent(const File & other) const{
	const Text_File* txt = dynamic_cast<const Text_File*>(&other);
	if(txt == nullptr)
		return false;
	return words == txt->words && File::CompareContent(other);
}

void Text_File::SaveAsData(std::ofstream & ofs) const{
	SavePrimitive(ofs, type);
	File::SaveAsData(ofs);
	SavePrimitive(ofs, words.size());
	SaveContainer(ofs, words);
}

const std::vector< std::string > & Text_File::GetWords() const{
	return words;
}

const std::string & Text_File::GetType() const{
	return TEXT_FILE_NAME;
}

//-----------------------------------------------------------------
Image_File::Image_File(std::filesystem::path path): File(path, std::filesystem::file_size(path), std::filesystem::last_write_time(path)), width(0), height(0), bit_depth(0){}

Image_File::Image_File(std::ifstream & ifs): File(ifs){
	LoadPrimitive(ifs, width);
	LoadPrimitive(ifs, height);
	LoadPrimitive(ifs, bit_depth);
}

void Image_File::Print(std::ostream & os) const{
		os << "[IMG] "; File::Print(os);
}

void Image_File::PrintExtended(std::ostream & os) const{
	File::PrintExtended(os);
	os << "Dimensions: " << width << " x " << height << " px\n";
	os << "Bit depth: " << bit_depth << " bit\n";
}

bool Image_File::CompareContent(const File & other) const{
	const Image_File* img = dynamic_cast<const Image_File*>(&other);
	if(img == nullptr)
		return false;
	return width == img->width && height == img->height && bit_depth == img->bit_depth && File::CompareContent(other);
}

void Image_File::SaveAsData(std::ofstream & ofs) const{
	File::SaveAsData(ofs);
	SavePrimitive(ofs, width);
	SavePrimitive(ofs, height);
	SavePrimitive(ofs, bit_depth);
}

const uint32_t & Image_File::GetWidth() const{
	return width;
}

const uint32_t & Image_File::GetHeight() const{
	return height;
}

const uint16_t & Image_File::GetBitDepth() const{
	return bit_depth;
}
//-----------------------------------------------------------------
const char PNG_File::type = 3;

PNG_File::PNG_File(std::ifstream & ifs): Image_File(ifs){
	LoadPrimitive(ifs, color_type);
}

PNG_File::PNG_File(std::filesystem::path path): Image_File(path){
	std::ifstream ifs(path, std::ios_base::binary);

	unsigned char header[PNG_HEADER_SIZE] = {0};
	ifs.read(reinterpret_cast<char*> (header), PNG_HEADER_SIZE);
	if(png_sig_cmp(header, 0, PNG_HEADER_SIZE) || !ifs.good()){
		throw std::domain_error(FILE_READ_ERR);
	}
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, ErrorCallback, NULL);
	if(png_ptr == NULL){
		throw std::domain_error(FILE_READ_ERR);
	}
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == NULL){
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		throw std::domain_error(FILE_READ_ERR);
	}

	png_set_sig_bytes(png_ptr, PNG_HEADER_SIZE);
	png_set_read_fn(png_ptr,(png_voidp)&ifs, StreamReadData);
	png_read_info(png_ptr, info_ptr);

	// Read whatever we need here for more function look here -> http://www.libpng.org/pub/png/libpng-1.2.5-manual.html
	//
	int tmp_depth;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &tmp_depth, &color_type, NULL, NULL, NULL);
	bit_depth = tmp_depth;
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

void PNG_File::PrintExtended(std::ostream & os) const{
	Image_File::PrintExtended(os);
	os << "Color type: " << color_type << '\n';
}

bool PNG_File::CompareContent(const File & other) const{
	const PNG_File* png = dynamic_cast<const PNG_File*>(&other);
	if(png == nullptr)
		return false;
	return color_type == png->color_type && Image_File::CompareContent(other);
}

void PNG_File::SaveAsData(std::ofstream & ofs) const{
	SavePrimitive(ofs, type);
	Image_File::SaveAsData(ofs);
	SavePrimitive(ofs, color_type);
}

const std::string & PNG_File::GetType() const{
	return PNG_FILE_NAME;
}
//-----------------------------------------------------------------
const char JPG_File::type = 4;

std::string LoadTagStr(Exiv2::ExifData & data, std::string exif){
		auto it = data.findKey(Exiv2::ExifKey(exif));
		if(it == data.end())
			return std::string();
		else{
			return it->value().toString();
		}
}

long LoadTagLong(Exiv2::ExifData & data, std::string exif){
		auto it = data.findKey(Exiv2::ExifKey(exif));
		if(it == data.end())
			return 0;
		else{
			return it->value().toLong();
		}
}

JPG_File::JPG_File(std::ifstream & ifs): Image_File(ifs){
	LoadPrimitive(ifs, iso);
	LoadContainer(ifs, description);
	LoadString(ifs, artist);
}

JPG_File::JPG_File(std::filesystem::path path) : Image_File(path){
	auto image = Exiv2::ImageFactory::open(path);
	if(image.get() == nullptr)
		throw std::filesystem::filesystem_error(FILE_READ_ERR, path, std::make_error_code(std::errc::io_error));
	image->readMetadata();
	width = image->pixelWidth();
	height = image->pixelHeight();
	auto &exifData = image->exifData();
	if (!exifData.empty()) {
		iso = LoadTagLong(exifData, "Exif.Image.ISOSpeedRatings");
		bit_depth = LoadTagLong(exifData, "Exif.Image.BitsPerSample");
		std::istringstream desc(LoadTagStr(exifData, "Exif.Image.ImageDescription"));
		while(desc.good()){
			std::string desc_sgl; desc >> desc_sgl;
			if(desc.good())
				description.push_back(desc_sgl);
		}
		artist = LoadTagStr(exifData, "Exif.Image.Artist");
	}
}

void JPG_File::PrintExtended(std::ostream & os) const{
	Image_File::PrintExtended(os);
	os << "ISO: ISO " << iso << '\n';
	os << "Artist: " << artist << '\n';
	if(description.size() != 0){
		os << "Description: ";
		auto it = description.begin();
		for(auto end = --description.end(); it != end; ++it)
			os << *it << " ";
		os << *it << '\n';
	}
}

bool JPG_File::CompareContent(const File & other) const{
	const JPG_File* jpg = dynamic_cast<const JPG_File*>(&other);
	if(jpg == nullptr)
		return false;
	return iso == jpg->iso && description == jpg->description && artist == jpg->artist && Image_File::CompareContent(other);
}

void JPG_File::SaveAsData(std::ofstream & ofs) const{
	SavePrimitive(ofs, type);
	Image_File::SaveAsData(ofs);
	SavePrimitive(ofs, iso);
	SaveContainer(ofs, description);
	SaveString(ofs, artist);
}

const uint32_t & JPG_File::GetIso() const{
	return iso;
}

const  std::string & JPG_File::GetArtist() const{
	return artist;
}

const std::vector < std::string > & JPG_File::GetDescription() const{
	return description;
}

const std::string & JPG_File::GetType() const{
	return JPG_FILE_NAME;
}
