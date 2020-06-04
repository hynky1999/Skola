#include <iostream>
#include <vector>

int main ( ) {
	std::vector < int > arr; // "chytré nafukovací pole", https://en.cppreference.com/w/cpp/container/vector

	// 50x vloz neco na konec
	for ( int i = 0; i < 50; i++ )
		arr.push_back ( i ) ;

	// zjisti pocet inicializovanych prvku
	std::cout << arr.size ( ) << std::endl;

	// vypis/modifikuj prvek - pokud prvek jiz existuje! Jinak undefined.
	// Napr. aktualni pole ma urcite velikost pole 50, tedy platne jsou indexy 0-49, jinak hrozi prinejlepsim uninitialized value, pripadne segfault
	std::cout << arr [ 10 ] << std::endl;
	arr[10] = 44;
	std::cout << arr [ 10 ] << std::endl;

	// std::cout << arr [ 100 ] << std::endl; // NE!

	// projdi vsechny prvky
	for ( const int& x : arr ) {
		std::cout << x << " ";
	}
	std::cout << std::endl;

	// projdi vsechny prvky, v2
	for ( size_t i = 0; i < arr.size ( ); ++ i )
		std::cout << arr [ i ] << " ";
	std::cout << std::endl;

	// projdi vsechny prvky, v3 (pomoci iteratoru, zatim nepotrbeujeme)
	for ( std::vector < int > :: iterator it = arr.begin ( ); it != arr.end ( ); ++ it )
		std::cout << * it << " ";
	std::cout << std::endl;

	// zaruc, ze pole bude velikosti 100 (zmensi/zvetsi). Pokud zvetsujes, nastav nove pridane hodnoty na 1
	arr.resize ( 100, 1 );
	std::cout << arr[99] << std::endl;


	// --------------------- dalsi -------------------------------

	// vloz na pozici N prvek, nasledujici se posunou, velikost je o 1 vetsi
	arr.insert ( arr.begin ( ) + 1, 10);
	std::cout << arr.size ( ) << " " << arr [ 1 ] << std::endl;
	for ( size_t i = 0; i < 5; i++ )
		std::cout << arr [ i ] << " ";
	std::cout << std::endl;

	// smaz prvek s pozice N, nasledujici se posunou, velikost se zmensi
	arr.erase ( arr.begin ( ) + 1 );
	std::cout << arr.size ( ) << " " << arr [ 1 ] << std::endl;
	for ( size_t i = 0; i < 5; i++ )
		std::cout << arr [ i ] << " ";
	std::cout << std::endl;

	return 0;
}
