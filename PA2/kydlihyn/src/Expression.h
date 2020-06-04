#pragma once
#include "File.h"

/**
 * @brief Abstract base class for all expression
 */
class BasicExpr{
public:
	/**
	 * @brief Evaluation function which checks if file comply all constraints
	 *
	 * @param file file to check against
	 *
	 * @return true if it comply all constraints else false
	 */
	virtual bool Evaluate(const File & file) const = 0;
};

