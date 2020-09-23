#pragma once

#include "Expression.h"
#include "Query.h"

/**
 * @brief Abstract class representing Relation of two query and atom
 * When parsing relation must be in this format query Rel atom
 *
 */
class Relation: public BasicExpr{
protected:
	std::shared_ptr<BasicExpr> atom;
	std::shared_ptr<BaseQuery> query;
public:

	Relation(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom);
};
	
/**
 * @brief Class representing < Relation
 */
class LowerThan: public Relation{
public:
	static const std::string LexicalAtom;

	LowerThan(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom);

	bool Evaluate(const File & file) const override;
};

/**
 * @brief Class representing > Relation
 */
class GreaterThan: public Relation{
public:
	static const std::string LexicalAtom;

	GreaterThan(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom);

	bool Evaluate(const File & file) const override;
};

/**
 * @brief Class representing == Relation
 */
class Equal: public Relation{
public:
	static const std::string LexicalAtom;

	Equal(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom);

	bool Evaluate(const File & file) const override;
};

/**
 * @brief Class representing != Relation
 */
class NotEqual: public Relation{
public:
	static const std::string LexicalAtom;

	NotEqual(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom);

	bool Evaluate(const File & file) const override;
};

/**
 * @brief Class representing <= Relation
 */
class LowerEqual: public Relation{
public:
	static const std::string LexicalAtom;

	LowerEqual(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom);

	bool Evaluate(const File & file) const override;
};

/**
 * @brief Class representing >= Relation
 */
class GreaterEqual: public Relation{
public:
	static const std::string LexicalAtom;

	GreaterEqual(std::shared_ptr<BasicExpr> query, std::shared_ptr<BasicExpr> atom);

	bool Evaluate(const File & file) const override;
};
