#include <stdarg.h>  // For va_start, etc.
#include <string.h>
#include <memory>  // For std::unique_ptr

#include <fstream>
#include <iostream>

using namespace std;
class ThucSolve {
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

class Checker {
#define MAXN 2005
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
        while (out >> nxtX >> nxtY) {
            ++len;
            verify(0 < nxtX && nxtX <= nRow,
                   string_format("Move %d ((%d, %d) -> (%d, %d)) is invalid, expected `X` in range [%d, %d], but found %d",
                                 len, tankX, tankY, nxtX, nxtY, 1, nRow, nxtX));
            verify(0 < nxtY && nxtY <= nRow,
                   string_format("Move %d ((%d, %d) -> (%d, %d)) is invalid, expected `Y` in range [%d, %d], but found %d",
                                 len, tankX, tankY, nxtX, nxtY, 1, nCol, nxtY));
            verify(abs(tankX - nxtX) + abs(tankY - nxtY) == 1,
                   string_format("Move %d ((%d, %d) -> (%d, %d)) is invalid, expected manhattan's distance is 1, found %d",
                                 len, tankX, tankY, nxtX, nxtY, abs(tankX - nxtX) + abs(tankY - nxtY)));
            verify(grid[nxtX][nxtY] != 0,
                   string_format("Move %d ((%d, %d) -> (%d, %d)) is invalid, tank move into a forbiden cell",
                                 len, tankX, tankY, nxtX, nxtY));
            verify(currentGas != 0, 
                string_format("Move %d ((%d, %d) -> (%d, %d)) is invalid, ran out of gas",
                len, tankX, tankY, nxtX, nxtY));
            --currentGas;
            tankX = nxtX;
            tankY = nxtY;
            if (grid[tankX][tankY] == 2)
                cntStar += 1;
            if (grid[tankX][tankY] == 3)
                currentGas = gasMax;
        }
        return {cntStar, len};
    }
#undef MAXN
};
int main() {
    ThucSolve sol1;
    sol1.readInput("sample.inp");
    sol1.writeOutput("sample.out");
    Checker checker;
    string comment;
    pair<int, int> result = checker.verifyOutput("fossil-summer-coding/test/sampleInput.txt", "fossil-summer-coding/test/sampleOutput.txt", comment);
    if (result.first == -1) {
        cout << comment;
    } else {
        cout << "Correct answer\n";
        cout << "Number of stars: " << result.first << '\n';
        cout << "Path's length: " << result.second << '\n';
    }
    return 0;
}