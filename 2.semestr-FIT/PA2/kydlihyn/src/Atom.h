#pragma once
#include "Expression.h"

#include <memory>
#include <vector>
#include <string>

/**
 * @brief Templated class for representing atom
 * Anything can be atom
 *
 * @tparam AttrType type of atom can be really anything
 */
template <typename AttrType>
class Atom : public BasicExpr{
	AttrType value;

public:
	Atom(const AttrType value);

	bool Evaluate(const File & file) const;

	const AttrType & getValue() const;

	friend bool operator < (const Atom < AttrType > & self, const Atom < AttrType > & other){
		return self.getValue() < other.getValue();
	}

	friend bool operator == (const Atom < AttrType > & self, const Atom < AttrType > & other){
		return self.getValue() == other.getValue();
	}
};

template <typename AttrType>
Atom<AttrType>::Atom(const AttrType value): value(value){}

template <typename AttrType>
bool Atom<AttrType>::Evaluate(const File & file) const{
	return true;
}

template <typename AttrType>
const AttrType & Atom<AttrType>::getValue() const{
	return value;
}

/**
 * @brief Template specification of atom to get correct evaluate function
 */
template <>
class Atom<bool> : public BasicExpr{
	bool value;

public:
	Atom(const bool value): value(value){}
	bool Evaluate(const File & file) const{
		return value;
	}
	const bool & getValue() const{
		return value;
	}
	friend bool operator < (const Atom < bool > & self, const Atom < bool > & other){
		return self.getValue() < other.getValue();
	}
	friend bool operator == (const Atom < bool > & self, const Atom < bool > & other){
		return self.getValue() == other.getValue();
	}
};

/**
 * @brief Class for representing Vector of Atoms with getAsSomethingMethods
 */
class AtomsVector : public BasicExpr{
	std::vector< std::shared_ptr < BasicExpr > > atoms;

public:
	AtomsVector(const std::vector < std::shared_ptr < BasicExpr > > atoms);

	bool Evaluate(const File & file) const;
	/**
	 * @brief Function try to return vector of its contents as string
	 *
	 * @return vector of string atoms if fails it throws logic error exception
	 */
	const std::vector< Atom < std::string > > GetAsStrings() const;
};
