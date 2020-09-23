#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;
#endif /* __PROGTEST__ */


class MyString{
	char* str = nullptr;
public:
	MyString() = default;
	MyString(const char * str){
		this->str = new char[strlen(str)+1];
		strcpy(this->str,str);
	}
	MyString(const MyString & other){
		this->str = new char[strlen(other.str)+1];
		strcpy(this->str,other.str);
	}

	MyString & operator = (const MyString & other){
		if(&other == this)
			return *this;
		delete [] str;
		str = nullptr;
		this->str = new char[strlen(other.str)+1];
		strcpy(this->str,other.str);
		return *this;
		}



	friend ostream & operator << (ostream & os, const MyString & dt){
		os << dt.str;
		return os;
	}

	bool operator == (const MyString & cmp) const {
		return strcmp(cmp.str, str) == 0;
	}

	~MyString(){
		delete [] str;
	}
};
template <class T> class CVector {
public:
	T* list = nullptr;
	size_t allocated = 0;
	size_t len = 0;

	void reallocate(size_t newSize){
		allocated = newSize;
		T* tmp = new T[allocated];
		for (size_t i = 0; i < len; ++i)
			  tmp[i] = list[i];
		delete [] list;
		list = nullptr;
		list = tmp;
	}

        CVector() = default;
        CVector(const CVector &other) {
          for (size_t i = 0; i < other.length(); ++i)
            push_back(other.list[i]);
        }

        CVector &operator=(const CVector &other) {
          if (this == &other)
            return *this;
          delete [] list;
          list = nullptr;
          allocated = 0;
          len = 0;
          for (size_t i = 0; i < other.length(); ++i)
            push_back(other.list[i]);
          return *this;
        }

        void push_back(T & item){
          if (allocated == len) {
            reallocate(allocated * 2 + 10);
          }
          list[len] = item;
          ++len;
	}

	size_t search(T & item) const{
	  for (size_t i = 0; i < len; ++i) {
		if (item == list[i]){
		  return i;
		 }
	  }
	  return len;
	}

	T & operator [] (size_t index){
		return list[index];
	}
	const T & operator [] (size_t index) const{
		return list[index];
	}

	void resize(size_t size){
		delete [] list;
		list = nullptr;
		len = 0;
		allocated = 0;
	}

	const size_t length() const{
		return len;
	}

	~CVector() {
	  delete[] list;
	}
};

class Transaction{
public:
	MyString id;
	MyString sign;
	bool sender;
	int amount;

	Transaction(){
		sender = false;
		amount = 0;
	}
	Transaction(MyString sign, MyString id, int amount, bool sender){
		this->id = id;
		this->sign = sign;
		this->sender = sender;
		this->amount = amount;
	}
	friend ostream & operator << (ostream & os, const Transaction & tr){
		os << (tr.sender ? " - " : " + ") << abs(tr.amount) << (tr.sender ? ", to: " : ", from: ") << tr.id << ", sign: "<< tr.sign;
		return os;
	}

	int getAmount() const{
		return amount;
	}

};

class CAccount{
public:
    struct Internal{
    	MyString* id = nullptr;
    	CVector<Transaction>* trans = nullptr;
    	int* balance = nullptr;
    	size_t refNum = 1;
    };
    Internal* internal = nullptr;
	CAccount() = default;

	CAccount(const char* AccId, int initialBalance = 0) {
		internal = new Internal;
		internal->id = new MyString(AccId);
		internal->balance = new int(initialBalance);
		internal->trans = new CVector<Transaction>;
	}
    CAccount(const CAccount & other){
    	internal = other.internal;
    	++(internal->refNum);
    }
    CAccount & operator = (const CAccount & other){
    	if(&other == this)
    		return *this;
    	this->~CAccount();
    	internal = other.internal;
    	++(internal->refNum);
    	return *this;
    }

	int Balance() const{
		int balance_tmp = *(internal->balance);
		for(size_t i = 0; i < internal->trans->length(); ++i){
			balance_tmp += (*(internal->trans))[i].getAmount();
		}
		return balance_tmp;
	}

	void trim(){
		Copy();
		*(internal->balance) = Balance();
		internal->trans->resize(0);
	}

	void send(MyString sign, MyString receiver, int value){
		Copy();
		Transaction t(sign, receiver, -value, 1);
		internal->trans->push_back(t);
	}
	void receive(MyString sign, MyString sender, int value){
		Copy();
		Transaction t(sign, sender, value, 0);
		internal->trans->push_back(t);
	}
    void Copy(){
    	if(internal->refNum == 1)
    		return;
    	Internal* internal_tmp = new Internal;
    	internal_tmp->trans = new CVector<Transaction>;
    	internal_tmp->balance = new int;
    	internal_tmp->id = new MyString();
    	*(internal_tmp->balance) = *(internal->balance);
    	*(internal_tmp->trans) = *(internal->trans);
    	*(internal_tmp->id) = *(internal->id);
    	this->~CAccount();
    	internal = internal_tmp;
    }


	friend ostream & operator << (ostream & os, const CAccount & acc){
		int balanceEnd = *(acc.internal->balance);
		os << *(acc.internal->id) << ":" << endl;
		os << "   " << *(acc.internal->balance) << endl;
		for(size_t i = 0; i < acc.internal->trans->length(); ++i){
			balanceEnd += (*(acc.internal->trans))[i].getAmount();
			os << (*(acc.internal->trans))[i] << endl;
		}
		os << " = " << balanceEnd << endl;
		return os;
	}
	bool operator == (const CAccount & cmp) const{
		return *(internal->id) == *(cmp.internal->id);
	}

	~CAccount(){
		if(internal && --(internal->refNum) == 0){
			delete internal->trans;
			delete internal->id;
			delete internal->balance;
			delete internal;
		}
	}


};


class CBank
{
  public:
    struct Internal{
    	CVector<CAccount>* list = nullptr;
    	size_t refNum = 1;
    };
    Internal* internal;
    // todo
    // default constructor
    // copy constructor
    // destructor
    // operator =
    CBank(){
    	internal = new Internal;
    	internal->list = new CVector<CAccount>;
    }
    CBank(const CBank & other){
    	internal = other.internal;
    	++(internal->refNum);
    }
    CBank & operator = (const CBank & other){
    	if(&other == this)
    		return *this;
    	this->~CBank();
    	internal = other.internal;
    	++(internal->refNum);
    	return *this;
    }

    bool   NewAccount   ( const char * accID,
                           int          initialBalance ){
        CAccount acc(accID, initialBalance);
    	size_t pos = internal->list->search(acc);
    	if(pos != internal->list->length())
    		return false;
    	Copy();
    	internal->list->push_back(acc);
    	return true;
    }

    void Copy(){
    	if(internal->refNum == 1)
    		return;
    	Internal* internal_tmp = new Internal;
    	internal_tmp->list = new CVector<CAccount>;
    	*(internal_tmp->list) = *(internal->list);
    	this->~CBank();
    	internal = internal_tmp;
    }

    bool   Transaction   ( const char * debAccID,
                           const char * credAccID,
                           unsigned int amount,
                           const char * signature ){
        CAccount sender(debAccID);
        CAccount receiver(credAccID);
        size_t pos1 = internal->list->search(sender);
        size_t pos2 = internal->list->search(receiver);
        if(pos1 == internal->list->length() || pos2 == internal->list->length() || pos2 == pos1)
        	return false;
        Copy();
       	(*(internal->list))[pos1].send(signature, credAccID, amount);
       	(*(internal->list))[pos2].receive(signature, debAccID, amount);
       	return true;
       }
   	
    bool   TrimAccount   ( const char * accID ){
    	CAccount acc(accID);
    	size_t pos1 = internal->list->search(acc);
    	if(pos1 == internal->list->length())
    		return false;
    	Copy();
    	(*(internal->list))[pos1].trim();
    	return true;
    }
	
	CAccount & Account(const char* accID){
		CAccount acc(accID);
		size_t pos1 = internal->list->search(acc);
		if(pos1 == internal->list->length())
			throw logic_error("");
		return (*internal->list)[pos1];
	}

	~CBank(){
		if(--(internal->refNum) == 0){
			delete internal->list;
			delete internal;
		}
	}

};

#ifndef __PROGTEST__
int main ( void )
{
  ostringstream os;
  char accCpy[100], debCpy[100], credCpy[100], signCpy[100];
  /* CBank x0; */
  /* assert ( x0 . NewAccount ( "123456", 1000 ) ); */
  /* assert ( x0 . NewAccount ( "987654", -500 ) ); */
  /* assert ( x0 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) ); */
  /* assert ( x0 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) ); */
  /* assert ( x0 . NewAccount ( "111111", 5000 ) ); */
  /* assert ( x0 . Transaction ( "111111", "987654", 290, "Okh6e+8rAiuT5=" ) ); */
  /* assert ( x0 . Account ( "123456" ). Balance ( ) ==  -2190 ); */
  /* assert ( x0 . Account ( "987654" ). Balance ( ) ==  2980 ); */
  /* assert ( x0 . Account ( "111111" ). Balance ( ) ==  4710 ); */
  /* os . str ( "" ); */
  /* os << x0 . Account ( "123456" ); */
  /* assert ( ! strcmp ( os . str () . c_str (), "123456:\n   1000\n - 300, to: 987654, sign: XAbG5uKz6E=\n - 2890, to: 987654, sign: AbG5uKz6E=\n = -2190\n" ) ); */
  /* os . str ( "" ); */
  /* os << x0 . Account ( "987654" ); */
  /* assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 290, from: 111111, sign: Okh6e+8rAiuT5=\n = 2980\n" ) ); */
  /* os . str ( "" ); */
  /* os << x0 . Account ( "111111" ); */
  /* assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 290, to: 987654, sign: Okh6e+8rAiuT5=\n = 4710\n" ) ); */
  /* assert ( x0 . TrimAccount ( "987654" ) ); */
  /* assert ( x0 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) ); */
  /* os . str ( "" ); */
  /* os << x0 . Account ( "987654" ); */
  /* assert ( ! strcmp ( os . str () . c_str (), "987654:\n   2980\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 3103\n" ) ); */

  /* CBank x2; */
  /* strncpy ( accCpy, "123456", sizeof ( accCpy ) ); */
  /* assert ( x2 . NewAccount ( accCpy, 1000 )); */
  /* strncpy ( accCpy, "987654", sizeof ( accCpy ) ); */
  /* assert ( x2 . NewAccount ( accCpy, -500 )); */
  /* strncpy ( debCpy, "123456", sizeof ( debCpy ) ); */
  /* strncpy ( credCpy, "987654", sizeof ( credCpy ) ); */
  /* strncpy ( signCpy, "XAbG5uKz6E=", sizeof ( signCpy ) ); */
  /* assert ( x2 . Transaction ( debCpy, credCpy, 300, signCpy ) ); */
  /* strncpy ( debCpy, "123456", sizeof ( debCpy ) ); */
  /* strncpy ( credCpy, "987654", sizeof ( credCpy ) ); */
  /* strncpy ( signCpy, "AbG5uKz6E=", sizeof ( signCpy ) ); */
  /* assert ( x2 . Transaction ( debCpy, credCpy, 2890, signCpy ) ); */
  /* strncpy ( accCpy, "111111", sizeof ( accCpy ) ); */
  /* assert ( x2 . NewAccount ( accCpy, 5000 )); */
  /* strncpy ( debCpy, "111111", sizeof ( debCpy ) ); */
  /* strncpy ( credCpy, "987654", sizeof ( credCpy ) ); */
  /* strncpy ( signCpy, "Okh6e+8rAiuT5=", sizeof ( signCpy ) ); */
  /* assert ( x2 . Transaction ( debCpy, credCpy, 2890, signCpy ) ); */
  /* assert ( x2 . Account ( "123456" ). Balance ( ) ==  -2190 ); */
  /* assert ( x2 . Account ( "987654" ). Balance ( ) ==  5580 ); */
  /* assert ( x2 . Account ( "111111" ). Balance ( ) ==  2110 ); */
  /* os . str ( "" ); */
  /* os << x2 . Account ( "123456" ); */
  /* assert ( ! strcmp ( os . str () . c_str (), "123456:\n   1000\n - 300, to: 987654, sign: XAbG5uKz6E=\n - 2890, to: 987654, sign: AbG5uKz6E=\n = -2190\n" ) ); */
  /* os . str ( "" ); */
  /* os << x2 . Account ( "987654" ); */
  /* assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n = 5580\n" ) ); */
  /* os . str ( "" ); */
  /* os << x2 . Account ( "111111" ); */
  /* assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n = 2110\n" ) ); */
  /* assert ( x2 . TrimAccount ( "987654" ) ); */
  /* strncpy ( debCpy, "111111", sizeof ( debCpy ) ); */
  /* strncpy ( credCpy, "987654", sizeof ( credCpy ) ); */
  /* strncpy ( signCpy, "asdf78wrnASDT3W", sizeof ( signCpy ) ); */
  /* assert ( x2 . Transaction ( debCpy, credCpy, 123, signCpy ) ); */
  /* os . str ( "" ); */
  /* os << x2 . Account ( "987654" ); */
  /* assert ( ! strcmp ( os . str () . c_str (), "987654:\n   5580\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n" ) ); */

  /* CBank x4; */
  /* assert ( x4 . NewAccount ( "123456", 1000 ) ); */
  /* assert ( x4 . NewAccount ( "987654", -500 ) ); */
  /* assert ( !x4 . NewAccount ( "123456", 3000 ) ); */
  /* assert ( !x4 . Transaction ( "123456", "666", 100, "123nr6dfqkwbv5" ) ); */
  /* assert ( !x4 . Transaction ( "666", "123456", 100, "34dGD74JsdfKGH" ) ); */
  /* assert ( !x4 . Transaction ( "123456", "123456", 100, "Juaw7Jasdkjb5" ) ); */
  /* try */
  /* { */
  /*   x4 . Account ( "666" ). Balance ( ); */
  /*   assert ( "Missing exception !!" == NULL ); */
  /* } */
  /* catch ( ... ) */
  /* { */
  /* } */
  /* try */
  /* { */
  /*   os << x4 . Account ( "666" ). Balance ( ); */
  /*   assert ( "Missing exception !!" == NULL ); */
  /* } */
  /* catch ( ... ) */
  /* { */
  /* } */
  /* assert ( !x4 . TrimAccount ( "666" ) ); */

  CBank x6;
  assert ( x6 . NewAccount ( "123456", 1000 ) );
  assert ( x6 . NewAccount ( "987654", -500 ) );
  assert ( x6 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
  assert ( x6 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
  assert ( x6 . NewAccount ( "111111", 5000 ) );
  assert ( x6 . Transaction ( "111111", "987654", 2890, "Okh6e+8rAiuT5=" ) );
  CBank x7 ( x6 );
  assert ( x6 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
  assert ( x7 . Transaction ( "111111", "987654", 789, "SGDFTYE3sdfsd3W" ) );
  assert ( x6 . NewAccount ( "99999999", 7000 ) );
  assert ( x6 . Transaction ( "111111", "99999999", 3789, "aher5asdVsAD" ) );
  assert ( x6 . TrimAccount ( "111111" ) );
  assert ( x6 . Transaction ( "123456", "111111", 221, "Q23wr234ER==" ) );
  os . str ( "" );
  os << x6 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   -1802\n + 221, from: 123456, sign: Q23wr234ER==\n = -1581\n" ) );
  os . str ( "" );
  os << x6 . Account ( "99999999" );
  assert ( ! strcmp ( os . str () . c_str (), "99999999:\n   7000\n + 3789, from: 111111, sign: aher5asdVsAD\n = 10789\n" ) );
  os . str ( "" );
  os << x6 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n" ) );
  os . str ( "" );
  os << x7 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n" ) );
  try
  {
    os << x7 . Account ( "99999999" ). Balance ( );
    assert ( "Missing exception !!" == NULL );
  }
  catch ( ... )
  {
  }
  os . str ( "" );
  os << x7 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 789, from: 111111, sign: SGDFTYE3sdfsd3W\n = 6369\n" ) );

  CBank x8;
  CBank x9;
  assert ( x8 . NewAccount ( "123456", 1000 ) );
  assert ( x8 . NewAccount ( "987654", -500 ) );
  assert ( x8 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
  assert ( x8 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
  assert ( x8 . NewAccount ( "111111", 5000 ) );
  assert ( x8 . Transaction ( "111111", "987654", 2890, "Okh6e+8rAiuT5=" ) );
  x9 = x8;
  x8 = x8;
  assert ( x8 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
  assert ( x9 . Transaction ( "111111", "987654", 789, "SGDFTYE3sdfsd3W" ) );
  assert ( x8 . NewAccount ( "99999999", 7000 ) );
  assert ( x8 . Transaction ( "111111", "99999999", 3789, "aher5asdVsAD" ) );
  assert ( x8 . TrimAccount ( "111111" ) );
  os . str ( "" );
  os << x8 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   -1802\n = -1802\n" ) );
  os . str ( "" );
  os << x9 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n" ) );

  return 0;
}
#endif /* __PROGTEST__ */
