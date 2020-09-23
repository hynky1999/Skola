#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <functional>
#define maxn 100005
typedef long long int ll;
using namespace std;

struct pond{
	static pond* last;
	double volume;
	double flow;
	double timeToDo;
	bool usable;
	pond* next;
	pond* prev;

	pond(double volume,double flow){
		this->volume = volume;
		this->flow = flow;
		this->timeToDo = volume/flow;
		this->usable = true;
		this->next = nullptr;
		this->prev= nullptr;
		if(last){
			last->next = this;
			this->prev = last;
		}
		last = this;
	}
	pond(pond & a, double time){
		this->volume = a.next->volume - a.next->flow * time;
		if(this->volume <= 0)
			this->volume = 0;
		this->flow = a.next->flow + a.flow;
		this->timeToDo = time + this->volume/this->flow;
		this->prev = a.prev;
		this->next = a.next->next;
		this->usable = true;
		last = this;
	}

};

pond* pond::last = nullptr;

bool cmp(pond& p1, pond& p2){
	return p1.timeToDo > p2.timeToDo;
}

int main()
{
	priority_queue<pond, vector<pond>, decltype(&cmp)> tanks(cmp);
	int n;
	double flow;
	double volume;
	while(true){
		cin >> n >> flow;
		if(!cin.good())
			break;
		for(int i = 0; i < n; i++){
			cin >> volume;
			tanks.emplace(pond(volume, flow));
		}
		double time = 0;
		double lastTime = 0;
		while(!tanks.empty()){
			pond a = tanks.top();
			if(a.usable == false){
				tanks.pop();
				delete &a;
				continue;
			}
			time = a.timeToDo;
			if(a.next != nullptr){
				tanks.emplace(pond(a, time));
				a.next->usable = false;
				if(a.next->next)
					a.next->next->prev = pond::last;
			}
			else{
				if (lastTime == 0)
					lastTime = time;
				if(a.prev != nullptr)
					a.prev->next = nullptr;
			}
			tanks.pop();
		}
		cout << lastTime << endl;
		cout << time << endl;
	}
	return 0;
}

