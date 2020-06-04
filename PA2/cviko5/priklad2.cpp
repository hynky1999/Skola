#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
using namespace std;


struct cmpAbs{
	bool operator()(const int self, const int other)const{
		if(abs(self) > abs(other))
			return 1;
		else if(abs(self) == abs(other))
			return self > other;
		return 0;
	}
};
class CSequence{
	/* We use vector for fast access, map for counting duplicates and multiset for abs print.*/
	vector<int> inner_default;
	map<int,int,greater<int>> inner_map;
	multiset<int,cmpAbs> inner_set;
public:
	CSequence & insert(int x){
		inner_default.push_back(x);
		inner_map[x] += 1;
		inner_set.insert(x);
		return *this;
	}
	const int & operator [](size_t index) const{
		return inner_default[index];
	}
	int & operator [](size_t index){
		return inner_default[index];
	}
	friend ostream & operator << (ostream & ofs, CSequence & other){
		size_t i = 0;
		for(; i+1 < other.inner_default.size(); ++i)
			ofs << other.inner_default[i] << ", ";
		if(i != other.inner_default.size())
			ofs << other.inner_default[i];
		return ofs;
	}
	void printCollissions() const{
		// Go throught the sorted map and print num and number of occurances
		for(auto it = inner_map.begin(); it != inner_map.end(); ++it){
			cout << (*it).first << ": " << (*it).second;
			if ( it != inner_map.begin ()) cout << ", ";
		}
		cout << endl;
	}

	void printAbs() const{
		// Go throught multiset and print whatever is first because we have it sorted correctly
		for(auto it = inner_set.begin(); it != inner_set.end(); ++it){
			cout << (*it);
			if ( it != inner_set.begin ()) cout << ", ";
		}
		cout << endl;
	}

	size_t count(const int num) const{
		try{
			return inner_map.at(num);
		}
		catch (const out_of_range& err){
			return 0;
		}
	}

};


int main(int argc, char *argv[])
{
	CSequence s;
	s.insert(1).insert(2).insert(3).insert(-3).insert(3).insert(-3).insert(3);
	s.insert(-99);
	cout << s[3] << endl;
	cout << s << endl;
	cout << (int)s.count(0) << endl;
	cout << (int)s.count(3) << endl;
	s.printCollissions();
	s.printAbs();
	return 0;
}
