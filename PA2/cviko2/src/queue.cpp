#include "queue.h"

CQueue::CQueue(){
	cnt = 0;
	head = 0;
	tail = 0;
	arr = nullptr; // alokujeme pri prvnim pushi. Ale klidne muzeme i zde.
	arrSz = 0;
}

CQueue::CQueue(const CQueue & src){
	cnt = src.cnt;
	head = src.head;
	tail = src.tail;
	arrSz = src.arrSz;
	arr = new T [ arrSz ];
	for(size_t i = 0; i < arrSz; ++i)
		arr[i] = src.arr[i];
}

CQueue& CQueue::operator = (const CQueue& src){
	if(&src == this)
		return *this;
	delete [] arr;
	cnt = src.cnt;
	head = src.head;
	tail = src.tail;
	arrSz = src.arrSz;
	arr = new T [ arrSz ];
	for(size_t i = 0; i < arrSz; ++i)
		arr[i] = src.arr[i];
	return *this;
}


CQueue::~CQueue(){
	delete [] arr; // safe, delete nullptr je OK
}


void CQueue::realloc( ){
	/*
	 * Mejme nasledujici stav: Pole ve fronte je plne a obsahuje 0,1,2,3,4,5,6,7,8,9
	 *          H
	 *          T
	 * |0|1|2|3|4|5|6|7|8|9|
	 * |6|7|8|9|0|1|2|3|4|5|
	 * |->---->|----->----->
	 *
	 * Potrebujeme tedy pole realokovat. Realokaci v C++ muzeme vyresit napriklad alokaci noveho, vetsiho, pole
	 * a naslenym prekopirovanim hodnot a smazanim stareho pole.
	 *
	 * V nasem pripade vsak jeste musime stare pole preskladat do noveho nejak chytre, aby se nerozbilo poradi prvku v nasi fronte.
	 * Kdyby se nyni pole jen zvetsilo bez ohledu na obsah, vypadalo by to nejak takhle:
	 *
	 *          H
	 *          T
	 * |0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|
	 * |6|7|8|9|0|1|2|3|4|5|  |  |  |  |  |  |  |  |  |  |
	 * |       |----->--------->-------|
	 *
	 * No a to neni stejna fronta jako pred tim. Od pozice head by nasledujicich 10 polozek melo reprezentovat nasi frontu 0,1,...,9.
	 *
	 * Co tedy udelame, je, ze probehneme stare pole v presne v poradi fronty. Zacneme na headu a zkopirujeme
	 * nasledujicich 10 prvku do noveho pole, to tedy bude vypadat treba takto:
	 *
	 *  H                   T
	 * |0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|
	 * |0|1|2|3|4|5|6|7|8|9|  |  |  |  |  |  |  |  |  |  |
	 * |----->--------->---|
	 */

	size_t oldSize = arrSz;
	arrSz = 2*arrSz + 5; // na pocatku 0, protoze 2*0 = 0, davame +5

	T* narr = new T [ arrSz ];

	for ( size_t i = 0 ; i < oldSize ; i++ )
		narr[i] = arr[(head + i) % oldSize];

	head = 0;
	tail = cnt;

	delete [] arr;
	arr = narr;
}

void CQueue::push( const T & elem ){
	// Mame plno - pocet prvku ve fronte je stejny jako naalokovana velikost pole
	// dalsi se nam tam uz nevejde - nejdriv zvetsime
	if( cnt == arrSz ){
		realloc();
	}

	// Vlozime do pole na prvni volnou pozici
	/*
	 *                H   T
	 * |0|1|2|3|4|5|6|7|8|9|
	 * | | | | | | | |3|4| |
	 *
	 *  T             H
	 * |0|1|2|3|4|5|6|7|8|9|
	 * | | | | | | | |3|4|5|
	 */
	arr[tail] = elem;
	tail = (tail + 1) % arrSz;
	cnt += 1;
}

void CQueue::pop()
{
	if ( empty ( ) )
		throw "pop(): empty queue";
	// https://courses.fit.cvut.cz/BI-PA2/tutorials/texts/exceptions.html

	// Chceme odstranit prvek na prvni pozici.
	/*
	 *      T         H
	 * |0|1|2|3|4|5|6|7|8|9|
	 * |6|7| | | | | |3|4|5|
	 *
	 * Predstavme si opet stav fronty podle "obrazku". Co potrebujeme, je vlastne posun indexu HEAD o 1 dopredu.
	 * Toho dosahneme jeho inkrementaci o jedna (coz je snad jasne) a dale, protoze muzeme opet vyskocit mimo pole,
	 * vymodulime tento vysledek veliksoti pole. Fronta by pak mela vypadat asi takto:
	 *
	 *                  H
	 * |0|1|2|3|4|5|6|7|8|9|
	 * |6|7| | | | | | |4|5|
	 */
	head = (head + 1) % arrSz;
	cnt -= 1;
}

const CQueue::T & CQueue::front() const
{
	if ( empty ( ) )
		throw "front(): empty queue";

	return arr[head];
}

bool CQueue::empty () const{
	return cnt == 0;
}
