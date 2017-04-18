#include <bits/stdc++.h>
using namespace std;

const double TIME_LIMIT = 970;
const int max_size = 701;
const int max_vertex = 1000;
int N;
int E;
int vertex[max_vertex][2];
int edges[max_vertex][max_vertex];
double length[max_vertex][max_vertex];

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

double calc_dist(double i, double j, double x, double y) {
  double a = i - x;
  double b = j - y;
  return sqrt(a * a + b * b);
}

double calc_score(int x, double r, double c, double time) {
  double sum = 0.0;
  double max = 1.0;
  for (int i = 1; i <= edges[x][0]; ++i) {
    const int y = edges[x][i];
    const double d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
    const double l = length[x][y];
    const double r = d > l ? d / l : l / d;
    sum += (r * r * r) - 1.0;
    if (max < r) max = r;
  }
  return sum * time + max * (1.0 - time);
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
          const int dist = 10 + (max_size / 2 - 10) * time;
          const int a = max(pr - dist, 0);
          const int b = max(pc - dist, 0);
          const int c = min(pr + dist + 1, max_size);
          const int d = min(pc + dist + 1, max_size);
          row = a + get_random() % (c - a);
          col = b + get_random() % (d - b);
        }
        const double ps = calc_score(v, pr, pc, time);
        const double ns = calc_score(v, row, col, time);
        const double allow = max(-log(get_random_double()) * ps * time, 0.0);
        if (ps > ns - allow) {
          vertex[v][0] = row;
          vertex[v][1] = col;
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
                const int range = 10;
                double ratio = calc_score(i, r, c, 0);
                for (int nr = max(r - range, 0),
                         nrs = min(r + range, max_size - 1);
                     nr <= nrs; ++nr) {
                  for (int nc = max(c - range, 0),
                           ncs = min(c + range, max_size - 1);
                       nc <= ncs; ++nc) {
                    if (count[nr][nc] == 0) {
                      const double v = calc_score(i, nr, nc, 0) - ratio;
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