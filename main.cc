#include <bits/stdc++.h>

using namespace std;

vector<vector<int>> f(set<int> &V, vector<vector<int>> &G, int l) {
    if(l == 0) return {{}};
    vector<vector<int>> ans;
    for(int u : V) {
        set<int> I;
	for(int v : G[u]) if(v > u && V.find(v) != V.end()) I.insert(v);
    	for(vector<int> se : f(I, G, l-1)) {
	    se.push_back(u);
	    ans.push_back(se);
	}
    }
    return ans;
}

set<int> g(vector<vector<int>> &G, int k, int s) {
    int n = G.size(), m = 0, best_i = -1;
    for(auto neighbours : G) m += neighbours.size();
    m /= 2;
    double best_sc = ((double) m) / (((double) n) * ((double) n - 1));
    set<int> V, seen;
    for(int i = 0; i < n; ++i) V.insert(i);
    auto cliques = f(V, G, k);
    vector<set<int>> which_cliques(n);
    for(int c = 0; c < cliques.size(); ++c)
	for(int u : cliques[c]) which_cliques[u].insert(c);
    set<pair<int, int>> D;
    for(int u = 0; u < n; ++u) D.insert(make_pair(which_cliques[u].size(), u));
    vector<int> order_removed;
    for(int i = n - 1; i >= s; --i) {
	//cout << best_sc << endl;
	int u = (*(D.begin())).second;
	//cout << (*(D.rbegin())).first << ' ' << (*(D.begin())).first << endl;
	D.erase(D.begin());
	order_removed.push_back(u);
	seen.insert(u);
	for(int v : G[u]) if(seen.find(v) == seen.end()) m--;
	for(int c : which_cliques[u]) for(int v : cliques[c]) if(u != v) {
	    D.erase(make_pair(which_cliques[v].size(), v));
	    which_cliques[v].erase(c);
	    D.insert(make_pair(which_cliques[v].size(), v));
	}
	//cout << m << ' ' << i << endl;
	double sc = ((double) m) / (((double) i) * ((double) i - 1));
	//cout << i << ' ' << m << ' ' << sc << ' ' << best_sc << endl;
	if(sc > best_sc) {
	    best_sc = sc;
	    best_i = n - i - 1;
	}
    }
    for(int i = 0; i <= best_i; ++i) V.erase(order_removed[i]);
    return V;
}

int main() {
    int n, k = 3, s = 10, u, v;
    cin >> n;
    vector<vector<int>> G(n);
    while(cin >> u >> v) {
	G[u].push_back(v);
	G[v].push_back(u);
    }
    set<int> se = g(G, k, s);
    for(int x : se) cout << x << endl;
}
