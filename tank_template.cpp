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
    public:
        /**
         * Verify output
         * 
         * @param inPath path to input file
         * @param outPath path to output file
         * @param comment Comment about the output
         * @return a pair of number of stars and length of the path
         * if answer is correct, otherwise it will return {-1, -1}
         */
        pair<int, int> verifyOutput(string inPath, string outPath, string &comment) {
            //todo later;
            return {-1, -1};
        }
};
int main() {
    ThucSolve sol1;
    sol1.readInput("sample.inp");
    sol1.writeOutput("sample.out");
    Checker checker;
    string comment;
    pair<int, int> result = checker.verifyOutput("sample.inp", "sample.out", comment);
    if (result.first == -1) {
        cout << comment;
    }
    else {
        cout << "Correct answers\n";
        cout << "Number of stars: " << result.first << '\n';
        cout << "Path's length: " << result.second << '\n';
    }
    return 0;
}