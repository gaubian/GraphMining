#include <bits/stdc++.h>

using namespace std;

void date(string s) {
    time_t result = time(nullptr);
    cout << s << ' ' << asctime(localtime(&result)) << endl;
}

vector<vector<int>> f(const vector<int> &V, vector<vector<int>> &G, int l) {
    if(l == 0) return {{}};
    vector<vector<int>> ans;
    for(int u : V) {
	vector<int> I;
    	for(int v : G[u]) if(binary_search(V.begin(), V.end(), v)) I.push_back(v);
    	for(vector<int> se : f(I, G, l-1)) {
	    se.push_back(u);
	    ans.push_back(se);
	}
    }
    return ans;
}

unordered_set<int> g(vector<vector<int>> &G, vector<vector<int>> &ori, int k, int s) {
    int n = G.size(), m = 0, best_i = -1;
    for(auto neighbours : G) m += neighbours.size();
    m /= 2;
    double best_sc = ((double) m) / (((double) n) * ((double) n - 1));
    vector<int> V;
    vector<bool> seen(n, false);
    for(int i = 0; i < n; ++i) V.push_back(i);
    date("startf");
    auto cliques = f(V, ori, k);
    date("endf");
    vector<unordered_set<int>> revcliques(n);
    for(int c = 0; c < cliques.size(); ++c)
	for(int u : cliques[c]) revcliques[u].insert(c);
    priority_queue<pair<int,int>> D;
    for(int u = 0; u < n; ++u) D.push(make_pair(-revcliques[u].size(), u));
    vector<int> order_removed;
    date("D");
    for(int i = n; i >= s;) {
	int u = D.top().second;
	D.pop();
	if(seen[u]) continue;
	--i;
	order_removed.push_back(u);
	seen[u] = true;
	for(int v : G[u]) if(!seen[v]) m--;
	unordered_set<int> temp;
	for(int c : revcliques[u]) for(int v : cliques[c]) if(u != v) {
	    revcliques[v].erase(c);
	    temp.insert(v);
	}
	for(int v : temp) D.push(make_pair(-revcliques[v].size(), v));
	double sc = ((double) m) / (((double) i) * ((double) i - 1));
	if(sc > best_sc) {
	    best_sc = sc;
	    best_i = n - i - 1;
	}
    }
    unordered_set<int> VV(V.begin(), V.end());
    for(int i = 0; i <= best_i; ++i) VV.erase(order_removed[i]);
    return VV;
}

int main() {
    int n, k = 3, s = 4, u, v;
    cin >> n;
    vector<vector<int>> G(n), ori(n);
    date("PARSE");
    while(cin >> u >> v) {
	G[u].push_back(v);
	G[v].push_back(u);
	ori[min(u, v)].push_back(max(u, v));
    }
    for(int u = 0; u < G.size(); ++u) sort(ori[u].begin(), ori[u].end());
    date("g");
    unordered_set<int> se = g(G, ori, k, s);
    date("END");
    for(int x : se) cout << x << endl;
}

