#include "Atom.h"
#include "Constants.hpp"

#include <string>
#include <vector>

AtomsVector::AtomsVector(const std::vector < std::shared_ptr < BasicExpr > > atoms): atoms(atoms){}

bool AtomsVector::Evaluate(const File & file) const{
	// By default all atoms are true
	return true;
}
const std::vector< Atom < std::string > > AtomsVector::GetAsStrings() const{
	std::vector < Atom < std::string > > strings;
	for(auto it = atoms.begin(); it != atoms.end(); ++it){
		Atom< std::string >* str = dynamic_cast< Atom< std::string > * >(it->get());
		if(str == nullptr)
			throw std::logic_error(EVAL_ERR + ": EXPECTED ONLY STRINGS");
		strings.push_back(*str);
	}
	return strings;
}
