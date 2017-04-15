#include <bits/stdc++.h>
using namespace std;

const double TIME_LIMIT = 1000;
const int max_size = 701;
const int max_vertex = 1000;
bool used[max_size][max_size];
int N;
int E;
int sum_value;
int vertices[max_vertex][2];
int edges[max_vertex][max_vertex];
int length[max_vertex][max_vertex];
int vertex_value[max_vertex];
int center_dist[max_vertex];

double get_time() {
  unsigned long long a, d;
  __asm__ volatile("rdtsc" : "=a"(a), "=d"(d));
  return (d << 32 | a) / 2500000.0;
}

unsigned get_random() {
  static unsigned y = 2463534242;
  return y ^= (y ^= (y ^= y << 13) >> 17) << 5;
}

int calc_dist(int i, int j, int x, int y) {
  int a = i - x;
  int b = j - y;
  return sqrt((double)(a * a + b * b));
}

int calc_center_dist(int i, int j) {
  return calc_dist(max_size / 2, max_size / 2, i, j);
}

int calc_dist(int i, int j) {
  return calc_dist(vertices[i][0], vertices[i][1], vertices[j][0],
                   vertices[j][1]);
}

int calc_value(int i, int j) {
  const int d = calc_dist(i, j);
  const int len = length[i][j];
  assert(d > 0);
  assert(len > 0);
  int ratio;
  if (d > len) {
    ratio = (d << 4) / len;
  } else {
    ratio = (len << 4) / d;
  }
  return ratio * ratio;
}

int calc_score(int x) {
  assert(x < N);
  assert(edges[x][0] < N);
  int sum = 0;
  for (int i = 1, size = edges[x][0]; i <= size; ++i) {
    sum += calc_value(x, edges[x][i]);
  }
  assert(sum >= 0);
  return sum;
}

int select_vertex() {
  assert(sum_value >= 0);
  int v = get_random() % sum_value;
  for (int i = 0; i < N; ++i) {
    if (v < vertex_value[i]) return i;
    v -= vertex_value[i];
  }
  return -1;
}

class GraphDrawing {
 public:
  vector<int> plot(int N_, vector<int> edges_) {
    const double START_TIME = get_time();
    N = N_;
    E = edges_.size() / 3;
    memset(edges, 0, sizeof(edges));
    memset(center_dist, 0, sizeof(center_dist));
    for (int i = 0; i < E; ++i) {
      const int v1 = edges_[i * 3 + 0];
      const int v2 = edges_[i * 3 + 1];
      const int len = edges_[i * 3 + 2];
      edges[v1][++edges[v1][0]] = v2;
      edges[v2][++edges[v2][0]] = v1;
      length[v2][v1] = len;
      length[v1][v2] = len;
    }
    memset(used, 0, sizeof(used));
    for (int i = 0; i < N; ++i) {
      vertices[i][0] = get_random() % max_size;
      vertices[i][1] = get_random() % max_size;
      used[vertices[i][0]][vertices[i][1]] = true;
    }
    sum_value = 0;
    for (int i = 0; i < N; ++i) {
      vertex_value[i] = calc_score(i);
      sum_value += vertex_value[i];
    }
    const int batch = (1 << 12) - 1;
    int iterate = 0;
    while (true) {
      const double time = (get_time() - START_TIME);
      if (time > TIME_LIMIT) break;
      ++iterate;
      for (; (iterate & batch) != batch; ++iterate) {
        const int v = get_random() % N;
        // const int v = select_vertex();
        assert(v != -1);
        const int pr = vertices[v][0];
        const int pc = vertices[v][1];
        int row, col;
        while (true) {
          row = get_random() % max_size;
          col = get_random() % max_size;
          if (!used[row][col]) break;
        }
        vertices[v][0] = row;
        vertices[v][1] = col;
        int ns = calc_score(v);
        int allow = vertex_value[v] * (TIME_LIMIT - time) / TIME_LIMIT / 2;
        if (vertex_value[v] > ns - allow) {
          sum_value += (ns - vertex_value[v]) * 2;
          vertex_value[v] = ns;
          used[row][col] = true;
          used[pr][pc] = false;

          vertices[v][0] = pr;
          vertices[v][1] = pc;
          for (int i = 1, size = edges[v][0]; i <= size; ++i) {
            const int w = edges[v][i];
            vertex_value[w] -= calc_value(v, w);
          }

          vertices[v][0] = row;
          vertices[v][1] = col;
          for (int i = 1, size = edges[v][0]; i <= size; ++i) {
            const int w = edges[v][i];
            vertex_value[w] += calc_value(v, w);
          }
        } else {
          vertices[v][0] = pr;
          vertices[v][1] = pc;
        }
      }
    }
    cerr << "iterate   = " << iterate << endl;
    vector<int> ret;
    for (int i = 0; i < N; ++i) {
      ret.push_back(vertices[i][0]);
      ret.push_back(vertices[i][1]);
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
