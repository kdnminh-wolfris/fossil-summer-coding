#include <fstream>
using namespace std;
class ThucSolve {
    public:
        void readInput(string inPath) {
            ifstream inp;
            inp.open(inPath);
            //put your code here

            //remember to close the input stream
            inp.close();
        }
        void writeOutput(string outPath) {
            ofstream out;
            out.open(outPath);
            //put your code here

            //remember to close the output stream
            out.close();
        }
};

int main() {
    ThucSolve sol1;
    sol1.readInput("sample.inp");
    sol1.writeOutput("sample.out");
    return 0;
}