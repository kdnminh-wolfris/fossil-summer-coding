#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <queue>

using namespace std;

class Khau {
	int size_m, size_n;
	int max_gas_unit;
	int init_x, init_y;
	vector <vector <int>> data;

	ifstream fi;
	ofstream fo;

	const int debug = 1;

	public:
        void resize(int _m, int _n) {
            size_m = _m; size_n = _n;
            data.clear();
            data.resize(_m, vector <int>(_n));
        }

        void load() {
            fi >> init_x >> init_y;
            fi >> max_gas_unit;
            fi >> size_m >> size_n;
            resize(size_m, size_n);
            for (int i = 0; i < size_m; ++i)
                for (int j = 0; j < size_n; ++j)
                    fi >> data[i][j];
            --init_x; --init_y;
        }

        void display() {
            for (int i = 0; i < size_m; ++i)
                for (int j = 0; j < size_n; ++j)
                    cout << data[i][j] << " \n"[j == size_n - 1];
        }

        int star_count() {
            int ret = 0;
            for (int i = 0; i < size_m; ++i)
                for (int j = 0; j < size_n; ++j)
                    if (data[i][j] == 3) ++ret;
            return ret;
        }

        vector <pair <int, int>> star_list() {
            if (debug) cout << "Getting star list..." << endl;

            vector <pair <int, int>> ret;
            for (int i = 0; i < size_m; ++i)
                for (int j = 0; j < size_n; ++j)
                    if (data[i][j] == 3)
                        ret.push_back({i, j});

            if (debug) cout << "Star list installed." << endl;

            return ret;
        }

        vector <pair <int, int>> dijkstra() {
            const int inf = 1e9;
            const int dirX[4] = {-1, 0, 1, 0};
            const int dirY[4] = {0, 1, 0, -1};
            const int radius = 30;

            vector <pair <int, int>> star = star_list();
            vector <vector <int>> star_id;
            star_id.resize(size_m, vector <int> (size_n, 0));
            int nStar = 0;
            for (auto s: star)
                if (s.first - init_x + s.second - init_y <= radius)
                    star_id[s.first][s.second] = nStar++;

            vector <vector <vector <vector <int>>>> dp;
            /// dp[x][y][g][m]
            /// the shortest path ends at (x, y) with g gas unit left and stars collected masked as m

            if (debug) {
                cout << "Map size: " << size_n << ' ' << size_m << '\n';
                cout << "Max gas unit: " << max_gas_unit << '\n';
                cout << "Number of stars: " << nStar << endl;
                system("pause");
            }

            dp.resize(size_m, vector <vector <vector <int>>> (size_n, vector <vector <int>> (max_gas_unit + 1, vector <int> (1 << nStar, inf))));

            priority_queue <pair <int, pair <pair <int, int>, pair <int, int>>>> heap;
            heap.push({0, {{init_x, init_y}, {max_gas_unit, 0}}});

            int ansX = init_x, ansY = init_y, ansG = max_gas_unit, ansM = 0;

            if (debug) cout << "Dijkstra..." << endl;

            for (int x, y, g, m, d; !heap.empty(); heap.pop()) {
                d = -heap.top().first;
                x = heap.top().second.first.first;
                y = heap.top().second.first.second;
                g = heap.top().second.second.first;
                m = heap.top().second.second.second;

                if (dp[x][y][g][m] <= d) continue;
                dp[x][y][g][m] = d;

                //cout << '[' << x << ',' << y << ',' << g << ',' << m << ']' << '=' << d << endl;

                if (g == 0) continue;
                if (__builtin_popcount(m) > __builtin_popcount(ansM))
                    ansX = x, ansY = y, ansG = g, ansM = m;
                if (m == (1 << nStar) - 1) break;

                for (int xx, yy, gg, mm, i = 0; i < 4; ++i) {
                    xx = x + dirX[i];
                    yy = y + dirY[i];
                    if (0 > xx || xx >= size_m || 0 > yy || yy >= size_n)
                        continue;

                    if (data[xx][yy] == 2)
                        gg = max_gas_unit;
                    else gg = g - 1;
                    if (data[xx][yy] == 3)
                        mm = m | (1 << star_id[xx][yy]);
                    else mm = m;

                    if (dp[xx][yy][gg][mm] == inf)
                        heap.push({-d - 1, {{xx, yy}, {gg, mm}}});
                }
            }

            if (debug) cout << "Done Dijkstra." << endl;

            if (debug) cout << "Tracing... " << dp[ansX][ansY][ansG][ansM] << " steps" << endl;

            vector <pair <int, int>> ret;
            while (true) {
                cout << ansX << ' ' << ansY << ' ' << ansG << ' ' << ansM << ' ' << data[ansX][ansY] << endl;

                ret.push_back({ansX, ansY});
                if (ansX == init_x && ansY == init_y && ansG == max_gas_unit && ansM == 0)
                    break;

                for (int x, y, g, m, i = 0; i < 4; ++i) {
                    x = ansX + dirX[i];
                    y = ansY + dirY[i];
                    if (0 > x || x >= size_m || 0 > y || y >= size_n)
                        continue;

                    if (data[ansX][ansY] == 2) {
                        for (g = 1; g <= max_gas_unit; ++g)
                            if (dp[x][y][g][ansM] + 1 == dp[ansX][ansY][ansG][ansM])
                                break;
                    }
                    else if (dp[x][y][ansG + 1][ansM] + 1 == dp[ansX][ansY][ansG][ansM])
                        g = ansG + 1;
                    else g = max_gas_unit + 1;

                    if (g > max_gas_unit) continue;

                    if (data[ansX][ansY] == 3) {
                        m = -1;
                        if (dp[x][y][g][ansM] + 1 == dp[ansX][ansY][ansG][ansM])
                            m = ansM;
                        else {
                            m = ansM ^ (1 << star_id[ansX][ansY]);
                            if (dp[x][y][g][m] + 1 != dp[ansX][ansY][ansG][ansM])
                                m = -1;
                        }
                    }
                    else m = ansM;

                    if (m == -1) continue;

                    ansX = x; ansY = y; ansG = g; ansM = m;
                    break;
                }
            }

            if (debug) cout << "Done tracing." << endl;

            reverse(ret.begin(), ret.end());
            return ret;
        }

        void readInput(string inPath) {
            fi.open(inPath);
            load();
            fi.close();
        }

        void writeOutput(string outPath) {
            fo.open(outPath);
            vector <pair <int, int>> res = dijkstra();
            for (auto p: res) fo << p.first << ' ' << p.second << '\n';
            fo.close();
        }
};

int main() {
    Khau sol;
    sol.readInput("map.txt");
    sol.writeOutput("path.txt");
}
