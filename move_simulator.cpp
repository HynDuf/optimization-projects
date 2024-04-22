#include <bits/stdc++.h>
#include <chrono>

using namespace std;

mt19937_64 rng(133769420);

#define DB(X)                                                                  \
    { cout << #X << " = " << (X) << '\n'; }
#define DB1(A, _)                                                              \
    { cout << #A << "[" << _ << "] = " << (A[_]) << '\n'; }
#define DB2(A, _, __)                                                          \
    { cout << #A << "[" << _ << "][" << __ << "] = " << (A[_][__]) << '\n'; }
#define DB3(A, _, __, ___)                                                     \
    {                                                                          \
        cout << #A << "[" << _ << "][" << __ << "][" << ___                    \
             << "] = " << (A[_][__][___]) << '\n';                             \
    }
#define PR(A, l, r)                                                            \
    {                                                                          \
        cout << '\n';                                                          \
        for (int _ = l; _ <= r; ++_)                                           \
            DB1(A, _);                                                         \
        cout << '\n';                                                          \
    }
#define sz(x) ((int)(x).size())
#define all(v) (v).begin(), (v).end()
#define fi first
#define se second

using ll = long long;
using ld = long double;
using ii = pair<int, int>;
using vi = vector<int>;

const int NUM_TILES = 16;
int n, T;

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

struct Node {
    vector<vector<int>> adj;
    int emptyx, emptyy;
    Node() {}
    Node(vector<vector<int>> &_adj, int _ex, int _ey)
        : adj(_adj), emptyx(_ex), emptyy(_ey) {}
    void printTable() {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (adj[i][j] < 10) {
                    cout << (char)('0' + adj[i][j]);
                } else {
                    cout << (char)('a' + (adj[i][j] - 10));
                }
            }
            cout << '\n';
        }
    }
    void move(char c) {
        int t = 0;
        if (c == 'L') {
            t = 0;
        } else if (c == 'U') {
            t = 1;
        } else if (c == 'R') {
            t = 2;
        } else {
            t = 3;
        }
        int xx = emptyx + dx[t], yy = emptyy + dy[t];
        swap(adj[xx][yy], adj[emptyx][emptyy]);
        emptyx = xx;
        emptyy = yy;
    }
};

int main() {
    cin.tie(0)->sync_with_stdio(0);
    freopen("M.inp", "r", stdin);
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
    auto adj = convertStrToGraph(s);
    Node root = Node(adj, ex, ey);
    string ans;
    cin >> ans;
    int cnt = 0;
    for (char c : ans) {
        cnt++;
        root.move(c);
        if (cnt == 52) {
            root.printTable();
            cout << "left = " << ans.substr(cnt) << '\n';
        }
    }
}
