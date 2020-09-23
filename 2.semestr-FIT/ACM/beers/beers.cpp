#include <iostream>
#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;
typedef double ld;
// push_back insert lower_bound upper_bound erase

//segtree
#define L(a) (a*2)
#define R(a) (a*2|1)
#define LEAF (lb+1==rb)
#define OUT  (l>=rb || lb>=r)
#define FULL (l<=lb && rb<=r)
// this seg tree solves range minimum queries
#define INF 1000000000l
#define MAXN 100500
#define COMBINE(a, b) (min(a, b))

class SegmentTree{
public:
    ll *arr, *ans;
    ll n;

    SegmentTree(int n, ll* arr){
        ll POW = pow(2, ceil(log2(n)));
        this->arr = arr;
    	ans = new ll[2*POW];
    	
    	this->n = n;
    }

    void build(int at, int lb, int rb){
    	if (LEAF){
    		ans[at] = arr[lb];
			return;
    	}
    	int mid = (lb+rb)/2;
    	build(L(at),lb,mid);
    	build(R(at),mid, rb);
    	ans[at] = ans[L(at)] * ans[R(at)];
    }

    ~SegmentTree(){
    	delete [] arr;
    	delete [] ans;
	}
    // return minimal value in [l,r) or INF if range is invalid
    ll getMin(ll l, ll r){
        return getMin(l, r, 1, 0, n);
    }
    ll getMin(int l, int r, int idx, int lb, int rb){
        if(OUT) return 1;
        if(FULL) return ans[idx];
        ll mid = (lb + rb)/2;
        return getMin(l, r, L(idx), lb, mid) * getMin(l, r, R(idx), mid, rb);
    }
    // add val to all elements in [l,r)
    void add(int l, int r, int val){ add(l, r, val, 1, 0, n); }
    void add(int l, int r, int val, int idx, int lb, int rb){
        if(OUT) return;
        if(LEAF) { ans[idx]=val; return; }
        ll mid = (lb + rb)/2;
        add(l, r, val, L(idx), lb, mid);
        add(l, r, val, R(idx), mid, rb);
        ans[idx] = ans[L(idx)] * ans[R(idx)];
    }
};
///segtree

int main() {
    int x,n,q, lr, rg, v;
	char f;
    ll* arr= new ll[MAXN];
    SegmentTree sg_tree(MAXN, arr);
    while(cin >> n >> q){
		for (int i = 0; i < n; ++i) {
			cin >> x;
			x = x < 0 ? -1: x >0 ? 1 : 0;
			sg_tree.arr[i] = x;
		}
		sg_tree.n = n;
		sg_tree.build(1, 0, n);
		for (;q > 0; q--) {
			cin.ignore(256,'\n');
			f = cin.get();
			cin >> lr >> rg;
			/* cout << f << " " << lr-1 << " " << rg << endl; */
			if( f == 'C'){
				rg = rg < 0 ? -1 : rg > 0 ? 1 : 0;
				sg_tree.add(lr-1, lr, rg);
			}
			else{
				v = sg_tree.getMin(lr-1, rg);
				cout << (v < 0 ? "-": v >0 ? "+" : "0");
			}
		}
		cout << endl;
	}
    return 0;

}
