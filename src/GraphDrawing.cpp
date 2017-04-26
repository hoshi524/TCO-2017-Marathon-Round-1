#include <bits/stdc++.h>
using namespace std;

constexpr double PI2 = M_PI * 2.0;
constexpr double TIME_LIMIT = 9800;
constexpr int max_edge = 64;
constexpr int max_size = 700;
constexpr int max_vertex = 1000;
int N;
int esize[max_vertex];
int edges[max_vertex][max_edge][2];
int vertex[max_vertex][2];
int DIST[max_size + 1][max_size + 1];
double START_TIME;

double get_time() {
  unsigned long long a, d;
  __asm__ volatile("rdtsc" : "=a"(a), "=d"(d));
  return (d << 32 | a) / 2500000.0;
}

unsigned get_random() {
  static unsigned y = 2463534242;
  return y ^= (y ^= (y ^= y << 13) >> 17) << 5;
}

double get_random_double() { return (double)get_random() / UINT_MAX; }

int calc_dist(int i, int j, int x, int y) {
  return DIST[abs(i - x)][abs(j - y)];
}

double calc_score(int x, int r, int c) {
  double max = 0;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    const int d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
    const double r = d > l ? (double)d / l : (double)l / d;
    if (max < r) max = r;
  }
  return max;
}

bool apply1(int x, int r, int c, int a, int b, double time) {
  int s1 = 0, s2 = 0;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    {
      const int d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
      s1 += d > l ? d - l : l - d;
    }
    {
      const int d = calc_dist(a, b, vertex[y][0], vertex[y][1]);
      s2 += d > l ? d - l : l - d;
    }
  }
  return s1 * (1 + get_random_double() * time * 0.7) > s2;
}

bool apply2(int x, int r, int c, int a, int b, double time) {
  double m1 = 1, m2 = 1;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    {
      const int d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
      const double r = d > l ? (double)d / l : (double)l / d;
      if (m1 < r) m1 = r;
    }
    {
      const int d = calc_dist(a, b, vertex[y][0], vertex[y][1]);
      const double r = d > l ? (double)d / l : (double)l / d;
      if (m2 < r) m2 = r;
    }
  }
  m1 -= 1;
  m2 -= 1;
  return m1 * (1 + get_random_double() * time * 0.7) > m2;
}

void annealing(double end,
               bool (*apply)(int x, int r, int c, int a, int b, double time)) {
  while (true) {
    const double time = (START_TIME + TIME_LIMIT - get_time()) / TIME_LIMIT;
    if (time < end) break;
    const int md = 1 + 400 * time;
    for (int v = 0; v < N; ++v) {
      const int pr = vertex[v][0];
      const int pc = vertex[v][1];
      int row, col;
      {
        const int minr = max(pr - md, 0);
        const int maxr = min(pr + md, max_size) + 1;
        const int minc = max(pc - md, 0);
        const int maxc = min(pc + md, max_size) + 1;
        while (true) {
          row = minr + get_random() % (maxr - minr);
          col = minc + get_random() % (maxc - minc);
          if (pr != row || pc != col) break;
        }
      }
      if (apply(v, pr, pc, row, col, time)) {
        vertex[v][0] = row;
        vertex[v][1] = col;
      }
    }
  }
}

class GraphDrawing {
 public:
  vector<int> plot(int N_, vector<int> edges_) {
    START_TIME = get_time();
    N = N_;
    for (int i = 0; i <= max_size; ++i) {
      for (int j = 0; j <= max_size; ++j) {
        DIST[i][j] = sqrt((double)(i * i + j * j)) * (1 << 15);
      }
    }
    memset(esize, 0, sizeof(esize));
    for (int i = 0, size = edges_.size() / 3; i < size; ++i) {
      const int v1 = edges_[i * 3 + 0];
      const int v2 = edges_[i * 3 + 1];
      const int len = edges_[i * 3 + 2] << 15;
      edges[v1][esize[v1]][0] = v2;
      edges[v1][esize[v1]][1] = len;
      edges[v2][esize[v2]][0] = v1;
      edges[v2][esize[v2]][1] = len;
      ++esize[v1];
      ++esize[v2];
    }
    for (int i = 0; i < N; ++i) {
      assert(esize[i] < max_edge);
      vertex[i][0] = get_random() % max_size;
      vertex[i][1] = get_random() % max_size;
    }
    annealing(0.2, apply1);
    annealing(0.0, apply2);
    {
      struct Vertex {
        int id;
        double v;
      };
      vector<Vertex> vv;
      for (int i = 0; i < N; ++i) {
        vv.push_back((Vertex){i, calc_score(i, vertex[i][0], vertex[i][1])});
      }
      sort(vv.begin(), vv.end(),
           [](const Vertex& a, const Vertex& b) { return a.v > b.v; });
      vector<int> ret(N * 2);
      bool used[max_size + 1][max_size + 1];
      memset(used, 0, sizeof(used));
      for (auto v : vv) {
        const int pr = vertex[v.id][0];
        const int pc = vertex[v.id][1];
        double value = 1e10;
        int row = -1, col = -1;
        const int range = 5;
        for (int r = max(pr - range, 0), rs = min(pr + range, max_size);
             r <= rs; ++r) {
          for (int c = max(pc - range, 0), cs = min(pc + range, max_size);
               c <= cs; ++c) {
            if (used[r][c]) continue;
            const double ts = calc_score(v.id, r, c);
            if (value > ts) {
              value = ts;
              row = r;
              col = c;
            }
          }
        }
        vertex[v.id][0] = row;
        vertex[v.id][1] = col;
        ret[v.id * 2 + 0] = row;
        ret[v.id * 2 + 1] = col;
        used[row][col] = true;
      }
      return ret;
    };
  }
};

// -------8<------- end of solution submitted to the website -------8<-------
int main() {
  int N;
  cin >> N;
  int E;
  cin >> E;
  vector<int> edges(E);
  for (int i = 0; i < E; ++i) cin >> edges[i];

  GraphDrawing gd;
  vector<int> ret = gd.plot(N, edges);
  cout << ret.size() << endl;
  for (int i = 0; i < (int)ret.size(); ++i) cout << ret[i] << endl;
  cout.flush();
}