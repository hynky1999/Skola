#include <png.h>
void StreamReadData(png_structp png_ptr, png_bytep data, png_size_t length);
void ErrorCallback(png_structp png_ptr, png_const_charp error_msg);
