#include "Circuit.h"

Circuit::Circuit(){
    if(getenv("DEBUG"))
        _debug = true;
};

Circuit::~Circuit(){

};

void Circuit::addResistor(std::string* words){
    if(_debug)
        std::cout << "add resistor: " << words[0] << std::endl;
    Component comp = {words[1], words[2], stof(words[3])};
    _Resistors.push_back(comp);

    if (_Nodes.find(words[1]) == _Nodes.end() && words[1] != "gnd" && words[1] != "GND") {
        _NodeMap.insert(std::make_pair(words[1], _Nodes.size()));
        _Nodes.insert(words[1]);
        _nodeVec.push_back(words[1]);
    }
    if (_Nodes.find(words[2]) == _Nodes.end() && words[2] != "gnd" && words[2] != "GND") {
        _NodeMap.insert(std::pair<std::string, int>(words[2], _Nodes.size()));
        _Nodes.insert(words[2]);
        _nodeVec.push_back(words[2]);
    }
};

void Circuit::addCapacitor(std::string* words){
    //TODO

};

void Circuit::addCurrentSource(std::string* words){
    if(_debug)
        std::cout << "add current source: " << words[0] << std::endl;
    Component comp = {words[1], words[2], stof(words[3])};
    _CurrentSource.push_back(comp);
};

void Circuit::initAdjacencyMatrix(){
    int m = _Resistors.size(); // amount of Components
    int n = _Nodes.size(); // amount of nodes
    _adjacencyMatrix = Eigen::MatrixXd(n,n);
    // std::cout << m << std::endl;
    for (int i = 0; i < m; i++){
        // std::cout << this->NodeMap[Components[i].Np] << std::endl;
        // std::cout << this->NodeMap[Components[i].Nn] << std::endl;
        std::string Np, Nn;
        Np = _Resistors[i].Np;
        Nn = _Resistors[i].Nn;
        float val = _Resistors[i].value;
        if (Np != "gnd" && Nn != "gnd"){
            _adjacencyMatrix(_NodeMap[Np], _NodeMap[Nn]) += val;
            _adjacencyMatrix(_NodeMap[Nn], _NodeMap[Np]) += val;
        }
    }
};

void Circuit::getAdmittanceMatrix(Eigen::MatrixXd& matrix){
    int m = _Resistors.size(); // amount of Components
    int n = _Nodes.size(); // amount of nodes
    if(getenv("DEBUG"))
        std::cout << "total nodes: " << n << std::endl;
    Eigen::MatrixXd admittanceMatrix = Eigen::MatrixXd::Zero(n,n);
    std::string gnd = "gndGND0";
    for (int i = 0; i < m; i++){
        std::string Np, Nn;
        Np = _Resistors[i].Np;
        Nn = _Resistors[i].Nn;
        float val = _Resistors[i].value;
        if (gnd.find(Np) == std::string::npos)
            admittanceMatrix(_NodeMap[Np], _NodeMap[Np]) += 1/_Resistors[i].value;
        if (gnd.find(Nn) == std::string::npos)
            admittanceMatrix(_NodeMap[Nn], _NodeMap[Nn]) += 1/val;
        if (gnd.find(Np) == std::string::npos && gnd.find(Nn) == std::string::npos){
            admittanceMatrix(_NodeMap[Np], _NodeMap[Nn]) -= 1/val;
            admittanceMatrix(_NodeMap[Nn], _NodeMap[Np]) -= 1/val;
        }
    }
    matrix = admittanceMatrix;
};

void Circuit::getCurrentVector(Eigen::MatrixXd& matrix){
    int m = _CurrentSource.size(); // amount of current source
    int n = _Nodes.size();
    Eigen::MatrixXd vector = Eigen::MatrixXd::Zero(n,1);
    matrix = vector;
    for (int i = 0; i < m; i++){
        std::string Np = _CurrentSource[i].Np;
        matrix(_NodeMap[Np]) -= _CurrentSource[i].value;
    }
    if(_debug)
        std::cout << matrix << std::endl;
};

void Circuit::solve(){
    Eigen::MatrixXd admittanceMatrix;
    Eigen::MatrixXd currentVector;
    getAdmittanceMatrix(admittanceMatrix);
    getCurrentVector(currentVector);
    _nodalVoltage = admittanceMatrix.colPivHouseholderQr().solve(currentVector);
    // if(_debug)
        // std::cout << _nodalVoltage << std::endl;
};

void Circuit::printNodalVoltage(){
    int index = 0;
    for(auto node : _nodeVec){
        std::cout << node + ": "
        << _nodalVoltage(index++)
        << std::endl;
    }
}
