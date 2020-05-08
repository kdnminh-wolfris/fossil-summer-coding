#include <stdarg.h>  // For va_start, etc.
#include <string.h>
#include <queue>
#include <vector>
#include <utility>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>  // For std::unique_ptr

using namespace std;

#define WALL 0
#define STREET 1
#define GAS 2
#define STAR 3
#define EARNED_STAR 4

class SolverTemplate {
   public:
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

        //remember to close the input stream
        inp.close();
    }
    /**
     * Calculate the answer and write to output file 
     * 
     * @param outPath path to output file
     * 
     * Please remember to close the output stream
     */
    void writeOutput(string outPath) {
        ofstream out;
        out.open(outPath);
        //put your code here

        //remember to close the output stream
        out.close();
    }
};

class ThucSolver {
#define MAXN 100
#define PATH_LIMIT 1000000
   private:
    
    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};

    int grid[MAXN + 2][MAXN + 2];
    int nRow, nCol, tankX, tankY, gasMax;  

    int d[MAXN * MAXN + 5];
    int tIn[MAXN * MAXN + 5], tOut[MAXN * MAXN + 5], nodeAtTime[MAXN * MAXN + 5];
    int f[20][MAXN * MAXN + 5];
    int DFS_time;

    vector<pair<int, int> > path;
    int re_path[MAXN * MAXN + 5];

    //segment tree
    struct TNode {
        pair<int, int> star, gas; //closest star and closest gas
    } st[MAXN * MAXN << 2];
    int lz[MAXN * MAXN];
#undef MAXN
#define mid ((l + r) >> 1)
#define lc (id << 1)
#define rc (id << 1 | 1)
    TNode merge(const TNode &l, const TNode &r) {
        return {min(l.star, r.star), min(l.gas, r.gas)};
    }
    void build(int id, int l, int r) {
        if (l == r) {
            int u = nodeAtTime[l];
            int y = (u - 1) % nCol + 1;
            int x = (u - 1) / nCol + 1;
            st[id] = {{1e9, 0}, {1e9, 0}};
            if (d[u] != 0) {
                if (grid[x][y] == STAR)
                    st[id].star = {d[u] - 1, u};
                if (grid[x][y] == GAS)
                    st[id].gas = {d[u] - 1, u};
            }
            return;
        }
        build(lc, l, mid);
        build(rc, mid + 1, r);
        st[id] = merge(st[lc], st[rc]);
    }
    void update(int id, int diff) {
        st[id].star.first += diff;
        if (st[id].star.first > 1e9)
            st[id].star.first = 1e9;
        st[id].gas.first += diff;
        if (st[id].gas.first > 1e9)
            st[id].gas.first = 1e9;
        lz[id] += diff;
    }
    void down(int id) {
        int &tmp = lz[id];
        if (tmp == 0) return;
        update(lc, tmp);
        update(rc, tmp);
        tmp = 0;
    }
    void updateDistance(int id, int l, int r, int u, int v, int diff) {
        if (r < u || v < l) return;
        if (u <= l && r <= v) {
            update(id, diff);
            return;
        }
        down(id);
        updateDistance(lc, l, mid, u, v, diff);
        updateDistance(rc, mid + 1, r, u, v, diff);
        st[id] = merge(st[lc], st[rc]);
    }

#undef mid
#undef lc
#undef rc

    /**
     * get closest star of the current position
     * 
     * @return a pair with the first is the distance (in BFS tree) to a star
     * the second is the position of the star
     * @return (?, -1) if these is no stars 
     */  
    pair<int, int> getClosestStar() {
        return st[1].star;
    }
    /**
     * get closest gas station of the current position
     * 
     * @return a pair with the first is the distance (in BFS tree) to a gas station
     * the second is the position of the gas station
     * @return (?, -1) if these is no gas station 
     */  
    pair<int, int> getClosestGas() {
        return st[1].gas;
    }

#define id(x, y) ((x - 1) * nCol + (y))
    void BFS() {
        queue<pair<int, int> > q;
        q.push({tankX, tankY});
        d[id(tankX, tankY)] = 1;
        while (!q.empty()) {
            auto [x, y] = q.front(); q.pop();
            int u = id(x, y);
            int du = d[u] + 1;
            for (int i = 0; i < 4; ++i) {
                int xx = x + dx[i], yy = y + dy[i];

                if (xx < 1 || yy < 1 || xx > nRow || yy > nCol)
                    continue;
                
                if (grid[xx][yy] == WALL)
                    continue;
                int v = id(xx, yy);
                if (d[v] == 0) {
                    d[v] = du;
                    f[0][v] = u;
                    q.push({xx, yy});
                }
            }
        }
    }
    void DFS(int x, int y) {
        int u = id(x, y);
        tIn[u] = ++DFS_time;
        nodeAtTime[DFS_time] = u;
        for (int i = 0; i < 4; ++i) {
            int xx = x + dx[i], yy = y + dy[i];
            int v = id(xx, yy);
            if (f[0][v] != u)
                continue;
            for (int j = 1; j < 19; ++j)
                f[j][v] = f[j - 1][f[j - 1][v]];
            DFS(xx, yy);
        }
        tOut[u] = DFS_time;
    }
    void initGraph() {
        cout << "BFS ...\n";
        BFS();
        //build shortest-path-tree, LCA, ...
        DFS_time = 0;
        cout << "Build LCA ...\n";
        memset(tIn, 0, sizeof(tIn));
        DFS(tankX, tankY); 
        cout << "Build segment tree ...\n";
        build(1, 1, DFS_time);
    }

    /**
     * check whether u is an ancestor of v on BFS tree or not
     * 
     * @return true if u is  an ancestor of v on BFS tree
     */  
    bool isAncestor(int u, int v) {
        return tIn[u] < tIn[v] && tOut[v] <= tOut[u];
    }
    int LCA(int u, int v) {
        if (d[u] > d[v]) swap(u, v);
        //d[u] <= d[v]
        for (int j = 19; j >= 0; --j)
            if (d[v] - (1 << j) >= d[u])
                v = f[j][v];
        if (u == v) return v;
        for (int j = 19; j >= 0; --j)
            if (f[j][v] != f[j][u])
                v = f[j][v], u = f[j][u];
        return f[0][u];
    }

    /**
     * Move tank from current position to the given position
     * 
     * @return true if the tank earned at least 1 star in the path
     */ 
    bool move_to(int nodeId, int &currentGas) {
        int tankId = id(tankX, tankY);
        int lca = LCA(tankId, nodeId);
        bool earned = false;
        while (tankId != lca && currentGas > 0) {
            //update segment tree
            update(1, -1);
            updateDistance(1, 1, DFS_time, tIn[tankId], tOut[tankId], 2);

            tankId = f[0][tankId];
            int y = (tankId - 1) % nCol + 1;
            int x = (tankId - 1) / nCol + 1;
            path.emplace_back(x, y);
            --currentGas;
            if (grid[x][y] == GAS)
                currentGas = gasMax;
            if (grid[x][y] == STAR) {
                updateDistance(1, 1, DFS_time, tIn[tankId], tIn[tankId], 1e9);
                grid[x][y] = EARNED_STAR,
                earned = true;
            }
        }
        
        int top = 0;
        while (nodeId != lca) {
            re_path[++top] = nodeId;
            nodeId = f[0][nodeId];
        }
        while (top) {
            int tankId = re_path[top--];
            //update segment tree
            update(1, 1);
            updateDistance(1, 1, DFS_time, tIn[tankId], tOut[tankId], -2);

            int y = (tankId - 1) % nCol + 1;
            int x = (tankId - 1) / nCol + 1;
            path.emplace_back(x, y);
            --currentGas;
            if (grid[x][y] == GAS)
                currentGas = gasMax;
            if (grid[x][y] == STAR) {
                updateDistance(1, 1, DFS_time, tIn[tankId], tIn[tankId], 1e9);
                grid[x][y] = EARNED_STAR,
                earned = true;
            }
        }
        if (path.size())
            tankX = path.back().first,
            tankY = path.back().second;
        return earned;
    }

   public:
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

#define PRIORITY GAS
    /**
     * Calculate the answer and write to output file 
     * 
     * @param outPath path to output file
     * 
     * Please remember to close the output stream
     */
    void writeOutput(string outPath) {
        ofstream out;
        out.open(outPath);
        //put your code here
        initGraph();
        int len = 0;
        int currentGas = gasMax;
        int cnt = 0;
        if (grid[tankX][tankY] == STAR) {
            updateDistance(1, 1, DFS_time, tIn[id(tankX, tankY)], tIn[id(tankX, tankY)], 1e9);
            grid[tankX][tankY] = EARNED_STAR;
        }
        while (cnt < 100) {
            ++cnt;
            cout << path.size() << '\n';
            if (grid[tankX][tankY] == GAS)
                currentGas = gasMax;
            auto [distToGas, gasID] = getClosestGas();
            auto [distToStar, starID] = getClosestStar();
            if (distToStar + len > PATH_LIMIT) break;
            if (grid[tankX][tankY] == GAS && distToStar > currentGas) break;
            if (PRIORITY == STAR) {
                if (distToStar > currentGas) {
                    if (distToGas == 0 || distToGas + len > PATH_LIMIT) break;
                    if (distToGas > currentGas) break;
                    //need to go to the gas station
                    if (move_to(gasID, currentGas))
                        cnt = 0;
                    continue;
                }
                if (move_to(starID, currentGas))
                    cnt = 0;
            }
            else {
                if (distToGas > currentGas || currentGas == gasMax) {
                    if (distToStar + len > PATH_LIMIT) break;
                    if (distToStar <= currentGas) {
                        if (move_to(starID, currentGas))
                            cnt = 0;
                        continue;
                    }
                }
                if (distToGas <= currentGas)
                    if (move_to(gasID, currentGas))
                        cnt = 0;
            }
        }
        //remember to close the output stream
        while (path.size()) {
            int x = path.back().first;
            int y = path.back().second;
            if (grid[x][y] != EARNED_STAR)
                path.pop_back();
            else 
                break;
        } 
        for (auto [x, y] : path)
            out << x << ' ' << y << '\n';
        out.close();
    }
#undef id
#undef PATH_LIMIT
};

class Checker {
#define MAXN 1005
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
    ThucSolver solver;
    solver.readInput("fossil-summer-coding/test/sampleInput.txt");
    solver.writeOutput("fossil-summer-coding/test/testing.txt");
    Checker checker;
    string comment;
    pair<int, int> result = checker.verifyOutput("fossil-summer-coding/test/sampleInput.txt", "fossil-summer-coding/test/testing.txt", comment);
    if (result.first == -1) {
        cout << comment;
    } else {
        cout << "Correct answer\n";
        cout << comment << '\n';
        cout << "Number of earned stars: " << result.first << '\n';
        cout << "Path's length: " << result.second << '\n';
    }
    return 0;
}