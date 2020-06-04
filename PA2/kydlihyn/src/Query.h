#pragma once

#include "Expression.h"
#include "Constants.hpp"
#include "Atom.h"
#include "RegularFile.h"

#include <algorithm>
#include <iostream>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <unordered_map>

/**
 * @brief Abstract class representing query eg. properity of file
 * Every query have to implement Lt and Eq method for relations
 * Evaluate method is currently not used in queries but could find its use in future.
 * For example in non-relation queries.
 */
class BaseQuery :public BasicExpr{
public:

	/**
	 * @brief Function implements lower than query, should return true if file.get < atom.get
	 *
	 * @param file file to compare
	 * @param atom atom to compare with
	 *
	 * @return true if file.get < atom.get else false
	 */
	virtual bool Lt(const File & file,const std::shared_ptr<BasicExpr> & atom) const = 0;

	/**
	 * @brief Function implements lower equal query, should return true if file.get == atom.get
	 *
	 * @param file file to compare
	 * @param atom atom to compare with
	 *
	 * @return true if file.get == atom.get else false
	 */
	virtual bool Eq(const File & file,const std::shared_ptr<BasicExpr> & atom) const = 0;

	/**
	 * @brief Currently has no meaning but could be used in future
	 */
	virtual bool Evaluate(const File & file) const override;
};


/**
 * @brief Class representing the simplest query.eg. simple equal/lower relation of atom and file attribute. Atom can be casted down to access attributes using getter, File can be casted down to access attributes using getter. Getters wouldn't be in need for simple queries but for more difficult they are needed and that is the reason why getters persist
 *
 * @tparam AtomType Type of atom which will be compared
 * @tparam FileType Type of file which will be compared
 * @tparam AtomReturn Return type of Atom accessor
 * @tparam FileReturn Return type of File accessor
 *
 */
template<
	typename AtomType,
	typename FileType = File,
	typename AtomReturn = typename AtomType::AttrType,
	typename FileReturn = AtomReturn
>
class Query : public BaseQuery{


	std::function<FileReturn(const FileType &)> getFile;
	std::function<AtomReturn(const AtomType &)> getAtom;

public:
	bool Lt(const File & file, const std::shared_ptr<BasicExpr> & atom) const override;
	bool Eq(const File & file, const std::shared_ptr<BasicExpr> & atom) const override;
	Query( AtomReturn (AtomType::*getAtom) () const, FileReturn (FileType::*getFile) () const);
};


template<typename AtomType, typename FileType, typename AtomReturn, typename FileReturn>
Query<AtomType, FileType, AtomReturn, FileReturn>::Query( AtomReturn(AtomType::* getAtom) () const,
		FileReturn(FileType::* getFile) () const): getFile(getFile), getAtom(getAtom){}

template<typename AtomType, typename FileType, typename AtomReturn, typename FileReturn>
bool Query<AtomType, FileType, AtomReturn, FileReturn>::Lt(const File & file, const std::shared_ptr<BasicExpr> & atom) const{
	// Typecast atom
	const AtomType* atom_cast = dynamic_cast<const AtomType*>(atom.get());
	if(atom_cast == nullptr)
		return false;

	// Typecast File
	const FileType* file_cast = dynamic_cast<const FileType*>(&file);
	if(file_cast == nullptr)
		return false;

	return getFile(*file_cast) < getAtom(*atom_cast);
}

template<typename AtomType, typename FileType, typename AtomReturn, typename FileReturn>
bool Query<AtomType, FileType, AtomReturn, FileReturn>::Eq(const File & file, const std::shared_ptr<BasicExpr> & atom) const{
	const AtomType* atom_cast = dynamic_cast<const AtomType*>(atom.get());

	// Typecast atom
	if(atom_cast == nullptr)
		return false;

	const FileType* file_cast = dynamic_cast<const FileType*>(&file);
	if(file_cast == nullptr)
		return false;

	// Typecast atom
	return getFile(*file_cast) == getAtom(*atom_cast);
}

//--------------------------------------------------------------------------------------

/**
 * @brief Class representing subset querry eg. querry where we ask is set of Atoms attribute is subset of File attribute
 *
 * @tparam AtomType Type of atom, can be container but can also be simple type
 * @tparam FileType Type of file
 * @tparam AtomReturn Return type of atom
 * @tparam FileReturn Return type of File
 */
template<
	typename AtomType,
	typename FileType = File,
	typename AtomReturn = typename AtomType::AttrType,
	typename FileReturn = AtomReturn
>
class QuerySubset : public BaseQuery{

	std::function<FileReturn(const FileType&)> getFile;
	std::function<AtomReturn(const AtomType&)> getAtom;

public:
	bool Lt(const File & file, const std::shared_ptr<BasicExpr> & atom) const override;
	bool Eq(const File & file, const std::shared_ptr<BasicExpr> & atom) const override;
	QuerySubset( AtomReturn (AtomType::*getAtom) () const, FileReturn (FileType::*getFile) () const);
};

template<typename AtomType, typename FileType, typename AtomReturn, typename FileReturn>
QuerySubset<AtomType, FileType, AtomReturn, FileReturn>::QuerySubset( AtomReturn(AtomType::* getAtom) () const,
		FileReturn(FileType::* getFile) () const): getFile(getFile), getAtom(getAtom){}

template<typename AtomType, typename FileType, typename AtomReturn, typename FileReturn>
bool QuerySubset<AtomType, FileType, AtomReturn, FileReturn>::Lt(const File & file, const std::shared_ptr<BasicExpr> & atom) const{
	throw std::logic_error(EVAL_ERR + std::string("Lt not implemented on subset Query"));
}

template<typename AtomType, typename FileType, typename AtomReturn, typename FileReturn>
bool QuerySubset<AtomType, FileType, AtomReturn, FileReturn>::Eq(const File & file, const std::shared_ptr<BasicExpr> & atom) const{
	// Create a new subset to load data to -> we have to copy but in order to be flexible there is no workaround + we will sort so reference would be bad
	typename std::remove_cv< typename std::remove_reference<AtomReturn>::type >::type subset;
	const AtomType* atom_set_cast = dynamic_cast<const AtomType*>(atom.get());
	// Load a atoms set
	if(atom_set_cast != nullptr){
		subset = getAtom(*atom_set_cast);
	}
	else{
		// If we cannot load atoms as set try to load it as set element
		const typename std::remove_cv< typename std::remove_reference<AtomReturn>::type >::type::value_type* atom_value_cast = dynamic_cast<const typename std::remove_cv< typename std::remove_reference<AtomReturn>::type >::type::value_type*>(atom.get());
		if(atom_value_cast == nullptr)
			return false;
		// Insert element to a set
		auto it = std::inserter(subset, subset.begin());
		it = *atom_value_cast;
	}
	// Same as loading atoms data
	typename std::remove_cv< typename std::remove_reference<FileReturn>::type >::type set;
	const FileType* file_cast = dynamic_cast<const FileType*>(&file);
	if(file_cast != nullptr)
		set = getFile(*file_cast);
	else{
		return false;
	}
	// Sort sets
	sort(subset.begin(), subset.end());
	sort(set.begin(), set.end());
	return std::includes(set.cbegin(), set.cend(), subset.cbegin(), subset.cend());
}

extern std::unordered_map< std::string, std::shared_ptr<BaseQuery>> QUERIES;
