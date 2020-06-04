#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
using namespace std;
#endif /* __PROGTEST__ */




class BinaryStream{

private:
	uint8_t offset;
	ifstream ifs;

public:
	BinaryStream(const char* fileName1){
		offset = 0;
		this->ifs = ifstream(fileName1, ios::binary);
	}

	int good(){
		return ifs.good();
	}


	int __getBits(int n, uint32_t & res) {
		if(n == 0)
			return 0;

		if(offset == 8){
			if(!ifs.ignore(1))
				return -1;
			offset = 0;
		}
		res = (res << 1) | ((ifs.peek() >> (7-offset)) & 1);
		offset += 1;
		return getBits(n-1, res);
	}

	int getBits(int n, uint32_t res){
		res = 0;
		return __getBits(n, res);
	}


};

int convertUTF(BinaryStream & bifs, char* &character, int & count){
	uint32_t byte = 0;
	if(bifs.getBits(8, byte) < 1)
		return -1;

	if(byte >> 7 == 0)
		count = 1;
	else{
		for(uint8_t x = 2; x < 7; x++){
			if(byte >> (7-x) == (1 << (x+1))-2)
				count = x;
		}
	}
	if (count == 0)
		return -1;

	character = new char[count];
	for(int x = 0;; x++){
		character[x] = byte;
		if( x+1 == count)
			break;
		if(bifs.getBits(8, byte) < 1 || byte >> 6 != 2 )
			return -1;
	}
	return 1;
}

struct Node{
	char* character;
	int len;
	Node* l_child;
	Node* r_child;
public:
	Node(){
		character = nullptr;
		l_child = nullptr;
		r_child = nullptr;
		len = 0;
	}

	~Node(){
		
	}
};

class DecompressTree{
public:
	Node* root;

	Node* createTree(BinaryStream & bifs){
		Node* node = new Node;
		uint8_t bit = 0;
		if(bifs.getBits(1, bit) < 1){
		cout << 1111 << endl;
			return nullptr;
		}

		cout << bit << endl;
		if(bit == 0){
			node->l_child = createTree(bifs);
			node->r_child = createTree(bifs);
			if(node->l_child == nullptr || node->r_child == nullptr)
				return nullptr;
		}

		if(bit == 1){
			if(convertUTF(bifs, node->character, node->len) < 1)
				return nullptr;
			cout << node->character[0] << endl;
			return node;
		}

		return nullptr;
	}

	void printTree(Node* node){
		if(node->character)
			cout << node->character << endl;
		if(node->l_child)
			printTree(node->l_child);
		if(node->r_child)
			printTree(node->r_child);
		
		return;
	}
		

public:
	DecompressTree(BinaryStream & bifs){
		root = createTree(bifs);
	}

	int __decompressCode(char* & character, int len, BinaryStream & bifs, Node* node) const{
		if(node->l_child == nullptr && node->r_child == nullptr)
			// We got a leaf lol
			character = node->character;
			len = node->len;
			return 1;
		
		uint8_t bit = 0;
		if(bifs.getBits(1, bit) < 1)
			return -1;

		if(bit == 0)
			return __decompressCode(character, len, bifs, node->l_child);

		else if(bit == 1)
			return __decompressCode(character, len, bifs, node->r_child);
		return -1;
	}

	int decompressCode(char * & character, int & len, BinaryStream & bifs) const{
		return __decompressCode(character, len, bifs, root);
	}

	int good(){
		return root == nullptr;
	}

	void destroyRec(Node* node){
		if(node->l_child)
			destroyRec(node->l_child);
		if(node->r_child)
			destroyRec(node->r_child);
		delete [] node->character;
		delete [] node;
		return;
	}

	~DecompressTree(){
		destroyRec(root);
	}
};

int decompressChunk(BinaryStream & bifs, ofstream & ofs, DecompressTree & dct){
	uint32_t bit = 0;
	if(bifs.getBits(1, bit) < 1)
		return -1;
	int count = 4096;
	if(bit == 0){
		if(bifs.getBits(12, bit) < 1)
			return -1;
		count = bit;
	}

	for(int x = 0; x < count; x++){
		char* ch = nullptr;
		int len = 0;
		if(dct.decompressCode(ch, len, bifs) < 1)
			return -1;
		ofs.write(ch, len);
	}
	return count == 4096 ? 1 : 2;
}

bool decompressFile ( const char * inFileName, const char * outFileName )
{
  // todo
	BinaryStream bifs(inFileName);
	if(!bifs.good())
		return false;

	ofstream ofs(outFileName);
	if(!ofs.good())
		return false;
	DecompressTree dct(bifs);
	if(!dct.good())
		return false;

	dct.printTree(dct.root);

	while(true){
		int res = decompressChunk(bifs, ofs, dct);
		if(res == -1)
			return -1;
		if(res == 2)
			break;
	}

	return true;
}

bool compressFile ( const char * inFileName, const char * outFileName )
{

  // keep this dummy implementation (no bonus) or implement the compression (bonus)
  return false;
}
#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1, const char * fileName2 )
{
  // todo
    std::ifstream file1(fileName1, std::ifstream::ate | std::ifstream::binary); //open file at the end
    std::ifstream file2(fileName2, std::ifstream::ate | std::ifstream::binary); //open file at the end
    const std::ifstream::pos_type fileSize = file1.tellg();

    if (fileSize != file2.tellg()) {
        return false; //different file size
    }

    file1.seekg(0); //rewind
    file2.seekg(0); //rewind

    std::istreambuf_iterator<char> begin1(file1);
    std::istreambuf_iterator<char> begin2(file2);

    return std::equal(begin1,std::istreambuf_iterator<char>(),begin2); //Second argument is end-of-range iterator
}

int main ( void )
{
  assert ( decompressFile ( "tests/test0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test4.orig", "tempfile" ) );

  assert ( ! decompressFile ( "tests/test5.huf", "tempfile" ) );


  assert ( decompressFile ( "tests/extra0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra4.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra5.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra5.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra6.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra6.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra7.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra7.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra8.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra8.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra9.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra9.orig", "tempfile" ) );

  return 0;
}
#endif /* __PROGTEST__ */
