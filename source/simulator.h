#include "netListParser.h"

class Simulator
{
public:
    Simulator();

    ~Simulator() = default;
    void simulate(string fileName);
    void buildMatrixG();
    void buildMatrixC();
    void buildMatrixL();
private:
    Parser _parser;
};
