#include "netListParser.h"
#include "../Eigen/Eigen"
typedef Eigen::SparseMatrix<double> spMatrix;


class Simulator
{
public:
    Simulator(string fileName);
    ~Simulator();

    void simulate();
    void initMatrix(const vector<component>& comps, spMatrix& mat);
    void createMatrix();
    void createVector();
    void buildDcCircuit();
    void buildTransientCircuit();
    void dcAnalysis();
    void transientAnalysis();


private:
    Parser *_parser;
    int _resNum, _indNum, _capNum, _currNum, _volNum, _nodeNum;
    float _delta_t;
    // vector<component> *_resistors, *_capacitors, *_inductors;
    // vector<component> *_voltageSource, *_currentSource;
    // unordered_map<string, int> *_nodeMap;
};
