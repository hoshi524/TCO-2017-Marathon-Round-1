#include <bits/stdc++.h>
using namespace std;

const double TIME_LIMIT = 1000;
const int max_size = 701;
const int max_vertex = 1000;
int N;
int E;
int vertex[max_vertex][2];
int edges[max_vertex][max_vertex];
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
    for (int i = 0; i < E; ++i) {
      const int v1 = edges_[i * 3 + 0];
      const int v2 = edges_[i * 3 + 1];
      const int len = edges_[i * 3 + 2];
      edges[v1][++edges[v1][0]] = v2;
      edges[v2][++edges[v2][0]] = v1;
      length[v2][v1] = len;
      length[v1][v2] = len;
    }
    for (int i = 0; i < N; ++i) {
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
        auto update_vertex_value = [](int v, int pr, int pc, int nr, int nc) {
          vertex[v][0] = pr;
          vertex[v][1] = pc;
          for (int i = 1, size = edges[v][0]; i <= size; ++i) {
            const int w = edges[v][i];
            vertex_value[w] -= calc_value(v, w);
          }
          vertex[v][0] = nr;
          vertex[v][1] = nc;
          for (int i = 1, size = edges[v][0]; i <= size; ++i) {
            const int w = edges[v][i];
            vertex_value[w] += calc_value(v, w);
          }
        };
        const int v = get_random() % N;
        const int pr = vertex[v][0];
        const int pc = vertex[v][1];
        int row, col;
        {
          const int dist = 8 + (max_size / 3 - 8) * time;
          const int a = max(pr - dist, 0);
          const int b = max(pc - dist, 0);
          const int c = min(pr + dist + 1, max_size);
          const int d = min(pc + dist + 1, max_size);
          row = a + get_random() % (c - a);
          col = b + get_random() % (d - b);
        }
        vertex[v][0] = row;
        vertex[v][1] = col;
        const double ns = calc_score(v);
        const double allow =
            -log(get_random_double()) * vertex_value[v] * time * 0.5;
        if (vertex_value[v] > ns - allow) {
          vertex_value[v] = ns;
          update_vertex_value(v, pr, pc, row, col);
        } else {
          vertex[v][0] = pr;
          vertex[v][1] = pc;
        }
      }
    }
    // cerr << "iterate   = " << iterate << endl;
    {
      int count[max_size][max_size];
      memset(count, 0, sizeof(count));
      for (int i = 0; i < N; ++i) {
        ++count[vertex[i][0]][vertex[i][1]];
      }
      for (int r = 0; r < max_size; ++r) {
        for (int c = 0; c < max_size; ++c) {
          while (count[r][c] > 1) {
            struct Move {
              int id, row, col;
              double ratio;
            };
            Move move = (Move){-1, -1, -1, 1e10};
            for (int i = 0; i < N; ++i) {
              if (r == vertex[i][0] && c == vertex[i][1]) {
                auto minRatio = [](int i, int r, int c) {
                  double ratio = 1.0;
                  for (int j = 1; j <= edges[i][0]; ++j) {
                    double jr = vertex[edges[i][j]][0];
                    double jc = vertex[edges[i][j]][1];
                    double jd = calc_dist(r, c, jr, jc);
                    double jl = length[i][edges[i][j]];
                    double t = jd < jl ? jd / jl : jl / jd;
                    if (ratio > t) ratio = t;
                  }
                  return ratio;
                };
                double ratio = minRatio(i, r, c);
                for (int nr = max(r - 1, 0), nrs = min(r + 1, max_size - 1);
                     nr <= nrs; ++nr) {
                  for (int nc = max(c - 1, 0), ncs = min(c + 1, max_size - 1);
                       nc <= ncs; ++nc) {
                    if (count[nr][nc] == 0) {
                      double v = minRatio(i, nr, nc) - ratio;
                      if (move.ratio > v) {
                        move = (Move){i, nr, nc, v};
                      }
                    }
                  }
                }
              }
            }
            --count[r][c];
            vertex[move.id][0] = move.row;
            vertex[move.id][1] = move.col;
            ++count[move.row][move.col];
          }
        }
      }
    };
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