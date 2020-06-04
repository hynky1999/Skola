
#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */
int main(int argc, char *argv[])
{
	istringstream is("ahoj\n\nahoj");
	while(!is.eof()){
		string line;
		getline(is, line);
		cout << line.size();
	}
	string a = string(10, ' ');
	cout << a;
	return 0;
}
