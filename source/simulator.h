#include "netListParser.h"
#include "../Eigen/Eigen"
#include "../Eigen/SparseLU"
#include "../Eigen/SparseCholesky"
typedef Eigen::SparseMatrix<double> spMatrix;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> VectorXd;

class Simulator
{
public:
    Simulator(string fileName);
    ~Simulator();

    void simulate();
    void initMatrix(const vector<component>& comps, spMatrix& mat);
    vector<vector<float>> initCurrVector(const vector<component>& srcs);
    VectorXd getCurrentVector(int step);
    void buildCircuit(spMatrix& A, spMatrix& D, spMatrix& L);
    void createMatrix();
    void createVector();
    void buildDcCircuit();
    void buildTransientCircuit();
    void dcAnalysis();
    void transientAnalysis();


private:
    Parser *_parser;
    int _resNum, _indNum, _capNum, _currNum, _volNum, _nodeNum;
    float _delta;
    // vector<component> *_resistors, *_capacitors, *_inductors;
    // vector<component> *_voltageSource, *_currentSource;
    // unordered_map<string, int> *_nodeMap;
};
