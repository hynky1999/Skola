#include "Constants.hpp"

#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iterator>
#include <type_traits>


/**
 * @brief Utility function for saving timepoint to a file
 *
 * @param ofs output filestream to write to
 * @param tp timepoint to write to file
 */
void SaveTimeStamp(std::ofstream & ofs, const std::chrono::system_clock::time_point & tp);

/**
 * @brief Utility function for load timepoint from a file
 *
 * @param ifs input filestream to read
 * @param tp timepoint to load from a file
 */
void LoadTimeStamp(std::ifstream & ifs, std::chrono::system_clock::time_point & tp);

/**
 * @brief Utility function for saving string to a file
 *
 * @param ofs output filestream to write to
 * @param str string to write to file
 */
void SaveString(std::ofstream & ofs, const std::string & str);

/**
 * @brief Utility function for load string from a file
 *
 * @param ifs input filestream to read
 * @param str string to load from a file
 */
void LoadString(std::ifstream & ifs, std::string & str);

/**
 * @brief Utility function for saving primitive type to a file
 *
 * @tparam Type type of primitive
 *
 * @param ofs output filestream to write to
 * @param primitive primitive type to write to file
 */
template<typename Type>
void SavePrimitive(std::ofstream & ofs,const Type & primitive){
	ofs.write(reinterpret_cast<const char*>(&primitive), sizeof(primitive));
	if(!ofs.good())
		throw std::filesystem::filesystem_error(FILE_WRITE_ERR, std::make_error_code(std::errc::io_error));
}

/**
 * @brief Utility function for loading primitive from a file
 *
 * @tparam Type type of primitive
 *
 * @param ifs input filestream to read
 * @param primitive primitive to load from a file
 */
template<typename Type>
void LoadPrimitive(std::ifstream & ifs, Type & primitive){
	if(!ifs.good())
		throw std::filesystem::filesystem_error(FILE_WRITE_ERR, std::make_error_code(std::errc::io_error));
	ifs.read(reinterpret_cast<char *>(&primitive), sizeof(primitive));
}

/**
 * @brief Utility function for saving container with primitive value type to a file
 *
 * @tparam Type type of container
 *
 * @param ofs output filestream to write to
 * @param cnt container to write to file
 */
template<typename Type>
void SaveContainer(std::ofstream & ofs, const Type & cnt){
	SavePrimitive(ofs, cnt.size());
	for(auto it = cnt.cbegin(); it != cnt.cend(); ++it)
		if(std::is_same< typename Type::value_type, std::string >::value)
			SaveString(ofs, *it);
		else
			SavePrimitive(ofs, *it);
}

/**
 * @brief Utility function for loading container with primitive value type from a file
 *
 * @tparam Type type of container
 *
 * @param ifs input filestream to read
 * @param cnt container to load from a file
 */
template<typename Type>
void LoadContainer(std::ifstream & ifs, Type & cnt){
	typename Type::size_type size;
	LoadPrimitive(ifs, size);
	auto it = std::inserter(cnt, cnt.begin());
	for(; size > 0; --size)
		if(std::is_same< typename Type::value_type, std::string >::value){
			std::string str;
			LoadString(ifs, str);
			it = str;
		}
		else{
			typename Type::value_type tp;
			LoadPrimitive(ifs, tp);
			it = tp;
		}
}
