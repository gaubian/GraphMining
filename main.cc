#include <bits/stdc++.h>

using namespace std;
using VI = vector<int>;
using VVI = vector<VI>;
using graph = VVI;

const int k = 3;
const int s = 10;

/*
 * Takes as input a graph, a vertex lastu, a vector assigning to each
 * vertex its degree, a vector associating to each vertex its label and
 * the current clique-size l we're looking for amongst lastu's neighbours.
 */
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

/*
 * Takes as input a graph G and an integer k, and returns all cliques of
 * size k.
 */
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

/*
 * Takes as input a graph G and a subset of its vertices and returns its
 * clique-density.
 */
double score(vector<int> &te, graph &G) {
    set<int> se(te.begin(), te.end());
    int e = 0;
    for(int u : se) for(int v : G[u]) if(se.find(v) != se.end()) ++e;
    return ((double) e)/(((double) se.size())*((double) (se.size() - 1)));
}

/*
 * Takes as inputs a number of edges and vertices, and returns
 * corresponding clique-density.
 */ 
double score(int nedg, int nver) {
    return 2 * ((double) nedg) / (((double) nver) * ((double) nver - 1));
}

/*
 * Takes as input an array of cliques, an array assigning to each vertex
 * its list of cliques, a vertex u, and update arrays considering we
 * erase u from the graph.
 */
void update_cliques(vector<unordered_set<int>> &rev_cliques, VVI& cliques, priority_queue<pair<int, int>> &D, int u) {
    unordered_set<int> temp;
    for(int c : rev_cliques[u]) for(int v : cliques[c]) if(u != v) {
        rev_cliques[v].erase(c);
        temp.insert(v);
    }
    for(int v : temp) D.push({-rev_cliques[v].size(), v});
}

/*
 * Takes as input a graph, and returns its number of edges.
 */
int count_edges(graph &G) {
    int m = 0;
    for(auto neighbours : G) m += neighbours.size();
    m /= 2;
    return m;
}

/*
 * Takes as input a cliques and returns an array assigning to each vertex
 * the list of cliques it is in.
 */
vector<unordered_set<int>> reverse_cliques(VVI &cliques, int n) {
    vector<unordered_set<int>> rev_cliques(n);
    for(int c = 0; c < cliques.size(); ++c) for(int u : cliques[c]) rev_cliques[u].insert(c);
    return rev_cliques;
}

/*
 * Takes as input a graph, a list of cliques, a minimum size s, and
 * returns a subset of vertices corresponding to a dense subgraph
 * according to the algorithm described in the project's subject.
 */
VI solve_by_cliques(graph &G, VVI &cliques, int s) {
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
    for(int i = 0; i < n; ++i) if(order_removed[i] < best.second)
        ans.push_back(i);
    return ans;
}

/*
 * My heuristic for solving the graph. Takes as input a graph, the list
 * of cliques, s and a score found so far and return a solution
 */
VI improve_heuristics(graph &G, VVI &cliques, int s, double B) {
    int n = G.size(), m = count_edges(G);
    pair<double, int> best = {score(m,n), -1};
    vector<bool> seen(n, false);
    auto rev_cliques = reverse_cliques(cliques, n);
    priority_queue<pair<int,int>> D;
    vector<unordered_set<int>> Gset(n);
    priority_queue<pair<int,int>> degrees;
    for(int u = 0; u < n; ++u) {
        Gset[u] = unordered_set<int>(G[u].begin(), G[u].end());
        degrees.push({-G[u].size(), u});
    }
    for(int u = 0; u < n; ++u) D.push({-rev_cliques[u].size(), u});
    VI order_removed(n, 0);
    for(int i = n; i > s;) {
        int u = degrees.top().second;
        if(seen[u]) {
            degrees.pop();
            continue;
        }
        int delta = Gset[u].size();
        if(delta >= B*s) {
            u = D.top().second;
            D.pop();
        }
        if(seen[u]) continue;
        seen[u] = true;
        order_removed[u] = --i;
        for(int v : G[u]) if(!seen[v]) {
            m--;
            Gset[v].erase(u);
            degrees.push({-G[v].size(), v});
        }
        update_cliques(rev_cliques, cliques, D, u);
        best = max(best, {score(m,i), i});
    }
    VI ans;
    for(int i = 0; i < n; ++i) if(order_removed[i] < best.second)
        ans.push_back(i);
    return ans;
}

/*
 * My heuristic for solving the graph. Takes as input a graph, the list
 * of cliques and s and return a solution
 */
VI my_heuristics(graph &G, VVI &cliques, int s) {
    pair<double, VI> best_so_far = {0, vector<int>(0)};
    while(true) {
        VI sol = improve_heuristics(G, cliques, s, best_so_far.first);
        double sc = score(sol, G);
        if(sc <= best_so_far.first) break;
        best_so_far = {sc, sol};
    }
    return best_so_far.second;
}

/*
 * Parse edges of a graph given as TSV.
 */
vector<pair<int, int>> parse_edges() {
    vector<pair<int, int>> ans;
    string line;
    while(getline(cin, line)) {
        if(line[0] == '%') continue;
        stringstream line_stream(line);
        int u, v;
        line_stream >> u >> v;
        ans.push_back({u, v});
    }
    return ans;
}

/*
 * Given a list of edges in a TSV file, output number of vertices.
 */
int count_vertices(vector<pair<int, int>> &edges) {
    int n = 0;
    for(auto edge : edges) n = max(n, max(edge.first, edge.second));
    return n + 1;
}

/*
 * Parse a graph given on standard input as TSV. Returns corresponding
 * graph and its oriented counterpart.
 */
pair<graph, graph> parse() {
    auto edges = parse_edges();
    int u, v, n = count_vertices(edges);
    graph G(n), oriG(n);
    for(auto edge : edges) {
        tie(u, v) = edge;
        G[u].push_back(v);
        G[v].push_back(u);
        oriG[min(u, v)].push_back(max(u, v));
    }
    return {G, oriG};
}

int main() {
    auto parsed = parse();
    graph G = parsed.first, oriG = parsed.second;

    cout << "Now using the algorithm of question 1" << endl;
    auto start = chrono::steady_clock::now();
    VVI cliques = listing(oriG, k);
    auto se_by_cliques = solve_by_cliques(G, cliques, s);
    auto end = chrono::steady_clock::now();
    cout << "The found subgraph :\n"
    << " has size " << se_by_cliques.size() << "\n"
    << " has clique density " << score(se_by_cliques, G) << "\n"
    << " was found in "
    << chrono::duration_cast<chrono::seconds>(end - start).count()
    << " seconds\n";

    cout << "Now using my own heuristic (question 3)" << endl;
    auto start_new = chrono::steady_clock::now();
    VVI cliques_new = listing(oriG, k);
    auto se_my_heuristics = my_heuristics(G, cliques, s);
    auto end_new = chrono::steady_clock::now();
    cout << "The found subgraph :\n"
    << " has size " << se_my_heuristics.size() << "\n"
    << " has clique density " << score(se_my_heuristics, G) << "\n"
    << " was found in "
    << chrono::duration_cast<chrono::seconds>(end_new - start_new).count()
    << " seconds\n";
}
