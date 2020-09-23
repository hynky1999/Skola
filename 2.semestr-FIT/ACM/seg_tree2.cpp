
#include <iostream>
#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;
typedef double ld;
// push_back insert lower_bound upper_bound erase

//segtree
#define L(a) (a*2)
#define R(a) (a*2|1)
#define LEAF (idx>=POW)
#define OUT  (l>=rb || lb>=r)
#define FULL (l<=lb && (rb<=r || ( rb == POW && (rb-added) <= r)))
// this seg tree solves range minimum queries
#define INF 10000000l
#define COMBINE(a, b) (min(a, b))

class SegmentTree{
public:
    ll N, POW, added;
    ll* a;
    ll* lazy;

    SegmentTree(ll n, ll* &p):N(n){
        POW = pow(2, ceil(log2(N)));
        added = POW-N;
        a = new ll[2*POW];
        lazy = new ll[2*POW];
        for(ll i=POW, j=0; i<POW+N; ++i, ++j){ 
        	a[i]=p[j];
        	lazy[i] = 0;
        }
        for(ll i=POW+N; i < POW*2; i++){
        	lazy[i] = 0;
        	a[i] = INF;
		}
        for(ll i=POW-1; i>0; --i){
        	lazy[i] = 0;
            a[i] = COMBINE(realVal(L(i)), realVal(R(i)));
        }
    }

    ~SegmentTree(){
    	delete [] a;
    	delete [] lazy;
	}

    ll realVal(ll idx){ return a[idx]+lazy[idx]; }
    void applyLazy(ll idx){
        if(lazy[idx]!=0){
            lazy[L(idx)]+=lazy[idx];
            lazy[R(idx)]+=lazy[idx];
            a[idx]=realVal(idx);
            lazy[idx]=0;
        }
    }
    // return minimal value in [l,r) or INF if range is invalid
    ll getMin(ll l, ll r){
        return getMin(l, r, 1, 0, POW);
    }
    ll getMin(ll l, ll r, ll idx, ll lb, ll rb){
        if(OUT) return INF;
        if(LEAF) return realVal(idx);
        if(FULL) return realVal(idx);
        applyLazy(idx);
        ll mid = (lb + rb)/2;
        return COMBINE(getMin(l, r, L(idx), lb, mid), getMin(l, r, R(idx), mid, rb));
    }
    // add val to all elements in [l,r)
    void add(ll l, ll r, ll val){ add(l, r, val, 1, 0, POW); }
    void add(ll l, ll r, ll val, ll idx, ll lb, ll rb){
        if(OUT) return;
        if(LEAF) { a[idx]+=val; return; }
        if(FULL) { lazy[idx]+=val; return; }
        applyLazy(idx);

        ll mid = (lb + rb)/2;
        add(l, r, val, L(idx), lb, mid);
        add(l, r, val, R(idx), mid, rb);
        a[idx] = COMBINE(realVal(L(idx)), realVal(R(idx)));
    }
};
///segtree

int main() {
    ll x,n, lr, rg, m, var, v;
    cin >> n;
    ll* my_vect = new ll[n];
    for (ll i = 0; i < n; ++i) {
        cin >> x;
        my_vect[i] = x;
    }
    SegmentTree sg_tree(n,  my_vect);
    cin >> m;
    string s;
    for (ll i = 0; i < m;) {
        vector<ll> input_vec;
        getline( cin, s);
        istringstream is(s);
        while(is >> var)
			input_vec.push_back(var);
		if(input_vec.size() == 0 || input_vec.size() == 1)
			continue;
		i++;

		lr = input_vec[0];
		rg = input_vec[1];
        if(input_vec.size() == 3){
        	v = input_vec[2];
            if(rg < lr){
					sg_tree.add(0, rg+1, v);
					sg_tree.add(lr, n, v);

            }else {
            	sg_tree.add(lr, rg+1, v);
            }

            // rmq
        }else{
            if(rg < lr){
				cout << min(sg_tree.getMin(0, rg+1), sg_tree.getMin(lr, n)) << "\n";
            } else  {
                cout << sg_tree.getMin(lr, rg+1) << "\n";
            }
        }



    }
    delete [] my_vect;
    return 0;

}
