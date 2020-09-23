#include "LogicOperator.h"

LogicOperator::LogicOperator(std::shared_ptr<BasicExpr> op1, std::shared_ptr<BasicExpr> op2): op1(op1), op2(op2){}

const std::string And::LexicalAtom = "AND";
And::And(std::shared_ptr<BasicExpr> op1, std::shared_ptr<BasicExpr> op2): LogicOperator(op1, op2){}

bool And::Evaluate(const File & file) const{
	return op1->Evaluate(file) && op2->Evaluate(file);
}

const std::string Or::LexicalAtom = "OR";
Or::Or(std::shared_ptr<BasicExpr> op1, std::shared_ptr<BasicExpr> op2): LogicOperator(op1, op2){}

bool Or::Evaluate(const File & file) const{
	return op1->Evaluate(file) || op2->Evaluate(file);
}

