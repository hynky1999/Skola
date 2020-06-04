// #pragma once

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <cstdlib>

class CQueue {
	using T = int;
	// typedef T int;

	T* arr;
	size_t arrSz, head, tail, cnt;

	void realloc();

public:
	CQueue ( );
	CQueue (const CQueue & src);
	
	~CQueue ( );

	CQueue & operator = (const CQueue & src);

	/** vlozi prvek do fronty */
	void push ( const T & elem ); 

	/** vrati prvni prvek */	
	const T& front ( ) const;

	/** dej pryc prvni prvek */
	void pop ( );

	/** zjisti zda je fronta prazdna */
	bool empty ( ) const;
};

#endif 
