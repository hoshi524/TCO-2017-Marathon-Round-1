#include <bits/stdc++.h>
using namespace std;

constexpr double PI2 = M_PI * 2.0;
constexpr double TIME_LIMIT = 9800;
constexpr int max_edge = 64;
constexpr int max_size = 701;
constexpr int max_vertex = 1000;
int N;
int esize[max_vertex];
int edges[max_vertex][max_edge][2];
double START_TIME;
double max_dist[max_vertex];
double vertex[max_vertex][2];

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

// http://takashiijiri.com/study/miscs/fastsqrt.html
double t_sqrt(const double& x) {
  double xHalf = 0.5 * x;
  long long int tmp = 0x5FE6EB50C7B537AAl - (*(long long int*)&x >> 1);
  double xRes = *(double*)&tmp;
  xRes *= (1.5 - (xHalf * xRes * xRes));
  return xRes * x;
}

double calc_dist(double i, double j, double x, double y) {
  const double a = i - x;
  const double b = j - y;
  return t_sqrt(a * a + b * b);
}

double calc_score(int x, double r, double c, double time) {
  double sum = 0.0;
  double max = 0.0;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    const double d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
    const double r = d > l ? d / l : l / d;
    sum += (r * r) - 1.0;
    if (max < r) max = r;
  }
  return sum * time + (max - 1.0) * (1.0 - time);
}

bool apply1(int x, double r, double c, double a, double b, double time) {
  double s1 = 0, s2 = 0;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    {
      const double d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
      s1 += d > l ? d - l : l - d;
    }
    {
      const double d = calc_dist(a, b, vertex[y][0], vertex[y][1]);
      s2 += d > l ? d - l : l - d;
    }
  }
  return s1 * (1 + get_random_double() * time * 0.7) > s2;
}

bool apply2(int x, double r, double c, double a, double b, double time) {
  double m1 = 0, m2 = 0;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    {
      const double d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
      const double r = d > l ? d / l : l / d;
      if (m1 < r) m1 = r;
    }
    {
      const double d = calc_dist(a, b, vertex[y][0], vertex[y][1]);
      const double r = d > l ? d / l : l / d;
      if (m2 < r) m2 = r;
    }
  }
  m1 -= 1;
  m2 -= 1;
  return m1 * (1 + get_random_double() * time * 0.7) > m2;
}

void annealing(double end, bool (*apply)(int x, double r, double c, double a,
                                         double b, double time)) {
  constexpr int batch = (1 << 8) - 1;
  static int iterate = 0;
  while (true) {
    const double time = (START_TIME + TIME_LIMIT - get_time()) / TIME_LIMIT;
    if (time < end) break;
    while ((++iterate & batch) != batch) {
      const int v = get_random() % N;
      const double pr = vertex[v][0];
      const double pc = vertex[v][1];
      double row, col;
      const double md = min(max_dist[v] * time, (double)max_size);
      while (true) {
        const double dist = md * get_random_double();
        const double dir = PI2 * get_random_double();
        row = pr + dist * sin(dir);
        col = pc + dist * cos(dir);
        if (0 < row && row < max_size && 0 < col && col < max_size) break;
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
    memset(esize, 0, sizeof(esize));
    double min_len[N];
    for (int i = 0; i < N; ++i) {
      min_len[i] = max_size;
    }
    for (int i = 0, size = edges_.size() / 3; i < size; ++i) {
      const int v1 = edges_[i * 3 + 0];
      const int v2 = edges_[i * 3 + 1];
      const int len = edges_[i * 3 + 2];
      edges[v1][esize[v1]][0] = v2;
      edges[v1][esize[v1]][1] = len;
      edges[v2][esize[v2]][0] = v1;
      edges[v2][esize[v2]][1] = len;
      ++esize[v1];
      ++esize[v2];
      if (min_len[v1] > len) min_len[v1] = len;
      if (min_len[v2] > len) min_len[v2] = len;
    }
    for (int i = 0; i < N; ++i) {
      assert(esize[i] < max_edge);
      vertex[i][0] = get_random() % max_size;
      vertex[i][1] = get_random() % max_size;
      max_dist[i] = min(10.0 * min_len[i] + 100.0, max_size * 2.0);
    }
    annealing(0.15, apply1);
    annealing(0, apply2);
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
      struct Vertex {
        int id;
        double value;
      };
      vector<Vertex> vv;
      for (int i = 0; i < N; ++i) {
        vv.push_back(
            (Vertex){i, calc_score(i, vertex[i][0], vertex[i][1], 0.005)});
      }
      sort(vv.begin(), vv.end(),
           [](const Vertex& a, const Vertex& b) { return a.value > b.value; });
      vector<int> ret(N * 2);
      bool used[max_size][max_size];
      memset(used, 0, sizeof(used));
      for (auto v : vv) {
        const double pr = vertex[v.id][0];
        const double pc = vertex[v.id][1];
        double value = 1e10;
        int row = -1, col = -1;
        const int range = 5;
        for (int r = max((int)pr - range, 0),
                 rs = min((int)pr + range, max_size - 1);
             r <= rs; ++r) {
          for (int c = max((int)pc - range, 0),
                   cs = min((int)pc + range, max_size - 1);
               c <= cs; ++c) {
            if (used[r][c]) continue;
            const double ts = calc_score(v.id, r, c, 0);
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