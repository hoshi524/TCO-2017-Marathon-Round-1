#include <bits/stdc++.h>
using namespace std;

constexpr float PI2 = M_PI * 2.0;
constexpr float TIME_LIMIT = 9800;
constexpr int max_edge = 64;
constexpr int max_size = 700;
constexpr int max_vertex = 1000;
int N;
int esize[max_vertex];
int edges[max_vertex][max_edge][2];
float START_TIME;
float vertex[max_vertex][2];

float get_time() {
  unsigned long long a, d;
  __asm__ volatile("rdtsc" : "=a"(a), "=d"(d));
  return (d << 32 | a) / 2500000.0;
}

unsigned get_random() {
  static unsigned y = 2463534242;
  return y ^= (y ^= (y ^= y << 13) >> 17) << 5;
}

float get_random_float() { return (float)get_random() / UINT_MAX; }

// http://takashiijiri.com/study/miscs/fastsqrt.html
float t_sqrt(const float& x) {
  float xHalf = 0.5f * x;
  int tmp = 0x5F3759DF - (*(int*)&x >> 1);
  float xRes = *(float*)&tmp;
  xRes *= (1.5f - (xHalf * xRes * xRes));
  return xRes * x;
}

float calc_dist(float i, float j, float x, float y) {
  const float a = i - x;
  const float b = j - y;
  return t_sqrt(a * a + b * b);
}

float calc_score(int x, float r, float c, float time) {
  float sum = 0.0;
  float max = 0.0;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    const float d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
    const float r = d > l ? d / l : l / d;
    sum += r - 1.0;
    if (max < r) max = r;
  }
  return sum * time + (max - 1.0) * (1.0 - time);
}

bool apply1(int x, float r, float c, float a, float b, float time) {
  float s1 = 0, s2 = 0;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    {
      const float d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
      s1 += d > l ? d - l : l - d;
    }
    {
      const float d = calc_dist(a, b, vertex[y][0], vertex[y][1]);
      s2 += d > l ? d - l : l - d;
    }
  }
  return s1 * (1 + get_random_float() * time * 0.7) > s2;
}

bool apply2(int x, float r, float c, float a, float b, float time) {
  float m1 = 0, m2 = 0;
  for (int i = 0; i < esize[x]; ++i) {
    const int y = edges[x][i][0];
    const int l = edges[x][i][1];
    {
      const float d = calc_dist(r, c, vertex[y][0], vertex[y][1]);
      const float r = d > l ? d / l : l / d;
      if (m1 < r) m1 = r;
    }
    {
      const float d = calc_dist(a, b, vertex[y][0], vertex[y][1]);
      const float r = d > l ? d / l : l / d;
      if (m2 < r) m2 = r;
    }
  }
  m1 -= 1;
  m2 -= 1;
  return m1 * (1 + get_random_float() * time * 0.7) > m2;
}

void annealing(float end, bool (*apply)(int x, float r, float c, float a,
                                        float b, float time)) {
  while (true) {
    const float time = (START_TIME + TIME_LIMIT - get_time()) / TIME_LIMIT;
    if (time < end) break;
    const float md = max_size * time;
    for (int v = 0; v < N; ++v) {
      const float pr = vertex[v][0];
      const float pc = vertex[v][1];
      float row, col;
      while (true) {
        const float dist = md * get_random_float();
        const float dir = PI2 * get_random_float();
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
        float value;
      };
      vector<Vertex> vv;
      for (int i = 0; i < N; ++i) {
        vv.push_back(
            (Vertex){i, calc_score(i, vertex[i][0], vertex[i][1], 0.005)});
      }
      sort(vv.begin(), vv.end(),
           [](const Vertex& a, const Vertex& b) { return a.value > b.value; });
      vector<int> ret(N * 2);
      bool used[max_size + 1][max_size + 1];
      memset(used, 0, sizeof(used));
      for (auto v : vv) {
        const float pr = vertex[v.id][0];
        const float pc = vertex[v.id][1];
        float value = 1e10;
        int row = -1, col = -1;
        const int range = 5;
        for (int r = max((int)pr - range, 0),
                 rs = min((int)pr + range, max_size);
             r <= rs; ++r) {
          for (int c = max((int)pc - range, 0),
                   cs = min((int)pc + range, max_size);
               c <= cs; ++c) {
            if (used[r][c]) continue;
            const float ts = calc_score(v.id, r, c, 0);
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