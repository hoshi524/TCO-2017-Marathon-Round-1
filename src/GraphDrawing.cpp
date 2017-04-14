#include <bits/stdc++.h>
using namespace std;

class GraphDrawing {
 public:
  vector<int> plot(int N, vector<int> edges) {
    vector<int> ret;
    srand(123);
    for (int i = 0; i < 2 * N; ++i) {
      ret.push_back(rand() % 701);
    }
    return ret;
  }
};

// -------8<------- end of solution submitted to the website -------8<-------
int main() {
  GraphDrawing gd;
  int N;
  cin >> N;
  int E;
  cin >> E;
  vector<int> edges(E);
  for (int i = 0; i < E; ++i) cin >> edges[i];

  vector<int> ret = gd.plot(N, edges);
  cout << ret.size() << endl;
  for (int i = 0; i < (int)ret.size(); ++i) cout << ret[i] << endl;
  cout.flush();
}
