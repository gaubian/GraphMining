#include <bits/stdc++.h>

using namespace std;
using VI = vector<int>;
using VVI = vector<VI>;
using graph = VVI;

VVI listing_aux(graph &G, int lastu, VI &deg, VI &label, int l) {
    VVI ans;
    if(l == 2) {
	for(int i = 0; i < deg[lastu]; ++i) {
	    int u = G[lastu][i];
	    for(int j = 0; j < deg[u]; ++j) ans.push_back({u, G[u][j]});
	}
	return ans;
    }

    for(int i = 0; i < deg[lastu]; ++i) {
	int u = G[lastu][i];
	for(int j = 0; j < deg[u]; ++j) label[G[u][j]] = l - 1;
	for(int j = 0; j < deg[u]; ++j) {
	    int v = G[u][j];
	    int d = 0;
	    for(int k = 0; k < deg[v]; ++k) if(label[G[v][k]] == l - 1) {
		    int z = G[v][k];
		    G[v][k] = G[v][d];				
		    G[v][d] = z;			
		    d++;	
	    }
	    deg[v] = d;	
	}	
	for(VI ve : listing_aux(G, u, deg, label, l-1)) {
	    ve.push_back(u);
	    ans.push_back(ve);			
	}	
	for(int j = 0; j < deg[u]; ++j) label[G[u][j]] = l;
	for(int j = 0; j < deg[u]; ++j) {
	    int v = G[u][j];					
	    int d = 0;				
	    while(d < G[v].size() && label[G[v][d]] == l) d++;
	    deg[v] = d;
	}			
    }
    return ans;
}

VVI listing(graph &G, int k) {
    int n = G.size();
    VI all(n), label(n+1, k), deg(n);
    for(int i = 0; i < n; ++i) {			    
    	all[i] = i;	
	deg[i] = G[i].size();		
    }		
    G.push_back(all);
    deg.push_back(all.size());		
    return listing_aux(G, n, deg, label, k);
}

double score(vector<int> &te, graph &G) {
    set<int> se(te.begin(), te.end());
    int e = 0;
    for(int u : se) for(int v : G[u]) if(se.find(v) != se.end()) ++e;
    return ((double) e)/(((double) se.size())*((double) (se.size() - 1)));
}

double score(int nedg, int nver) {
    return 2 * ((double) nedg) / (((double) nver) * ((double) nver - 1));
}

void update_cliques(vector<unordered_set<int>> &rev_cliques, VVI& cliques, priority_queue<pair<int, int>> &D, int u) {
    unordered_set<int> temp;
    for(int c : rev_cliques[u]) for(int v : cliques[c]) if(u != v) {
	rev_cliques[v].erase(c);
	temp.insert(v);
    }
    for(int v : temp) D.push({-rev_cliques[v].size(), v});

}

int count_edges(graph &G) {
    int m = 0;
    for(auto neighbours : G) m += neighbours.size();
    m /= 2;
    return m;
}

vector<unordered_set<int>> reverse_cliques(VVI &cliques, int n) {
    vector<unordered_set<int>> rev_cliques(n);
    for(int c = 0; c < cliques.size(); ++c)
	for(int u : cliques[c]) rev_cliques[u].insert(c);
    return rev_cliques;
}

VI solve_by_cliques(graph &G, VVI &cliques, int k, int s) {
    int n = G.size(), m = count_edges(G);
    pair<double, int> best = {score(m,n), -1};
    vector<bool> seen(n, false);
    auto rev_cliques = reverse_cliques(cliques, n);
    priority_queue<pair<int,int>> D;
    for(int u = 0; u < n; ++u) D.push({-rev_cliques[u].size(), u});
    VI order_removed(n, 0);
    for(int i = n; i > s;) {
	int u = D.top().second;
	D.pop();
	if(seen[u]) continue;
	seen[u] = true;
	order_removed[u] = --i;
	for(int v : G[u]) if(!seen[v]) m--;
	update_cliques(rev_cliques, cliques, D, u);
	best = max(best, {score(m,i), i});
    }
    VI ans;
    for(int i = 0; i <= n; ++i) if(order_removed[i] < best.second)
	ans.push_back(i);
    return ans;
}

pair<graph, graph> parse() {
    int n, u, v;
    cin >> n;
    graph G(n), oriG(n);
    while(cin >> u >> v) {
	G[u].push_back(v);
	G[v].push_back(u);
	oriG[min(u, v)].push_back(max(u, v));
    }
    return {G, oriG};
}

int main() {
    auto start = chrono::steady_clock::now();
    int k = 3, s = 4;
    auto parsed = parse();
    graph G = parsed.first, oriG = parsed.second;
    VVI cliques = listing(oriG, k);
    auto se = solve_by_cliques(G, cliques, k, s);
    assert(se.size() == 57);
    auto end = chrono::steady_clock::now();
    cout << "The found subgraph :\n"
    << " has size " << se.size() << "\n"
    << " has clique density " << score(se, G) << "\n"
    << " was found in "
    << chrono::duration_cast<chrono::seconds>(end - start).count()
    << " seconds\n";
}
