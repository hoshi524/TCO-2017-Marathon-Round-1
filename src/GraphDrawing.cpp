#include <bits/stdc++.h>
using namespace std;

const double PI2 = M_PI * 2.0;
const double TIME_LIMIT = 1000;
const int max_size = 701;
const int max_vertex = 1000;
int N;
int E;
int edges[max_vertex][max_vertex];
double vertex[max_vertex][2];
double length[max_vertex][max_vertex];
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

tuple<int, int> select_vertex(set<int>& selected) {
  int vertex = -1, contain = 0, edge = 0;
  for (int v = 0; v < N; ++v) {
    if (selected.count(v)) continue;
    int c = 0;
    for (int i = 1; i <= edges[v][0]; ++i) {
      if (selected.count(edges[v][i])) ++c;
    }
    if (contain < c || (contain == c && edge < edges[v][0])) {
      vertex = v;
      contain = c;
      edge = edges[v][0];
    }
  }
  return forward_as_tuple(vertex, contain);
}

double calc_value(int i, int j) {
  const double d = calc_dist(i, j);
  double ratio;
  if (d > length[i][j]) {
    ratio = d / length[i][j];
  } else {
    ratio = length[i][j] / d;
  }
  return (ratio * ratio * ratio) - 1.0;
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
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        length[i][j] = -1.0;
      }
    }
    for (int i = 0; i < E; ++i) {
      const int v1 = edges_[i * 3 + 0];
      const int v2 = edges_[i * 3 + 1];
      edges[v1][++edges[v1][0]] = v2;
      edges[v2][++edges[v2][0]] = v1;
      const double len = edges_[i * 3 + 2];
      length[v2][v1] = len;
      length[v1][v2] = len;
    }
    set<int> selected;
    for (int i = 0; i < N; ++i) {
      if (selected.count(i)) continue;
      vertex[i][0] = get_random() % max_size;
      vertex[i][1] = get_random() % max_size;
    }
    for (int i = 0; i < N; ++i) {
      vertex_value[i] = calc_score(i);
    }
    const int batch = (1 << 12) - 1;
    int iterate = 0;
    while (true) {
      const double time = (START_TIME + TIME_LIMIT - get_time()) / TIME_LIMIT;
      if (time < 0) break;
      while ((++iterate & batch) != batch) {
        const int v = get_random() % N;
        const double pr = vertex[v][0];
        const double pc = vertex[v][1];
        const double dist =
            1 + (9 + (max_size / 2 - 9) * time) * get_random_double();
        const double dir = PI2 * get_random_double();
        const double row = pr + dist * sin(dir);
        const double col = pc + dist * cos(dir);
        vertex[v][0] = row;
        vertex[v][1] = col;
        const double ns = calc_score(v);
        const double allow =
            -log(get_random_double()) * vertex_value[v] * time * 0.5;
        if (vertex_value[v] > ns - allow) {
          vertex_value[v] = ns;
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
    {
      double min_row = 1e10, max_row = -1e10;
      double min_col = 1e10, max_col = -1e10;
      for (int i = 0; i < N; ++i) {
        const double r = vertex[i][0];
        const double c = vertex[i][1];
        if (min_row > r) min_row = r;
        if (max_row < r) max_row = r;
        if (min_col > c) min_col = c;
        if (max_col < c) max_col = c;
      }
      double ratio = min((max_size - 1) / (max_row - min_row),
                         (max_size - 1) / (max_col - min_col));
      for (int i = 0; i < N; ++i) {
        vertex[i][0] = (vertex[i][0] - min_row) * ratio;
        vertex[i][1] = (vertex[i][1] - min_col) * ratio;
      }
    };
    {
      vector<int> ret;
      bool used[max_size][max_size];
      memset(used, 0, sizeof(used));
      struct V {
        double dist;
        int row, col;
      };
      for (int i = 0; i < N; ++i) {
        const double r = vertex[i][0];
        const double c = vertex[i][1];
        vector<V> tmp;
        for (int row = max((int)r - 3, 0), rows = min((int)r + 3, max_size - 1);
             row <= rows; ++row) {
          for (int col = max((int)c - 3, 0),
                   cols = min((int)c + 3, max_size - 1);
               col <= cols; ++col) {
            if (used[row][col]) continue;
            tmp.push_back((V){calc_dist(r, c, row, col), row, col});
          }
        }
        assert(tmp.size() > 0);
        sort(tmp.begin(), tmp.end(),
             [](const V& a, const V& b) { return a.dist < b.dist; });
        used[tmp[0].row][tmp[0].col] = true;
        ret.push_back(tmp[0].row);
        ret.push_back(tmp[0].col);
      }
      return ret;
    };
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
