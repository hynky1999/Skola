#include "Relation.h"
#include "Constants.hpp"
#include <memory>

Relation::Relation(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom): atom(atom){
	auto query_cast = std::dynamic_pointer_cast<BaseQuery>(query);
	if(query_cast.get() == nullptr)
		throw std::logic_error(PARSING_ERR + ": Expected query on left side" );
	this->query = query_cast;
}
	
const std::string LowerThan::LexicalAtom = "<";

LowerThan::LowerThan(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom): Relation(query, atom){}

bool LowerThan::Evaluate(const File & file) const{
	return query->Lt(file, atom);
}

const std::string GreaterThan::LexicalAtom = ">";

GreaterThan::GreaterThan(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom): Relation(query, atom){}

bool GreaterThan::Evaluate(const File & file) const{
	return !query->Lt(file, atom) && !query->Eq(file, atom);
}

const std::string Equal::LexicalAtom = "==";

Equal::Equal(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom): Relation(query, atom){}

bool Equal::Evaluate(const File & file) const{
	return query->Eq(file, atom);
}

const std::string NotEqual::LexicalAtom = "!=";

NotEqual::NotEqual(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom): Relation(query, atom){}

bool NotEqual::Evaluate(const File & file) const{
	return !query->Eq(file, atom);
}

const std::string LowerEqual::LexicalAtom = "<=";

LowerEqual::LowerEqual(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom): Relation(query, atom){}

bool LowerEqual::Evaluate(const File & file) const{
	return query->Lt(file, atom) || query->Eq(file, atom);
}

const std::string GreaterEqual::LexicalAtom = ">=";

GreaterEqual::GreaterEqual(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom): Relation(query, atom){}

bool GreaterEqual::Evaluate(const File & file) const{
	return !query->Lt(file, atom);
}
