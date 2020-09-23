#pragma once
#include "Expression.h"

/**
 * @brief Abstract class representing binary logic operator
 *
 */
class LogicOperator: public BasicExpr{
protected:
	std::shared_ptr<BasicExpr> op1;
	std::shared_ptr<BasicExpr> op2;
	LogicOperator(std::shared_ptr<BasicExpr> op1, std::shared_ptr<BasicExpr> op2);
};

/**
 * @brief Class representing AND function
 */
class And: public LogicOperator{
public:
	static const std::string LexicalAtom;
	And(std::shared_ptr<BasicExpr> op1, std::shared_ptr<BasicExpr> op2);
	bool Evaluate(const File & file) const override;
};

/**
 * @brief Class representing OR function
 */
class Or: public LogicOperator{
public:
	static const std::string LexicalAtom;
	Or(std::shared_ptr<BasicExpr> op1, std::shared_ptr<BasicExpr> op2);
	bool Evaluate(const File & file) const override;
};
