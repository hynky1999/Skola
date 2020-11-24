#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
typedef long long int ll;
using namespace std;
struct pond{
	double volume;
	double flow;
	double timeToDo;
	bool usable;
	pond* next;
	pond* prev;
};

bool cmp(pond* & p1, pond* & p2){
	return p1->timeToDo > p2->timeToDo;
}

int main()
{
	priority_queue<pond*, vector<pond*>, decltype(&cmp)> tanks(cmp);
	pond* arr = new pond[400000];
	int n;
	double flow;
	double volume;
	while(true){
		cin >> n >> flow;
		if(!cin.good())
			break;
		ll i = 0;
		for(; i < n; i++){
			cin >> volume;
			arr[i].volume = volume;
			arr[i].flow = flow;
			arr[i].timeToDo = volume/flow;
			arr[i].usable = true;
			arr[i].next = nullptr;
			arr[i].prev = nullptr;
			if(i != 0){
				arr[i-1].next = arr + i;
				arr[i].prev = arr + i -1;
			}
			tanks.push(arr + i);
		}
		double time = 0;
		double lastTime = 0;
		while(!tanks.empty()){
			pond* a = tanks.top();
			if(a->usable == false){
				tanks.pop();
				continue;
			}
			time = a->timeToDo;
			if(a->next != nullptr){
				arr[i].volume = a->next->volume - a->next->flow * time;
				if(arr[i].volume < 0)
					arr[i].volume = 0;
				arr[i].flow = a->next->flow + a->flow;
				arr[i].timeToDo = time + arr[i].volume/arr[i].flow;
				arr[i].prev = a->prev;
				if(arr[i].prev != nullptr)
					(arr[i].prev)->next = arr+i;
				arr[i].next = a->next->next;
				if(arr[i].next != nullptr)
					(arr[i].next)->prev = arr+i;
				arr[i].usable = true;
				tanks.pop();
				tanks.push(arr + i);
				i+=1;
				a->next->usable = false;
			}
			else{
				if (lastTime == 0)
					lastTime = time;
				if(a->prev != nullptr)
					a->prev->next = nullptr;
				tanks.pop();
			}
		}
		cout << lastTime << endl;
		cout << time << endl;
	}
	delete [] arr;
	return 0;
}

