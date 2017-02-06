#include <bits/stdc++.h>

using namespace std;


int N, K;
vector<vector<int>> adj_list;
vector<int> weight;
vector<vector<int>> memo;

void solve(int parent, int u) {
    for (int v : adj_list[u]) {
        if (v == parent)
            continue;

        solve(u, v);
        for (int i = K; i >= 1; i--)
            for (int j = 1; j < i; j++)
                memo[u][i] = max(memo[u][i], memo[v][j] + memo[u][i - j]);
    }
}

int main() {
    while (scanf("%d %d", &N, &K) == 2) {
        weight.assign(N, 0);
        adj_list.assign(N, vector<int>());

        for (int i = 0; i < N; i++)
            scanf("%d", &weight[i]);

        for (int i = 0; i < N - 1; i++) {
            int u, v;
            scanf("%d %d", &u, &v);
            adj_list[u].push_back(v);
            adj_list[v].push_back(u);
        }

        memo.assign(N, vector<int>(K + 1, 0));

        for (int i = 0; i < N; i++)
            memo[i][1] = weight[i];

        solve(-1, 0);

        int answer = 0;
        for (int i = 0; i < N; i++)
            answer = max(answer, memo[i][K]);
            
        printf("%d\n", answer);
    }
    return 0;
}
