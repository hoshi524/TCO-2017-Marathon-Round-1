#include <bits/stdc++.h>
using namespace std;

const double TIME_LIMIT = 1000;
const int max_size = 701;
const int max_vertex = 1000;
bool used[max_size][max_size];
int N;
int E;
int vertex[max_vertex][2];
int edges[max_vertex][max_vertex];
int length[max_vertex][max_vertex];
double sum_value;
double vertex_value[max_vertex];

double get_time() {
  unsigned long long a, d;
  __asm__ volatile("rdtsc" : "=a"(a), "=d"(d));
  return (d << 32 | a) / 2500000.0;
}

unsigned get_random() {
  static unsigned y = 2463534242;
  return y ^= (y ^= (y ^= y << 13) >> 17) << 5;
}

double get_random_double() {
  static uniform_real_distribution<double> unif(0, 1);
  static default_random_engine re;
  return unif(re);
}

double calc_dist(int i, int j, int x, int y) {
  int a = i - x;
  int b = j - y;
  return sqrt((double)(a * a + b * b));
}

double calc_center_dist(int i, int j) {
  return calc_dist(max_size / 2, max_size / 2, i, j);
}

double calc_dist(int i, int j) {
  return calc_dist(vertex[i][0], vertex[i][1], vertex[j][0], vertex[j][1]);
}

double calc_value(int i, int j) {
  const double d = calc_dist(i, j);
  const int l = length[i][j];
  double ratio;
  if (d > l) {
    ratio = d / l;
  } else {
    ratio = l / d;
  }
  return ratio * ratio * ratio;
}

double calc_score(int x) {
  double sum = 0;
  for (int i = 1, size = edges[x][0]; i <= size; ++i) {
    sum += calc_value(x, edges[x][i]);
  }
  return sum;
}

class GraphDrawing {
 public:
  vector<int> plot(int N_, vector<int> edges_) {
    const double START_TIME = get_time();
    N = N_;
    E = edges_.size() / 3;
    memset(edges, 0, sizeof(edges));
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
      vertex[i][0] = get_random() % max_size;
      vertex[i][1] = get_random() % max_size;
      used[vertex[i][0]][vertex[i][1]] = true;
    }
    sum_value = 0;
    for (int i = 0; i < N; ++i) {
      sum_value += vertex_value[i] = calc_score(i);
    }
    const int batch = (1 << 12) - 1;
    int iterate = 0;
    while (true) {
      const double time = (START_TIME + TIME_LIMIT - get_time()) / TIME_LIMIT;
      if (time < 0) break;
      while ((++iterate & batch) != batch) {
        const int v = get_random() % N;
        const int pr = vertex[v][0];
        const int pc = vertex[v][1];
        int row, col;
        {
          int dist = 10 + (max_size / 2 - 10) * time;
          int a = pr - dist;
          if (a < 0) a = 0;
          int b = pc - dist;
          if (b < 0) b = 0;
          int c = pr + dist + 1;
          if (c > max_size) c = max_size;
          int d = pc + dist + 1;
          if (d > max_size) d = max_size;
          while (true) {
            row = a + get_random() % (c - a);
            col = b + get_random() % (d - b);
            if (!used[row][col]) break;
          }
        }
        vertex[v][0] = row;
        vertex[v][1] = col;
        const double ns = calc_score(v);
        const double allow =
            -log(get_random_double()) * vertex_value[v] * time * 0.5;
        if (vertex_value[v] > ns - allow) {
          sum_value += (ns - vertex_value[v]) * 2;
          vertex_value[v] = ns;
          used[row][col] = true;
          used[pr][pc] = false;

          vertex[v][0] = pr;
          vertex[v][1] = pc;
          for (int i = 1, size = edges[v][0]; i <= size; ++i) {
            const int w = edges[v][i];
            vertex_value[w] -= calc_value(v, w);
          }

          vertex[v][0] = row;
          vertex[v][1] = col;
          for (int i = 1, size = edges[v][0]; i <= size; ++i) {
            const int w = edges[v][i];
            vertex_value[w] += calc_value(v, w);
          }
        } else {
          vertex[v][0] = pr;
          vertex[v][1] = pc;
        }
      }
    }
    // cerr << "iterate   = " << iterate << endl;
    vector<int> ret;
    for (int i = 0; i < N; ++i) {
      ret.push_back(vertex[i][0]);
      ret.push_back(vertex[i][1]);
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
