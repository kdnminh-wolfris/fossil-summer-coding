#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <queue>

#include <stdarg.h>  // For va_start, etc.
#include <string.h>
#include <utility>
#include <memory>  // For std::unique_ptr

using namespace std;

#define WALL 0
#define STREET 1
#define GAS 2
#define STAR 3
#define EARNED_STAR 4

class Khau {
	int size_m, size_n;
	int max_gas_unit;
	int init_x, init_y;
	vector <vector <int>> data;

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
        const int radius = 30; /// only stars within this radius (Mahattan) from starting point will be collected

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

        priority_queue <pair <int, pair <pair <pair <int, int>, pair <int, int>>, pair <pair <int, int>, pair <int, int>>>>> heap;
        heap.push({0, {{{init_x, init_y}, {max_gas_unit, 0}}, {{-1, -1}, {-1, -1}}}});

        vector <vector <vector <vector <int>>>> traceX;
        traceX.resize(size_m, vector <vector <vector <int>>> (size_n, vector <vector <int>> (max_gas_unit + 1, vector <int> (1 << nStar))));
        vector <vector <vector <vector <int>>>> traceY;
        traceY.resize(size_m, vector <vector <vector <int>>> (size_n, vector <vector <int>> (max_gas_unit + 1, vector <int> (1 << nStar))));
        vector <vector <vector <vector <int>>>> traceG;
        traceG.resize(size_m, vector <vector <vector <int>>> (size_n, vector <vector <int>> (max_gas_unit + 1, vector <int> (1 << nStar))));
        vector <vector <vector <vector <int>>>> traceM;
        traceM.resize(size_m, vector <vector <vector <int>>> (size_n, vector <vector <int>> (max_gas_unit + 1, vector <int> (1 << nStar))));

        int ansX = init_x, ansY = init_y, ansG = max_gas_unit, ansM = 0;

        if (debug) cout << "Dijkstra..." << endl;

        for (int d, x, y, g, m; !heap.empty(); heap.pop()) {
            d = -heap.top().first;
            x = heap.top().second.first.first.first;
            y = heap.top().second.first.first.second;
            g = heap.top().second.first.second.first;
            m = heap.top().second.first.second.second;

            if (dp[x][y][g][m] <= d) continue;
            dp[x][y][g][m] = d;

            traceX[x][y][g][m] = heap.top().second.second.first.first;
            traceY[x][y][g][m] = heap.top().second.second.first.second;
            traceG[x][y][g][m] = heap.top().second.second.second.first;
            traceM[x][y][g][m] = heap.top().second.second.second.second;

            //cout << '[' << x << ',' << y << ',' << g << ',' << m << ']' << '=' << d << endl;

            if (g == 0) continue;
            if (__builtin_popcount(m) > __builtin_popcount(ansM))
                ansX = x, ansY = y, ansG = g, ansM = m;
            if (m == (1 << nStar) - 1) break;

            for (int xx, yy, gg, mm, i = 0; i < 4; ++i) {
                xx = x + dirX[i];
                yy = y + dirY[i];
                if (0 > xx || xx >= size_m || 0 > yy || yy >= size_n || data[xx][yy] == 0)
                    continue;

                if (data[xx][yy] == 2)
                    gg = max_gas_unit;
                else gg = g - 1;
                if (data[xx][yy] == 3)
                    mm = m | (1 << star_id[xx][yy]);
                else mm = m;

                if (dp[xx][yy][gg][mm] == inf)
                    heap.push({-d - 1, {{{xx, yy}, {gg, mm}}, heap.top().second.first}});
            }
        }

        if (debug) cout << "Done Dijkstra." << endl;

        if (debug) cout << "Tracing... " << dp[ansX][ansY][ansG][ansM] << " steps" << endl;

        vector <pair <int, int>> ret;
        while (ansX != -1) {
            //cout << ansX << ' ' << ansY << ' ' << ansG << ' ' << ansM << ' ' << data[ansX][ansY] << endl;
            ret.push_back({ansX, ansY});
            int tmpX, tmpY, tmpG, tmpM;
            tmpX = traceX[ansX][ansY][ansG][ansM];
            tmpY = traceY[ansX][ansY][ansG][ansM];
            tmpG = traceG[ansX][ansY][ansG][ansM];
            tmpM = traceM[ansX][ansY][ansG][ansM];
            ansX = tmpX; ansY = tmpY; ansG = tmpG; ansM = tmpM;
        }

        if (debug) cout << "Done tracing." << endl;

        reverse(ret.begin(), ret.end());
        return ret;
    }

	public:
        ifstream fi;
        ofstream fo;

        int debug = 0;

        void readInput(string inPath) {
            fi.open(inPath);
            load();
            fi.close();
        }

        void writeOutput(string outPath) {
            fo.open(outPath);
            vector <pair <int, int>> res = dijkstra();
            for (int i = 1; i < res.size(); ++i)
                fo << res[i].first + 1 << ' ' << res[i].second + 1 << '\n';
            fo.close();
        }
};

class Checker {
#define MAXN 55
   private:
    int grid[MAXN][MAXN];
    int nRow, nCol, tankX, tankY, gasMax;
    /**
     * format string like sprintf()
     *
     * Thanks to https://stackoverflow.com/a/8098080
     */
    string string_format(const string fmt_str, ...) {
        int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
        unique_ptr<char[]> formatted;
        va_list ap;
        while (1) {
            formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
            strcpy(&formatted[0], fmt_str.c_str());
            va_start(ap, fmt_str);
            final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
            va_end(ap);
            if (final_n < 0 || final_n >= n)
                n += abs(final_n - n + 1);
            else
                break;
        }
        return string(formatted.get());
    }
    /**
     * Read input
     *
     * @param inPath path to input file
     *
     * Please remember to close the input stream
     */
    void readInput(string inPath) {
        ifstream inp;
        inp.open(inPath);
        //put your code here
        inp >> tankX >> tankY;
        inp >> gasMax;
        inp >> nRow >> nCol;
        for (int i = 1; i <= nRow; ++i)
            for (int j = 1; j <= nCol; ++j)
                inp >> grid[i][j];
        //remember to close the input stream
        inp.close();
    }

   public:
    /**
     * Verify output
     *
     * @param inPath path to input file
     * @param outPath path to output file
     * @param comment Comment about the output
     * @return a pair of number of stars and length of the path
     * if answer is correct, otherwise it will return {-1, -1}
     *
     */
    pair<int, int> verifyOutput(string inPath, string outPath, string &comment) {
        readInput(inPath);
        ifstream out;
        out.open(outPath);
#define verify(x, s)     \
    if (!(x)) {          \
        comment = s;     \
        out.close();     \
        return {-1, -1}; \
    }
        int nxtX, nxtY;
        int len = 0;
        int cntStar = 0;
        int currentGas = gasMax;
        int cntMapStar = 0;
        for (int i = 1; i <= nRow; ++i)
            for (int j = 1; j <= nCol; ++j)
                cntMapStar += (grid[i][j] == STAR);
        while (out >> nxtX >> nxtY) {
            ++len;
            verify(0 < nxtX && nxtX <= nRow,
                   string_format("Move %d ((%d, %d) -> (%d, %d)) is invalid, expected `X` in range [%d, %d], but found %d",
                                 len, tankX, tankY, nxtX, nxtY, 1, nRow, nxtX));
            verify(0 < nxtY && nxtY <= nCol,
                   string_format("Move %d ((%d, %d) -> (%d, %d)) is invalid, expected `Y` in range [%d, %d], but found %d",
                                 len, tankX, tankY, nxtX, nxtY, 1, nCol, nxtY));
            verify(abs(tankX - nxtX) + abs(tankY - nxtY) == 1,
                   string_format("Move %d ((%d, %d) -> (%d, %d)) is invalid, expected manhattan's distance is 1, found %d",
                                 len, tankX, tankY, nxtX, nxtY, abs(tankX - nxtX) + abs(tankY - nxtY)));
            verify(grid[nxtX][nxtY] != WALL,
                   string_format("Move %d ((%d, %d) -> (%d, %d)) is invalid, tank move into a forbiden cell",
                                 len, tankX, tankY, nxtX, nxtY));
            verify(currentGas != 0,
                   string_format("Move %d ((%d, %d) -> (%d, %d)) is invalid, ran out of gas",
                                 len, tankX, tankY, nxtX, nxtY));
            --currentGas;
            tankX = nxtX;
            tankY = nxtY;
            if (grid[tankX][tankY] == STAR)
                cntStar += 1,
                grid[tankX][tankY] = STREET;
            if (grid[tankX][tankY] == GAS)
                currentGas = gasMax;
        }
        comment = string_format("Map has %d stars", cntMapStar);
        return {cntStar, len};
    }
#undef MAXN
};

int main() {
    Khau sol;
    sol.readInput("map.txt");
    sol.writeOutput("path.txt");

    Checker check;
    string cmt;
    pair <int, int> ans = check.verifyOutput("map.txt", "path.txt", cmt);
    if (ans.first == -1)
        cout << cmt;
    else {
        cout << "Stars collected: " << ans.first << '\n';
        cout << "Path length: " << ans.second;
    }
}
