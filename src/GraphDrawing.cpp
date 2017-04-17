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

tuple<double, double, double, double> calc_intersection(double x1, double y1,
                                                        double r1, double x2,
                                                        double y2, double r2) {
  assert(r1 > 0.1);
  assert(r2 > 0.1);
  double mx = x2 - x1;
  double my = y2 - y1;
  double d = sqrt(mx * mx + my * my);
  double a = atan2(my, mx);
  double t = (d * d + r1 * r1 - r2 * r2) / (2 * d);
  double c = acos(t / r1);
  // assert(r1 + r2 >= d && max(r1, r2) <= min(r1, r2) + d);
  return forward_as_tuple(x1 + r1 * cos(a + c), y1 + r1 * sin(a + c),
                          x1 + r1 * cos(a - c), y1 + r1 * sin(a - c));
}

tuple<double, double, double> calc_position(double* circle, int size) {
  int vs = 0;
  double vertex[size * size][2];
  for (int i = 0; i < size; ++i) {
    for (int j = i + 1; j < size; ++j) {
      double x1, y1, x2, y2;
      tie(x1, y1, x2, y2) = calc_intersection(
          circle[i * 3 + 0], circle[i * 3 + 1], circle[i * 3 + 2],
          circle[j * 3 + 0], circle[j * 3 + 1], circle[j * 3 + 2]);
      vertex[vs][0] = x1;
      vertex[vs][1] = y1;
      ++vs;
      vertex[vs][0] = x2;
      vertex[vs][1] = y2;
      ++vs;
    }
  }
  double ratio[size * size];
  for (int i = 0; i < vs; ++i) {
    ratio[i] = 1.0;
  }
  for (int j = 0; j < size; ++j) {
    const double cx = circle[j * 3 + 0];
    const double cy = circle[j * 3 + 1];
    const double cd = circle[j * 3 + 2];
    for (int i = 0; i < vs; ++i) {
      double d = calc_dist(vertex[i][0], vertex[i][1], cx, cy);
      double r = d < cd ? d / cd : cd / d;
      if (ratio[i] > r)
        ratio[i] = r;  //   if (0 < row && row < max_size && 0 < col && col <
                       //   max_size) break;
                       // }
    }
  }
  double max = 0.0;
  int vi = -1;
  for (int i = 0; i < vs; ++i) {
    if (max < ratio[i]) {
      max = ratio[i];
      vi = i;
    }
  }
  return forward_as_tuple(max, vertex[vi][0], vertex[vi][1]);
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
    if (false) {
      int cv, v1, v2, c;

      tie(v1, c) = select_vertex(selected);
      selected.insert(v1);
      vertex[v1][0] = 0;
      vertex[v1][1] = 0;

      tie(v2, c) = select_vertex(selected);
      assert(c == 1);
      selected.insert(v2);
      vertex[v2][0] = length[v1][v2];
      vertex[v2][1] = 0;

      double circle[max_vertex * 3];
      while (true) {
        tie(cv, c) = select_vertex(selected);
        const int size = 3 < selected.size() ? 3 : selected.size();
        if (c < size) break;
        int vi = 0;
        for (int v : selected) {
          if (length[v][cv] < 0) continue;
          circle[vi * 3 + 0] = vertex[v][0];
          circle[vi * 3 + 1] = vertex[v][1];
          circle[vi * 3 + 2] = length[v][cv];
          ++vi;
        }
        // cerr << vi << " " << c << endl;
        // assert(vi == c);
        selected.insert(cv);
        double ratio, row, col;
        tie(ratio, row, col) = calc_position(circle, vi);
        vertex[cv][0] = row;
        vertex[cv][1] = col;
      }
      cerr << "selected  = " << selected.size() << endl;
    }
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
    if (false) {
      struct V {
        int i;
        double len;
      };
      vector<V> vertex_list;
      for (int i = 0; i < N; ++i) {
        V v = (V){i, 1e10};
        for (int j = 1; j <= edges[i][0]; ++j) {
          double len = length[i][edges[i][j]];
          if (v.len > len) v.len = len;
        }
        vertex_list.push_back(v);
      }
      sort(vertex_list.begin(), vertex_list.end(),
           [](const V& a, const V& b) { return a.len < b.len; });

      vector<int> ret(N * 2);
      bool used[max_size][max_size];
      memset(used, 0, sizeof(used));
      for (int i = 0; i < N; ++i) {
        const int vi = vertex_list[i].i;
        const double r = vertex[vi][0];
        const double c = vertex[vi][1];
        struct P {
          int r, c;
          double ratio;
        };
        vector<P> point_list;
        for (int row = max((int)r - 1, 0), rows = min((int)r + 1, max_size - 1);
             row <= rows; ++row) {
          for (int col = max((int)c - 1, 0),
                   cols = min((int)c + 1, max_size - 1);
               col <= cols; ++col) {
            if (used[row][col]) continue;
            point_list.push_back((P){row, col, 1.0});
          }
        }
        assert(point_list.size() > 0);
        P best = (P){-1, -1, 0};
        for (auto p : point_list) {
          for (int j = 0; j < i; ++j) {
            const double ln = length[vi][vertex_list[j].i];
            if (ln < 0) continue;
            const double jr = vertex[vertex_list[j].i][0];
            const double jc = vertex[vertex_list[j].i][1];
            const double jd = calc_dist(p.r, p.c, jr, jc);
            const double ratio = jd < ln ? jd / ln : ln / jd;
            if (p.ratio > ratio) p.ratio = ratio;
          }
          if (best.ratio < p.ratio) best = p;
        }
        used[best.r][best.c] = true;
        ret[vi * 2 + 0] = best.r;
        ret[vi * 2 + 1] = best.c;
        vertex[vi][0] = best.r;
        vertex[vi][1] = best.c;
      }
      return ret;
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
