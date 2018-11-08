#include "netListParser.h"
#include "../Eigen/Eigen"

class Simulator
{
public:
    Simulator(string fileName);
    ~Simulator();

    void simulate();
    void buildMatrixG();
    void buildMatrixC();
    void buildMatrixL();
    void createMatrix();
    void createVector();
    void buildDcCircuit();
    void buildTransientCircuit();
    void dcAnalysis();
    void transientAnalysis();


private:
    Parser *_parser;
    int resNum, indNum, capNum, currNum, volNum, nodeNode;
    // vector<component> *_resistors, *_capacitors, *_inductors;
    // vector<component> *_voltageSource, *_currentSource;
    // unordered_map<string, int> *_nodeMap;
};
