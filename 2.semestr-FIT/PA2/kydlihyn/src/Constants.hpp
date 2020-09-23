#pragma once
#include <string>
#include <vector>
#include <filesystem>

// Error Messages
const std::string SAVE_ERR = "Failed to save index";
const std::string LOAD_ERR = "Failed to load index";
const std::string CREATE_ERR = "Failed to create index";
const std::string OPEN_ERR = "Could not open file";
const std::string NO_FILES_ERR = "No files in index";
const std::string FACTORY_CREATE_ERR = "Error creating file";
const std::string FILE_READ_ERR = "Error while reading a file";
const std::string FILE_WRITE_ERR = "Error while writing to a file";
const std::string PARSING_ERR = "Error when parsing an expression";
const std::string PARSING_RANGE_ERR = "Expected a word";
const std::string EVAL_ERR = "Error when evaluating an expression";
const std::string NO_INDEX_ERR = "No index was specified";
const std::string INDEX_INIT_ERR = "Failed to initialize index";
const std::string ARGUMENTS_MULTIPLE_ERR = "Error Too many arguments";
const std::string AMBIGUOUS_ARGUMENTS_ERR = "Invalid combination of options";

// Success Messages
const std::string SAVE_SUC = "Successfully saved index";
const std::string LOAD_SUC = "Successfully loaded index";
const std::string CREATE_SUC = "Successfully created index";
const std::string PRINT_HELP = " USE OF PROGRAM:\
																\n--create(-c) filename creates index from filename\
																\n--save(-s) filename saves created index as filename\
																\n--load(-l) filename loads index from filename\
																\n--compare(-C) filename compares loaded index against index save in filename\
																\n--search(-Q) query query over loaded index\
																\n--print(-p) prints query as tree structure optonal(e) -> extended print(works only in search)\
																\n--include_dir(-d) flag to include directory to search\
																\n--follow_sym_links(-f) flag to follow symlinks when creating index\
																\n--help(-h) help message";


// Extensions
const std::vector<std::filesystem::path> PNG_EXTS{".png"};
const std::vector<std::filesystem::path> JPG_EXTS{".jpg", ".jpeg", ".jpe", ".jif", ".jfif", ".jfi"};
const std::vector<std::filesystem::path> TEXT_EXTS{".txt"};

//Commands
const std::string CREATE_CMD = "create";
const char CREATE_CMD_SHORT = 'c';
const std::string HELP_CMD = "help";
const char HELP_CMD_SHORT = 'h';
const std::string LOAD_CMD = "load";
const char LOAD_CMD_SHORT = 'l';
const std::string SAVE_CMD = "save";
const char SAVE_CMD_SHORT = 's';
const std::string SEARCH_CMD = "search";
const char SEARCH_CMD_SHORT = 'Q';
const std::string PRINT_CMD = "print";
const char PRINT_CMD_SHORT = 'p';
const std::string COMPARE_CMD = "compare";
const char COMPARE_CMD_SHORT = 'C';
const std::string INCLUDE_DIR_CMD = "include_dir";
const char INCLUDE_DIR_CMD_SHORT = 'd';
const std::string FOLLOW_SYM_CMD = "follow_sym_links";
const char FOLLOW_SYM_CMD_SHORT = 'f';
const std::string OPTS_STRING = "c:hl:s:p::C:Qdf";


//File Type names
const std::string DIRECTORY_NAME = "DIRECTORY";
const std::string TEXT_FILE_NAME = "TEXT_FILE";
const std::string JPG_FILE_NAME = "JPG_FILE";
const std::string PNG_FILE_NAME = "PNG_FILE";
const std::string BINARY_FILE_NAME = "BINARY_FILE";
const std::string SYMLINK_NAME = "SYMBOLIC_LINK";
const std::string BLOCK_FILE_NAME = "BLOC_FILE";
const std::string SOCKET_NAME = "SOCKET";
const std::string FIFO_NAME = "FIFO";
const std::string CHARACTER_FILE_NAME = "CHARACTER_FILE";

// Other constants
const int SPACING = 3;
const size_t PNG_HEADER_SIZE = 8;
const size_t MAX_WORDS = 20;
const size_t MAX_DATE_LENGTH = 16;
const std::string DEFAULT_INDEX_FILENAME = "index.bin";
const std::string DEFAULT_PRINT = "b";
const uint8_t sep_line_length = 60;
const uint8_t sep_line_char = '-';
const std::string DATE_FMT = "%d:%m:%Y %H:%M:%S";
