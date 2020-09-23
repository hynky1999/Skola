#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

class CItem
	/*Class representing person and his financial information */
{
		string address;
		string name;
		string bankAccount;
		int income;
		int expense;


	public:
		CItem(const string & name, const string & address, const string & bankAccount){
			this->address = string(address);
			this->name = string(name);
			this->bankAccount = bankAccount;
			income = 0;
			expense = 0;
		}

		string getBankAccount() const{
			return bankAccount;
		}

		string getName() const{
			return name;
		}

		string getAddress() const{
			return address;
		}

		int getIncome() const{
			return income;
		}

		int getExpense() const{
			return expense;
		}

		void addIncome(int inc){
			income += inc;
		}

		void addExpense(int exp){
			expense += exp;
		}

		static bool cmpNameAddress(const CItem* cmp1, const CItem* cmp2){
			/* Helper function for comparing two Items by names lexicographically.
			 * If names are the same address will be used. */
			if(cmp1->name < cmp2->name)
				return true;
			if(cmp1->name == cmp2->name && cmp1->address < cmp2->address)
				return true;

			return false;
		}

		static bool cmpBankAccount(const CItem* cmp1, const CItem* cmp2){
			/* Helper function for comparing two Items by bank account lexicographically */
			return cmp1->bankAccount < cmp2->bankAccount;
		}


		bool operator < (const CItem & other) const{
			/* Not needed in task but it's nice to have default ordering method.
			 * Compares the same as cmpBankAccount */
			return this->bankAccount < other.bankAccount;
		}

		bool operator == (const CItem & other) const{
			/* Two Items are the same if their bankAccounts are the same or address and names are the same */
			return (
					bankAccount == other.bankAccount
					|| (name == other.name && address == other.address)
			);
		}

		bool operator != (const CItem & other) const{
			return !(*this == other);
		}
};

class CIterator
	/* Function representing iterator for CTaxRegister.
	 * Nothing really impressive. Just reusing vector iterators.
	 * If underlying CTaxRegister is changed CIterator could be invalidated*/
{
  	vector<CItem*>::const_iterator it;
  	vector<CItem*>::const_iterator end;

  public:
  	CIterator(vector<CItem*>::const_iterator it, vector<CItem*>::const_iterator end){
  		this->end = end;
  		this->it = it;
  	}

    bool AtEnd (void) const{
    	return it == end;
    }

    void Next (void){
    	++it;
    }

    string Name (void) const{
			return (*it)->getName();
		}

    string Addr (void) const{
    	return (*it)->getAddress();
    }

    string Account (void) const{
    	return (*it)->getBankAccount();
    }
};


class CTaxRegister
	/* Function representing register of non duplicit persons.
	 * Works pretty much like vector, but little bit of upgrades were done to get better performance for searching.
	 * It is done using two vectors with pointers to Items. So in both vectors there are the same pointers but in different order.
	 * Thus searching by bank account or by name/address can be done in O(log(N)) time.
	 * Insertion and deletion is done sadly in O(N) time. */
{
  private:
  	vector <CItem*> regBankAcc;
  	vector <CItem*> regNameAddr;

  public:
    bool Birth (const string & name, const string & addr, const string & account ){
    	CItem* newPerson = new CItem(name, addr, account);
    	// Find position for new Item
    	auto posItName = lower_bound(regNameAddr.begin(), regNameAddr.end(), newPerson, CItem::cmpNameAddress);
    	auto posItBankAcc = lower_bound(regBankAcc.begin(), regBankAcc.end(), newPerson, CItem::cmpBankAccount);
    	// Check if position is valid
		if(
    			(posItName != regNameAddr.end() && **posItName == *newPerson)
    			|| (posItBankAcc != regBankAcc.end() && **posItBankAcc == *newPerson)
		)
			{
				delete newPerson;
				return false;
			}
		// If not duplicate add it
    	regNameAddr.insert(posItName, newPerson);
    	regBankAcc.insert(posItBankAcc, newPerson);
    	return true;
    }
    	
    bool Death ( const string & name, const string & addr ){
    	// Let's create deleted Item, so we can use compare function to find deleted Item position
    	CItem delPersonAddr(name, addr,"");
    	auto posItName = lower_bound(regNameAddr.begin(), regNameAddr.end(), &delPersonAddr, CItem::cmpNameAddress);
    	// Check if deleted Item exists in vector
    	if(posItName == regNameAddr.end() || **posItName != delPersonAddr){
    		return false;
    	}
    	CItem* PdelPerson = *posItName;
    	regNameAddr.erase(posItName);

    	// We also need to delete Item in second vector we will use the same trick but with different cmp function
    	CItem delPersonBank(name, addr, PdelPerson->getBankAccount());
    	auto posItBankAcc = lower_bound(regBankAcc.begin(), regBankAcc.end(), &delPersonBank, CItem::cmpBankAccount);
    	regBankAcc.erase(posItBankAcc);
    	delete PdelPerson;
    	return true;
    }
    	
    bool Income (const string & account, int amount){
    	// Add income to person desribed by bank account.
    	CItem searchPerson("", "", account);
    	auto posIt = lower_bound(regBankAcc.begin(), regBankAcc.end(), &searchPerson, CItem::cmpBankAccount);
    	if(posIt == regBankAcc.end() || **posIt != searchPerson)
    		return false;
    	(*posIt)->addIncome(amount);
    	return true;
    }

    bool Income (const string & name, const string & addr, int amount){
    	// Add income to person desribed by address and name.
    	CItem searchPerson(name, addr, "");
    	auto posIt = lower_bound(regNameAddr.begin(), regNameAddr.end(), &searchPerson, CItem::cmpNameAddress);
    	if(posIt == regNameAddr.end() || **posIt != searchPerson)
    		return false;
    	(*posIt)->addIncome(amount);
    	return true;
    }

    bool Expense (const string & account, int amount ){
    	// Add expense to person desribed by bank account.
    	CItem searchPerson("", "", account);
    	auto posIt = lower_bound(regBankAcc.begin(), regBankAcc.end(), &searchPerson, CItem::cmpBankAccount);
    	if(posIt == regBankAcc.end() || **posIt != searchPerson)
    		return false;
    	(*posIt)->addExpense(amount);
    	return true;
    }

    bool Expense (const string & name, const string & addr, int amount ){
    	// Add expense to person desribed by address and name.
    	CItem searchPerson(name , addr, "");
    	auto posIt = lower_bound(regNameAddr.begin(), regNameAddr.end(), &searchPerson, CItem::cmpNameAddress);
    	if(posIt == regNameAddr.end() || **posIt != searchPerson)
    		return false;
    	(*posIt)->addExpense(amount);
    	return true;
    }

    bool Audit (const string & name, const string & addr, string & account, int & sumIncome, int & sumExpense) const{
    	// Search for person and scrap all his data and return them using references to Mr. Babis.
    	CItem searchPerson(name, addr, "");
    	auto posIt = lower_bound(regNameAddr.begin(), regNameAddr.end(), &searchPerson, CItem::cmpNameAddress);
    	if(posIt == regNameAddr.end() || **posIt != searchPerson)
    		return false;
    	
    	account = (*posIt)->getBankAccount();
    	sumIncome = (*posIt)->getIncome();
    	sumExpense = (*posIt)->getExpense();
    	return true;
    }

    CIterator ListByName (void) const{
    	return CIterator(regNameAddr.begin(), regNameAddr.end());
    }

    ~CTaxRegister(){
    	// Inhuman things going on here. We have to kill all Items in our vectors.
    	// Luckily the class is called Items not Person so I wouldn't really bitch about it being inhumane.
    	for(auto it1 = regNameAddr.begin(); it1 != regNameAddr.end();)
    		Death((*it1)->getName(), (*it1)->getAddress());
		}
  
};

#ifndef __PROGTEST__
int main ( void )
{
  string acct;
  int    sumIncome, sumExpense;
  CTaxRegister b1;
  assert ( b1 . Birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b1 . Birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( b1 . Birth ( "Peter Hacker", "Main Street 17", "634oddT" ) );
  assert ( b1 . Birth ( "John Smith", "Main Street 17", "Z343rwZ" ) );
  assert ( b1 . Income ( "Xuj5#94", 1000 ) );
  assert ( b1 . Income ( "634oddT", 2000 ) );
  assert ( b1 . Income ( "123/456/789", 3000 ) );
  assert ( b1 . Income ( "634oddT", 4000 ) );
  assert ( b1 . Income ( "Peter Hacker", "Main Street 17", 2000 ) );
  assert ( b1 . Expense ( "Jane Hacker", "Main Street 17", 2000 ) );
  assert ( b1 . Expense ( "John Smith", "Main Street 17", 500 ) );
  assert ( b1 . Expense ( "Jane Hacker", "Main Street 17", 1000 ) );
  assert ( b1 . Expense ( "Xuj5#94", 1300 ) );
  assert ( b1 . Audit ( "John Smith", "Oak Road 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "123/456/789" );
  assert ( sumIncome == 3000 );
  assert ( sumExpense == 0 );
  assert ( b1 . Audit ( "Jane Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 1000 );
  assert ( sumExpense == 4300 );
  assert ( b1 . Audit ( "Peter Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "634oddT" );
  assert ( sumIncome == 8000 );
  assert ( sumExpense == 0 );
  assert ( b1 . Audit ( "John Smith", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Z343rwZ" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 500 );
  CIterator it = b1 . ListByName ();
  assert ( ! it . AtEnd ()
           && it . Name () == "Jane Hacker"
           && it . Addr () == "Main Street 17"
           && it . Account () == "Xuj5#94" );
  it . Next ();
  assert ( ! it . AtEnd ()
           && it . Name () == "John Smith"
           && it . Addr () == "Main Street 17"
           && it . Account () == "Z343rwZ" );
  it . Next ();
  assert ( ! it . AtEnd ()
           && it . Name () == "John Smith"
           && it . Addr () == "Oak Road 23"
           && it . Account () == "123/456/789" );
  it . Next ();
  assert ( ! it . AtEnd ()
           && it . Name () == "Peter Hacker"
           && it . Addr () == "Main Street 17"
           && it . Account () == "634oddT" );
  it . Next ();
  assert ( it . AtEnd () );

  assert ( b1 . Death ( "John Smith", "Main Street 17" ) );

  CTaxRegister b2;
  assert ( b2 . Birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b2 . Birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( !b2 . Income ( "634oddT", 4000 ) );
  assert ( !b2 . Expense ( "John Smith", "Main Street 18", 500 ) );
  assert ( !b2 . Audit ( "John Nowak", "Second Street 23", acct, sumIncome, sumExpense ) );
  assert ( !b2 . Death ( "Peter Nowak", "5-th Avenue" ) );
  assert ( !b2 . Birth ( "Jane Hacker", "Main Street 17", "4et689A" ) );
  assert ( !b2 . Birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b2 . Death ( "Jane Hacker", "Main Street 17" ) );
  assert ( b2 . Birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b2 . Audit ( "Joe Hacker", "Elm Street 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 0 );
  assert ( !b2 . Birth ( "Joe Hacker", "Elm Street 23", "AAj5#94" ) );

  return 0;
}
#endif /* __PROGTEST__ */
