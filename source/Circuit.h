#ifndef __Circuit_H__
#define __Circuit_H__
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <cfloat>
#include "../Eigen/Eigen"
#include <unordered_map>

struct Component {
    std::string Np;
    std::string Nn;
    float value;
};
class Circuit{
public:
    Circuit();
    ~Circuit();
    void addResistor(std::string* words);
    void addCapacitor(std::string* words);
    void addInductor(std::string* words);
    void AddVoltageSource(std::string* words);
    void addCurrentSource(std::string* words);
    void initAdjacencyMatrix();
    void init_shortest_path();
    void convert_matrix_to_MNA(Eigen::MatrixXd& matrix);
    void getAdmittanceMatrix(Eigen::MatrixXd& matrix);
    void getCurrentVector(Eigen::MatrixXd& matrix);
    void solve();
    void printNodalVoltage();
    Eigen::MatrixXd get_current_vector(int x);

private:
    std::map<std::string, int> _NodeMap;
    std::set<std::string> _Nodes;
    std::vector<float> _ShortestPath;
    std::vector<Component> _Resistors;
    std::vector<Component> _VoltageSource;
    std::vector<Component> _CurrentSource;
    std::vector<std::string> _nodeVec;
    Eigen::MatrixXd _adjacencyMatrix;
    Eigen::MatrixXd _nodalVoltage;
    bool _debug = false;
};
#endif
