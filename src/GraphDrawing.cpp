#include <bits/stdc++.h>
using namespace std;

constexpr double PI2 = M_PI * 2.0;
constexpr double TIME_LIMIT = 9800;
constexpr int max_edge = 64;
constexpr int max_size = 700;
constexpr int max_vertex = 1000;

struct Vertex {
  int id, v;
};

int N;
Vertex sort_vertex[max_vertex];
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

int calc_score(int x, int r, int c) {
  int max = 0;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    const int d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
    const int r = d > l ? (d << 10) / l : (l << 10) / d;
    if (max < r) max = r;
  }
  return max;
}

tuple<int, int> apply1(int x, int r, int c, int a, int b) {
  int v1 = 0, v2 = 0;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    v1 += abs(calc_dist(r, c, vertex[y][0], vertex[y][1]) - l);
    v2 += abs(calc_dist(a, b, vertex[y][0], vertex[y][1]) - l);
  }
  return make_tuple(v1, v2);
}

tuple<int, int> apply2(int x, int r, int c, int a, int b) {
  int v1 = 0, v2 = 0;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    {
      const int d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
      const int r = d > l ? (d << 10) / l : (l << 10) / d;
      if (v1 < r) v1 = r;
    }
    {
      const int d = calc_dist(a, b, vertex[y][0], vertex[y][1]);
      const int r = d > l ? (d << 10) / l : (l << 10) / d;
      if (v2 < r) v2 = r;
    }
  }
  v1 -= 1 << 10;
  v2 -= 1 << 10;
  return make_tuple(v1, v2);
}

void annealing(double end,
               tuple<int, int> (*apply)(int x, int r, int c, int a, int b)) {
  while (true) {
    const double time = (START_TIME + TIME_LIMIT - get_time()) / TIME_LIMIT;
    if (time < end) break;
    const int md = 1 + 300 * time;
    for (int i = 0; i < (1 << 10); ++i) {
      const int x = (pow(get_random_double(), 2) - 0.001) * N;
      const int v = sort_vertex[x].id;
      const int pr = vertex[v][0];
      const int pc = vertex[v][1];
      int row, col;
      {
        const int minr = max(pr - md, 0);
        const int maxr = min(pr + md, max_size) + 1;
        const int minc = max(pc - md, 0);
        const int maxc = min(pc + md, max_size) + 1;
        do {
          row = minr + get_random() % (maxr - minr);
          col = minc + get_random() % (maxc - minc);
        } while (pr == row && pc == col);
      }
      int v1, v2;
      tie(v1, v2) = apply(v, pr, pc, row, col);
      if (v1 * (1 + get_random_double() * time * 0.7) > v2) {
        sort_vertex[x].v = v2;
        vertex[v][0] = row;
        vertex[v][1] = col;
      } else {
        sort_vertex[x].v = v1;
      }
    }
    sort(sort_vertex, sort_vertex + N,
         [](const Vertex& a, const Vertex& b) { return a.v > b.v; });
  }
}

class GraphDrawing {
 public:
  vector<int> plot(int N_, vector<int> edges_) {
    START_TIME = get_time();
    N = N_;
    for (int i = 0; i <= max_size; ++i) {
      for (int j = 0; j <= max_size; ++j) {
        DIST[i][j] = sqrt((double)(i * i + j * j)) * (1 << 10);
      }
    }
    DIST[0][0] = 1;
    memset(esize, 0, sizeof(esize));
    for (int i = 0, size = edges_.size() / 3; i < size; ++i) {
      const int v1 = edges_[i * 3 + 0];
      const int v2 = edges_[i * 3 + 1];
      const int len = edges_[i * 3 + 2] << 10;
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
      sort_vertex[i] = (Vertex){i, 0};
    }
    annealing(0.2, apply1);
    annealing(0.0, apply2);
    {
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
        int value = 0xffffff, row = -1, col = -1;
        const int range = 5;
        for (int r = max(pr - range, 0), rs = min(pr + range, max_size);
             r <= rs; ++r) {
          for (int c = max(pc - range, 0), cs = min(pc + range, max_size);
               c <= cs; ++c) {
            if (used[r][c]) continue;
            const int ts = calc_score(v.id, r, c);
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