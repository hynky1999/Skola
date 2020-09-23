#include <cassert>
#include <iostream>
#include "queue.h"

void basic_tests ( ) {
	CQueue q;
	assert ( q.empty ( ) );

	q.push ( 10 );
	assert ( !q.empty ( ) );
	assert ( q.front() == 10 );
	q.pop ( );
	assert ( q.empty ( ) );

	q.push ( 10 );
	q.push ( 20 );
	assert ( !q.empty ( ) );
	assert ( q.front() == 10 );
	q.pop ( );
	assert ( !q.empty ( ) );
	assert ( q.front() == 20 );
	q.pop ( );
	assert ( q.empty ( ) );


	for (int i = 0; i < 1000; ++i)
		q.push ( 10 * i );
	for (int i = 0; i < 999; ++i) {
		q.pop();
		assert ( !q.empty ( ) );
	}

	q.pop ( );
	assert ( q.empty ( ) );

	// https://courses.fit.cvut.cz/BI-PA2/tutorials/texts/exceptions.html
	try {
		q.pop(); // should throw exception
		assert ( false );
	}
	catch ( const char * e ) {
		std::cout << "Exception catched: " << e << std::endl;
	}
	assert ( q.empty ( ) );

	// test realloc with head not pointing to element 0
	CQueue q2;
	for (int i = 0; i < 3; ++i) q2.push ( 3 );
	for (int i = 0; i < 3; ++i) q2.pop ( );
	// . . . H

	for ( int i = 0; i < 50; i++ ) q2.push ( i );
	for ( int i = 0; i < 50; i++ ) {
		assert(q2.front ( ) == i );
		q2.pop ( );
	}
}

int main ( ) {
	basic_tests ( );

	{
		CQueue q1;
		q1.push ( 3 );

		CQueue q2 = q1;
		q1.pop ( );

		assert ( q1.empty ( ) );
		assert ( ! q2.empty ( ) );
	}

	{
		CQueue q1;
		for ( int i = 0; i < 1000; i++ )
			q1.push ( 3 );
		for ( int i = 0; i < 950; i++ )
			q1.pop ( );

		CQueue q2 = q1;
		for ( int i = 0; i < 50; i++ )
			q2.pop ( );
		assert ( q2.empty ( ) );
		assert ( ! q1.empty ( ) );

		for ( int i = 0; i < 50; i++ )
			q1.pop ( );
		assert ( q2.empty ( ) );
		assert ( q1.empty ( ) );
	}

	{
		CQueue q1;
		for ( int i = 0; i < 50; i++ )
			q1.push ( 3 );


		CQueue q2 ( q1 );
		q2.push ( 4 );
		for ( int i = 0; i < 50; i++ )
			q2.pop ( );
		q2.pop ( );

		assert ( ! q1.empty ( ) );
		assert ( q2.empty ( ) );
	}

	{
		CQueue q1, q2;
		q1.push ( 3 );
		q2.push ( 30 );

		assert ( ! q1.empty ( ) );
		assert ( ! q2.empty ( ) );

		q1 = q2;

		q1.push ( 4 );
		q2.pop ( );

		assert ( ! q1.empty ( ) );
		assert ( q2.empty ( ) );
	}

	{
		CQueue q1;
		q1.push ( 3 );
		assert ( ! q1.empty ( ) );
		q1 = q1;
		assert ( ! q1.empty ( ) );
	}
}
