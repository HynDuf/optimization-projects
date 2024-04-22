#include <bits/stdc++.h>

using namespace std;
#define sz(x) ((int) (x).size())
#define all(v) (v).begin(), (v).end()
#define fi first
#define se second
#define rd(l, r) uniform_int_distribution<int>(l, r)(rng)
#define rdReal() uniform_real_distribution<double>(0.0, 1.0)(rng)

using ll = long long;
using ii = pair<int, int>;
using vi = vector<int>;

int n, T;
vector<vector<int>> globalAdj;
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

int getScore(int pathLength, const vector<vector<int>> &adj) {
    int mxTree = getAnswer(adj);
    if (mxTree == n * n - 1) {
        return int(500000.0 * (2 - (double) pathLength / T));
    } else {
        return int(500000.0 * (double) mxTree / (n * n - 1));
    }
}

int manhattanDist[16][11][11];
void buildManhattanDist(const vector<vector<int>> &adj) {
    for (int type = 0; type < 16; ++type) {
        vector<pair<int, int>> pos;
        for (int i = 0; i < n; ++i){
            for (int j = 0; j < n; ++j) {
                if (adj[i][j] == type) {
                    pos.push_back({i, j});
                }
            }
        }
        for (int i = 0; i < n; ++i){
            for (int j = 0; j < n; ++j) {
                manhattanDist[type][i][j] = n * n;
                for (auto &p : pos) {
                    manhattanDist[type][i][j] = min(manhattanDist[type][i][j], abs(p.fi - i) + abs(p.se - j));
                }
            }
        }
    }
}
const int COST_DISCONNECT = 2000;
int getCost(const vector<vector<int>> &adj) {
    int cost = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cost += manhattanDist[adj[i][j]][i][j];
            for (int t = 0; t < 4; ++t) {
                if (~adj[i][j] >> t & 1) {
                    continue;
                }
                int x = i + dx[t], y = j + dy[t];
                if (x < 0 || y < 0 || x == n || y == n) {
                    continue;
                }
                if (~adj[x][y] >> corr[t] & 1) {
                    cost += COST_DISCONNECT;
                }
            }
        }
    }
    return cost;
}

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
pair<int, int> getRandPair() {
    while (true) {
        int x = rd(0, n - 1);
        int y = rd(0, n - 1);
        if (x == n - 1 && y == n - 1) {
            continue;
        }
        return {x, y};
    }
}
pair<int, int> getRandPairDiffType(int type) {
    while (true) {
        auto p = getRandPair();
        if (globalAdj[p.fi][p.se] != type) {
            return {p.fi, p.se};
        }
    }
}
bool isGoodCell(int type, int i, int j) {
    if (j == 0 && (type & 1)) {
        return false;
    }
    if (i == 0 && (type >> 1 & 1)) {
        return false;
    }
    if (j == n - 1 && (type >> 2 & 1)) {
        return false;
    }
    if (i == n - 1 && (type >> 3 & 1)) {
        return false;
    }
    return true;
}
bool isGoodPairToSwap(ii &A, ii &B) {
    return isGoodCell(globalAdj[A.fi][A.se], B.fi, B.se) && isGoodCell(globalAdj[B.fi][B.se], A.fi, A.se);
}

const int ITER_GOOD_PAIR = 10;
pair<ii, ii> getGoodPairOfCells() {
    for (int it = 1; it <= ITER_GOOD_PAIR; ++it) {
        auto p = getRandPair();
        auto q = getRandPairDiffType(globalAdj[p.fi][p.se]);
        if (it == ITER_GOOD_PAIR || isGoodPairToSwap(p, q)) {
            return {p, q};
        }
    }
    return {{}, {}};
}
const int SA_ITERATION = 300000;
pair<vector<vector<int>>, int> simulatedAnnealing(vector<vector<int>> adj) {
    // bestAdj with bestScore
    vector<vector<int>> bestAdj;
    int bestScore = getScore(T, adj);
    int maxTemp = 1000;
    int minTemp = 500;

    // cost is for the annealing process
    int curCost = getCost(adj);
    for (int it = 0; it < SA_ITERATION; ++it) {
        double progress = (double) (it + 1) / SA_ITERATION;
        double temp = maxTemp + (minTemp - maxTemp) * progress;
        auto [p, q] = getGoodPairOfCells();
        swap(adj[p.fi][p.se], adj[q.fi][q.se]);
        int nxtCost = getCost(adj);
        int delta = curCost - nxtCost;
        bool update = false;
        if (delta < 0) {
            double prob = exp(delta / temp);
            double rnd = rdReal();
            if (rnd < prob) {
                update = true;
            }
        } else {
            update = true;
        }
        if (!update) {
            swap(adj[p.fi][p.se], adj[q.fi][q.se]);
            continue;
        }
        int nxtScore = getScore(T, adj);
        if (bestScore < nxtScore) {
            bestScore = nxtScore;
            bestAdj = adj;
            if (bestScore == 500000) {
                break;
            }
        }
    }
    return {bestAdj, bestScore};
}

int32_t main() {
#ifdef HynDuf
    freopen("A.inp", "r", stdin);
    freopen("A.out", "w", stdout);
#else
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
#endif
    
    cin >> n >> T;
    auto start = chrono::steady_clock::now();
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
    for (int i = ey; i < n - 1; ++i) {
        swap(s[ex][i], s[ex][i + 1]);
    }
    for (int i = ex; i < n - 1; ++i) {
        swap(s[i][n - 1], s[i + 1][n - 1]);
    }

    globalAdj = convertStrToGraph(s);
    buildManhattanDist(globalAdj);
    auto bestAdj = globalAdj;
    int bestScore = 0;
    int maxIter = 200;
    int iter = 0;
    while (iter < maxIter) {
        if (iter % 10 == 0) {
            cerr << "iter = " << iter << '\n';
        }
        iter++;
        auto [resultAdj, score] = simulatedAnnealing(bestAdj);
        if (score > bestScore) {
            bestAdj = resultAdj;
            bestScore = score;
            cerr << "bestScore = " << bestScore << endl;
        }
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (bestAdj[i][j] < 10) {
                cout << (char) ('0' + bestAdj[i][j]);
            } else {
                cout << (char) ('a' + (bestAdj[i][j] - 10));
            }
        }
        cout << '\n';
    }
    return 0;
}
/* stuff you should look for
 * int overflow, array bounds
 * special cases (n=1?)
 * do smth instead of nothing and stay organized
 * WRITE STUFF DOWN
 * DON'T GET STUCK ON ONE APPROACH
 */
