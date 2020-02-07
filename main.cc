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
    double best_sc = ((double) m) / ((double) (n * (n - 1)));
    set<int> V;
    for(int i = 0; i < n; ++i) V.insert(i);
    auto cliques = f(V, G, k);
    vector<set<int>> which_cliques(n);
    for(int c = 0; c < cliques.size(); ++c)
	for(int u : cliques[c]) which_cliques[u].insert(c);
    set<pair<int, int>> D;
    for(int u = 0; u < n; ++u) D.insert(make_pair(cliques[u].size(), u));
    vector<int> order_removed;
    for(int i = n - 1; i >= s; --i) {
	int u = (*(D.begin())).second;
	order_removed.push_back(u);
	D.erase(D.begin());
	for(int c : which_cliques[u]) for(int v : cliques[c]) if(u != v) {
	    D.erase(make_pair(which_cliques[v].size(), v));
	    which_cliques[v].erase(c);
	    D.insert(make_pair(which_cliques[v].size(), v));
	}
	m -= G[u].size();
	if(((double) m) / ((double) (n * (n - 1))) > best_sc) {
	    best_sc = ((double) m) / ((double) (n * (n - 1)));
	    best_i = n - 1 - i;
	}
    }
    for(int i = 0; i <= best_i; ++i) V.erase(order_removed[i]);
    return V;
}

int main() {
    int n, k, s, u, v;
    cin >> n >> k >> s;
    vector<vector<int>> G(n);
    while(cin >> u >> v) G[min(u, v)].push_back(max(u, v));
    set<int> se = g(G, k, s);
    for(int x : se) cout << x << endl;
}
