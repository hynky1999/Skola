#pragma once

#include "Expression.h"
#include "vector"
#include <string>
#include <vector>

/**
 * @brief Class representing Parser of relational queries from terminal
 * A lot of inspiration was taken from here "https://nerdparadise.com/programming/parsemathexpr" and Antlr4 parser
 * Since I didn't quite have a time to implement a lexer, parser can only move on word objects,
 * so each object have to be separated by a space
 *
 */
class Parser{
	int index;
	std::vector< std::string > buffer;

public:

	Parser(std::vector<std::string> args);

	/**
	 * @brief Function to get a word on index
	 *
	 * @param index index of word
	 *
	 * @return word on index if out of bound throw logic error
	 */
	std::string & peek(int index);

	/**
	 * @brief Function to check if buffer is empty
	 *
	 * @return true if buffer is empty
	 */
	bool empty();

	/**
	 * @brief Returns parsed expression
	 *
	 * @return nullptr if parsing failed else an expression
	 */
	std::shared_ptr<BasicExpr> GetExpr();

	/**
	 * @brief Parsing Expr Wrapper
	 *
	 * @return expr parsed
	 */
	std::shared_ptr<BasicExpr> parseExpr();

	/**
	 * @brief Function parsing current index as AND operator
	 *
	 * @return expr with added AND operator
	 */
	std::shared_ptr<BasicExpr> parseAnd();

	/**
	 * @brief Function parsing current index as OR operator
	 *
	 * @return expr with added OR operator
	 */
	std::shared_ptr<BasicExpr> parseOr();
	/**
	 * @brief Function parsing current index as '(' and searching for closing ')'
	 *
	 * @return expr with parenthesis added
	 */
	std::shared_ptr<BasicExpr> parseParenthesis();
	/**
	 * @brief Function parsing current index as Rel operator
	 *
	 * @return expr with relation added throw exception if not supported
	 */
	std::shared_ptr<BasicExpr> parseRel();

	/**
	 * @brief Function parsing current index as query eg. size, filename....
	 *
	 * @return expr with query added
	 */
	std::shared_ptr<BasicExpr> parseQuery();

	/**
	 * @brief Function parsing multiple attoms separated by ','
	 *
	 * @return atoms cojoint together
	 */
	std::shared_ptr<BasicExpr> parseAtoms();
	/**
	 * @brief Function parsing "string"
	 *
	 * @return returns parsed string if any
	 */
	std::shared_ptr<BasicExpr> parseString();

	/**
	 * @brief 
	 *
	 * @return 
	 */
	std::shared_ptr<BasicExpr> parseDate();
	/**
	 * @brief Function parsing number
	 *
	 * @return number if any else throw exception
	 */
	std::shared_ptr<BasicExpr> parseNumber();
};
