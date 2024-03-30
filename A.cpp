#include <bits/stdc++.h>
#include <chrono>
 
using namespace std;

mt19937_64 rng(133769420);

#define DB(X) { cout << #X << " = " << (X) << '\n'; }
#define DB1(A, _) { cout << #A << "[" << _ << "] = " << (A[_]) << '\n'; }
#define DB2(A, _, __) { cout << #A << "[" << _ << "][" << __ << "] = " << (A[_][__]) << '\n'; }
#define DB3(A, _, __, ___) { cout << #A << "[" << _ << "][" << __ << "][" << ___ << "] = " << (A[_][__][___]) << '\n'; }
#define PR(A, l, r) { cout << '\n'; for (int _ = l; _ <= r; ++_) DB1(A, _); cout << '\n';}
#define sz(x) ((int) (x).size())
#define all(v) (v).begin(), (v).end()
#define fi first
#define se second
 
using ll = long long;
using ld = long double;
using ii = pair<int, int>;
using vi = vector<int>;

const int NUM_TILES = 16;
const int BASE = 317;
int n, T;

unsigned long long randomInt()
{
    uniform_int_distribution<unsigned long long> dist(0, UINT64_MAX);
    return dist(rng);
}

int conv(char x) {
    if (x >= '0' && x <= '9') {
        return x - '0';
    }
    if (x >= 'a' && x <= 'f') {
        return 10 + (x - 'a');
    }
    return -1;
}
vector<vector<int>> convertStrToGraph(const vector<string> &s) {
    vector<vector<int>> adj(n, vi(n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            adj[i][j] = conv(s[i][j]);
        }
    }
    return adj;
}
int dx[] = {0, -1, 0, 1}, dy[] = {-1, 0, 1, 0};
string dir = "LURD";
int corr[] = {2, 3, 0, 1};
bool dfs(int i, int j, int pi, int pj, int &numVertices, const vector<vector<int>> &adj, vector<vector<bool>> &vis) {
    if (vis[i][j]) {
        return false;
    }
    numVertices++;
    vis[i][j] = true;
    for (int t = 0; t < 4; ++t) {
        if (~adj[i][j] >> t & 1) {
            continue;
        }
        int x = i + dx[t];
        int y = j + dy[t];
        if (x < 0 || y < 0 || x == n || y == n) {
            continue;
        }
        if (~adj[x][y] >> corr[t] & 1) {
            continue;
        }
        if (x == pi && y == pj) {
            continue;
        }
        if (!dfs(x, y, i, j, numVertices, adj, vis)) {
            return false;
        }
    }
    return true;
}
int getAnswer(const vector<vector<int>> &adj) {
    vector<vector<bool>> vis(n, vector<bool> (n, false));
    int mxVertices = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int numVertices = 0;
            if (vis[i][j] == false && dfs(i, j, -1, -1, numVertices, adj, vis)) {
                mxVertices = max(mxVertices, numVertices);
            }
        }
    }
    return mxVertices;
}

int getHeuristics(int pathLength, const vector<vector<int>> &adj) {
    vector<vector<bool>> vis(n, vector<bool> (n, false));
    int mxVertices = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int numVertices = 0;
            if (vis[i][j] == false && dfs(i, j, -1, -1, numVertices, adj, vis)) {
                mxVertices = max(mxVertices, numVertices);
            }
        }
    }
    return -pathLength + 19 * mxVertices;
}
vector<vector<vector<unsigned long long>>> zobrist;
vector<pair<int, int>> trace;

struct Node {
    unsigned long long zobristHash;
    vector<vector<int>> adj;
    int emptyx, emptyy;
    int posInTrace;
    int pathLength;
    Node() {}
    Node(vector<vector<int>> &_adj, int _ex, int _ey, bool hashExisted, unsigned long long hash = 0) : adj(_adj), emptyx(_ex), emptyy(_ey), zobristHash(hash) {
        posInTrace = sz(trace);
        trace.push_back({-1, -1});
        pathLength = 0;
        if (!hashExisted) {
            updHash();
        }
    }
    bool operator<(const Node &x) const {
        return getHeuristicsNode() > x.getHeuristicsNode();
    }

    int getAnswerNode() {
        return getAnswer(adj);
    }

    int getHeuristicsNode() const {
        return getHeuristics(pathLength, adj);
    }

    void updHash() {
        zobristHash = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                zobristHash ^= zobrist[i][j][adj[i][j]];
            }
        }
    }

    vector<Node> getNeighbors() {
        vector<Node> neighbors;
        if (pathLength == T) {
            return {};
        }
        int i = emptyx, j = emptyy;
        for (int t = 0; t < 4; ++t) {
            int x = i + dx[t];
            int y = j + dy[t];
            if (x < 0 || y < 0 || x == n || y == n) {
                continue;
            }
            vector<vector<int>> newAdj = adj;
            swap(newAdj[x][y], newAdj[i][j]);
            unsigned long long newZobristHash = zobristHash;
            newZobristHash ^= zobrist[i][j][adj[i][j]] ^ zobrist[x][y][adj[x][y]];
            newZobristHash ^= zobrist[i][j][newAdj[i][j]] ^ zobrist[x][y][newAdj[x][y]];
            Node newNeighbor = Node(newAdj, x, y, true, newZobristHash);
            newNeighbor.pathLength = pathLength + 1;
            trace[newNeighbor.posInTrace] = {posInTrace, t};
            neighbors.push_back(newNeighbor);
        }
        return neighbors;
    }
};

void initZobrist(int n) {
    zobrist.assign(n, vector<vector<unsigned long long>>(n, vector<unsigned long long>(NUM_TILES, 0)));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < NUM_TILES; ++k) {
                zobrist[i][j][k] = randomInt();
            }
        }
    }
}

struct CompareNode {
    bool operator()(Node &x, Node &y) {
        return x.getHeuristicsNode() < y.getHeuristicsNode();
    }
};
map<ll, bool> visited;
int main() {
    cin.tie(0)->sync_with_stdio(0);
    
    cin >> n >> T;
    auto start = chrono::steady_clock::now();
    initZobrist(n);
    int MAX_NODE = n * 100;
    vector<string> s(n);
    int ex = 0, ey = 0;
    for (int i = 0; i < n; ++i) {
        cin >> s[i];
        for (int j = 0; j < n; ++j) {
            if (s[i][j] == '0') {
                ex = i;
                ey = j;
            }
        }
    }
    auto adj = convertStrToGraph(s);
    Node root = Node(adj, ex, ey, false);
    // priority_queue<Node, vector<Node>, CompareNode> pq;
    set<Node> pq; 
    pq.insert(root);
    visited[root.zobristHash] = true;
    int mxAns = -1;
    int bestAns = -1;
    while (mxAns < n * n - 1 && !pq.empty()) {
        auto ed = chrono::steady_clock::now();
        auto diff = ed - start;
        if (chrono::duration<double, milli> (diff).count() > 2960) {
            break;
        }
        Node cur = *pq.begin();
        int ans = cur.getAnswerNode();
        if (mxAns < ans) {
            mxAns = ans;
            bestAns = cur.posInTrace;
        }
        pq.erase(pq.begin());
        vector<Node> neighbors = cur.getNeighbors();
        for (Node &nei : neighbors) {
            if (!visited[nei.zobristHash]) {
                pq.insert(nei);
                visited[nei.zobristHash] = true;
            }
        }
        while (pq.size() > MAX_NODE) {
            auto it = pq.end();
            it--;
            visited[it->zobristHash] = false;
            pq.erase(it);
        }
    }

    string ans = "";
    while (bestAns > 0) {
        ans = ans + dir[trace[bestAns].second];
        bestAns = trace[bestAns].first;
    }
    reverse(all(ans));
    cout << ans;
    return 0;
}
