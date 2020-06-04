#include "RegularFile.h"
#include "ExprParser.h"
#include "Atom.h"
#include "Query.h"
#include "Relation.h"
#include "LogicOperator.h"
#include "Constants.hpp"

#include <ctime>
#include <functional>
#include <iomanip>
#include <memory>
#include <string>
#include <set>
#include <iostream>
#include <cstring>

namespace fs = std::filesystem;
using fc = std::chrono::system_clock;

Parser::Parser(std::vector<std::string> args): index(0), buffer(args){
}

std::string & Parser::peek(int index){
	if(index < 0 || (size_t)index >= buffer.size())
		throw std::range_error(PARSING_RANGE_ERR);
	return buffer[index];
}

bool Parser::empty(){
	return buffer.size() <= (size_t)index;
}

std::shared_ptr< BasicExpr > Parser::GetExpr(){
	if(buffer.empty())
		return std::make_shared< Atom < bool > >(Atom < bool >(true));

	try{
		std::shared_ptr< BasicExpr > expr = parseExpr();
		if(!empty())
			throw std::overflow_error("Failed to parse all arguments");
		return expr;
	}
	catch(std::exception & e){
		std::cerr << "PARSER_ERROR: " << e.what() << " | " << "Error at index: " << index;
		if(index >= 0 && (size_t)index < buffer.size())
			std::cerr << " -> " << peek(index);
		else
			std::cerr << " -> Out of range";
		std::cerr << std::endl;

		return std::shared_ptr<BasicExpr>(nullptr);
	}
}

std::shared_ptr<BasicExpr> Parser::parseExpr(){
	return parseAnd();
}

std::shared_ptr<BasicExpr> Parser::parseAnd(){
	std::vector<std::shared_ptr<BasicExpr>> values;
	values.push_back(parseOr());
	while(!empty()){
		if(peek(index) == And::LexicalAtom){
			++index;
			values.push_back(parseOr());
		}
		else{
			break;
		}
	}

	auto it = values.rbegin();
	std::shared_ptr<BasicExpr> last = std::make_shared<Atom<bool>>(true);
	for(auto end = values.rend(); it != end; ++it){
		std::shared_ptr< BasicExpr > next_last = std::make_shared<And>(last, *it);
		last = next_last;
	}
	return last;
}

std::shared_ptr<BasicExpr> Parser::parseOr(){
	std::vector<std::shared_ptr<BasicExpr>> values;
	values.push_back(parseParenthesis());
	while(!empty()){
		if(peek(index) == Or::LexicalAtom){
			++index;
			values.push_back(parseParenthesis());
		}
		else{
			break;
		}
	}

	auto it = values.rbegin();
	std::shared_ptr<BasicExpr> last = std::make_shared<Atom<bool>>(false);
	for(auto end = values.rend(); it != end; ++it){
	std::shared_ptr<BasicExpr> next_last = std::make_shared<Or>(last, *it);
		last = next_last;
	}
	return last;
}

std::shared_ptr<BasicExpr> Parser::parseParenthesis(){
	if(peek(index) == "("){
		++index;
		std::shared_ptr<BasicExpr> expr = parseExpr();
		if(peek(index) != ")")
			throw std::logic_error("Expected ending ')'");
		++index;
		return expr;
	}
	else
		return parseRel();
}

std::shared_ptr<BasicExpr> Parser::parseRel(){
	std::shared_ptr<BasicExpr> op1 = parseQuery();
	if(peek(index) == LowerThan::LexicalAtom){
		++index;
		std::shared_ptr<BasicExpr> op2 = parseQuery();
		return std::make_shared<LowerThan>(op1, op2);
	}
	else if(peek(index) == GreaterThan::LexicalAtom){
		++index;
		std::shared_ptr<BasicExpr> op2 = parseQuery();
		return std::make_shared<GreaterThan>(op1, op2);
	}
	else if(peek(index) == Equal::LexicalAtom){
		++index;
		std::shared_ptr<BasicExpr> op2 = parseQuery();
		return std::make_shared<Equal>(op1, op2);
	}
	else if(peek(index) == NotEqual::LexicalAtom){
		++index;
		std::shared_ptr<BasicExpr> op2 = parseQuery();
		return std::make_shared<NotEqual>(op1, op2);
	}
	else if(peek(index) == LowerEqual::LexicalAtom){
		++index;
		std::shared_ptr<BasicExpr> op2 = parseQuery();
		return std::make_shared<LowerEqual>(op1, op2);
	}
	else if(peek(index) == GreaterEqual::LexicalAtom){
		++index;
		std::shared_ptr<BasicExpr> op2 = parseQuery();
		return std::make_shared<GreaterEqual>(op1, op2);
	}
	throw std::logic_error("Failed to parse relation");
}

std::shared_ptr<BasicExpr> Parser::parseQuery(){
	auto it = QUERIES.find(peek(index));
	if(it != QUERIES.end()){
		++index;
		return it->second;
	}

	return parseAtoms();
}

std::shared_ptr<BasicExpr> Parser::parseAtoms(){
	std::vector< std::shared_ptr< BasicExpr > > values;
	values.push_back(parseString());
	while(!empty()){
		if(peek(index) == ","){
			++index;
			values.push_back(parseString());
		}
		else{
			break;
		}
	}
	if(values.size() == 1)
		return values.back();
	
	return std::make_shared< AtomsVector >(values);
}

std::shared_ptr<BasicExpr> Parser::parseString(){
	if(peek(index).size() >= 2 && peek(index)[0] == '"' && peek(index).back() == '"'){
		std::string str = peek(index).substr(1, peek(index).size()-2);
		++index;
		return std::make_shared<Atom<std::string>>(str);
	}
	return parseDate();
}

std::shared_ptr<BasicExpr> Parser::parseDate(){
	struct std::tm tm;
	std::string str = peek(index);
	memset(&tm, 0, sizeof(struct tm));
	char* not_processed = strptime(str.c_str(), DATE_FMT.c_str(), &tm);
	if(not_processed == NULL || *not_processed != '\0'){
		return parseNumber();
	}
	tm.tm_isdst = -1;
	std::time_t time = std::mktime(&tm);
	if(time == -1)
		return parseNumber();
	++index;
	return std::make_shared< Atom< fc::system_clock::time_point > >(fc::system_clock::from_time_t(time));
}

std::shared_ptr<BasicExpr> Parser::parseNumber(){
	std::string numStr = peek(index);
	auto num = std::stoll(numStr);
	if(num < 0){
		std::cerr << "Warning: Converting " << peek(index) << " to unsgined\n";
	}
	++index;
	return std::make_shared< Atom < size_t > >(num);
}
