#include <list>
#include <cmath>
#include <iostream>
#include <algorithm>
using namespace std;

void erath(list<int> & l){
	// Iterate until sqrt(maxNum in list)
	int sqrt_num = sqrt(l.size() + 1);
	for(auto it = l.begin(); it != l.end(); ++it){
		// It will be the number that we will use to delete other numbers
		if(*it > sqrt_num){
			break;
		}
		auto it_tmp = it;
		it_tmp++;
		// Next number of it_tmp will be the next possible delete
		while(it_tmp != l.end()){
			// Remove everything we can to end of list
			// If the number we are using to delete is factor of it_tmp remove it
			if(*it_tmp% *it == 0){
				auto it_delete = it_tmp;
				it_tmp++;
				l.erase(it_delete);
			}
			else
				it_tmp++;
		}
	}
}

int main(int argc, char *argv[])
{
	list<int> l;
	for(int i = 2; i <= 500 * 1000; ++i)
		l.push_back(i);
	erath(l);
	for(const auto & elem : l)
		cout << elem<< " ";
	cout << endl;
	return 0;
}
