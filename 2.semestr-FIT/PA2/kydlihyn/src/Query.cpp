#include "Query.h"
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;

bool BaseQuery::Evaluate(const File & file) const{
	return true;
}


//Queries definitions

std::unordered_map< std::string, std::shared_ptr<BaseQuery>> QUERIES = {
	{"index", std::make_shared < Query< Atom < size_t >, File,
		const size_t & > > (&Atom < size_t >::getValue, &File::GetIndex)},
	{"type", std::make_shared < Query< Atom < std::string >, File,
		const std::string &, const std::string & > >(&Atom < std::string >::getValue, &File::GetType)},
	{"path", std::make_shared < Query< Atom < std::string >, File,
		const std::string &, const fs::path & > > (&Atom< std::string >::getValue, &File::GetPath)},
	{"filename", std::make_shared < Query< Atom < std::string >, File,
		const std::string &,  const fs::path> > (&Atom < std::string >::getValue, &File::GetFilename)},
	{"size", std::make_shared < Query < Atom < size_t > , File,
		const size_t & > > (&Atom< size_t >::getValue, &File::GetSize)},
	{"mod_time", std::make_shared < Query< Atom < fs::file_time_type >, File,
		const fs::file_time_type &,const fs::file_time_type &> > (&Atom < fs::file_time_type >::getValue,  &File::GetModTime)},
	{"txt_words", std::make_shared < QuerySubset< AtomsVector, Text_File,
		const std::vector< Atom < std::string > >, const std::vector< std::string > & > > (&AtomsVector::GetAsStrings, &Text_File::GetWords)},
	{"img_width", std::make_shared < Query< Atom < size_t >, Image_File,
		const size_t &, const uint32_t & > > (&Atom < size_t >::getValue, &Image_File::GetWidth)},
	{"img_height", std::make_shared < Query< Atom < size_t >, Image_File,
		const size_t &, const uint32_t & > > (&Atom < size_t >::getValue, &Image_File::GetHeight)},
	{"img_bit_depth", std::make_shared < Query< Atom < size_t >, Image_File,
		const size_t &, const uint16_t & > > (&Atom < size_t >::getValue, &Image_File::GetBitDepth)},
	{"jpg_description", std::make_shared < QuerySubset< AtomsVector, JPG_File,
		const std::vector< Atom < std::string > >, const std::vector< std::string > & > > (&AtomsVector::GetAsStrings, &JPG_File::GetDescription)},
	{"jpg_artist", std::make_shared < Query< Atom < std::string >, JPG_File,
		const std::string &, const std::string & > > (&Atom < std::string >::getValue, &JPG_File::GetArtist)},
	{"jpg_iso", std::make_shared < Query< Atom < size_t >, JPG_File,
		const size_t &, const uint32_t & > > (&Atom < size_t >::getValue, &JPG_File::GetIso)}
};
