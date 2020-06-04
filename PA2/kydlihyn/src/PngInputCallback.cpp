#include <fstream>
#include <functional>
#include <stdexcept>
#include "PngInputCallback.h"
void StreamReadData(png_structp png_ptr, png_bytep data, png_size_t length){
	// Used some info from
	// http://www.piko3d.net/tutorials/libpng-tutorial-loading-png-files-from-streams/
	png_voidp stream = png_get_io_ptr(png_ptr);
	((std::ifstream*)stream)->read(reinterpret_cast<char *>(data), length);
}
void ErrorCallback(png_structp png_ptr, png_const_charp error_msg){
	throw std::domain_error(error_msg);
}
