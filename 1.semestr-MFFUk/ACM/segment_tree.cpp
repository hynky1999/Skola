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
#define COMBINE(a, b) (min(a, b))

class SegmentTree{
public:
    ll *arr, *ans, *lazy;
    ll n;

    SegmentTree(int n, ll* arr){
        ll POW = pow(2, ceil(log2(n)));
        this->arr = arr;
    	ans = new ll[2*POW];
    	lazy = new ll[2*POW];
    	
    	this->n = n;
    }

    void build(int at, int lb, int rb){
    	lazy[at] = 0;
    	if (LEAF){
    		ans[at] = arr[lb];
			return;
    	}
    	int mid = (lb+rb)/2;
    	build(L(at),lb,mid);
    	build(R(at),mid, rb);
    	ans[at] = COMBINE(ans[L(at)],ans[R(at)]);
    }

    ~SegmentTree(){
    	delete [] arr;
    	delete [] ans;
    	delete [] lazy;
	}

    ll realVal(ll idx){ return ans[idx]+lazy[idx]; }
    void applyLazy(ll idx){
        if(lazy[idx]!=0){
            lazy[L(idx)]+=lazy[idx];
            lazy[R(idx)]+=lazy[idx];
            ans[idx]=realVal(idx);
            lazy[idx]=0;
        }
    }
    // return minimal value in [l,r) or INF if range is invalid
    ll getMin(ll l, ll r){
        return getMin(l, r, 1, 0, n);
    }
    ll getMin(int l, int r, int idx, int lb, int rb){
        if(OUT) return INF;
        if(LEAF) return realVal(idx);
        if(FULL) return realVal(idx);
        applyLazy(idx);
        ll mid = (lb + rb)/2;
        return COMBINE(getMin(l, r, L(idx), lb, mid), getMin(l, r, R(idx), mid, rb));
    }
    // add val to all elements in [l,r)
    void add(int l, int r, int val){ add(l, r, val, 1, 0, n); }
    void add(int l, int r, int val, int idx, int lb, int rb){
        if(OUT) return;
        if(LEAF) { ans[idx]+=val; return; }
        if(FULL) { lazy[idx]+=val; return; }
        applyLazy(idx);

        ll mid = (lb + rb)/2;
        add(l, r, val, L(idx), lb, mid);
        add(l, r, val, R(idx), mid, rb);
        ans[idx] = COMBINE(realVal(L(idx)), realVal(R(idx)));
    }
};
///segtree

int main() {
    ll x,n, lr, rg, m, v;
    cin >> n;
    ll* my_vect = new ll[n];
    for (ll i = 0; i < n; ++i) {
        cin >> x;
        my_vect[i] = x;
    }
    SegmentTree sg_tree(n,  my_vect);
    sg_tree.build(1,0,n);
    cin >> m;
    cin.ignore();
    string s;
    for (;m > 0; m--) {
        getline( cin, s);
        istringstream is(s);
        if(is >> lr >> rg >> v){
            if(rg < lr){
					sg_tree.add(0, rg+1, v);
					sg_tree.add(lr, n, v);
            }else {
            	sg_tree.add(lr, rg+1, v);
            }
        }else{
            if(rg < lr){
				cout << min(sg_tree.getMin(0, rg+1), sg_tree.getMin(lr, n)) << "\n";
            } else  {
                cout << sg_tree.getMin(lr, rg+1) << "\n";
            }
        }



    }
    return 0;

}
