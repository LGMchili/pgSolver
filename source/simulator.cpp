#include "simulator.h"
// typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> MatrixX;

Simulator::Simulator(string fileName){
    _parser = new Parser();
    _parser->parse(fileName);
    _resNum = (_parser->getResistors())->size();
    _indNum = (_parser->getInductors())->size();
    _capNum = (_parser->getCapacitors())->size();
    _currNum = (_parser->getCurrentSource())->size();
    _volNum = (_parser->getVoltageSource())->size();
    _nodeNum = (_parser->getNodeMap())->size();
    _delta_t = _parser->getDelta();
    // _resistors = _parser->getResistors();
    // _inductors = _parser->getInductors();
    // _capacitors = _parser->getCapacitors();
    // _currentSource = _parser->getCurrentSource();
    // _voltageSource = _parser->getVoltageSource();
    // _nodeMap = _parser->getNodeMap();
}

Simulator::~Simulator(){
    delete _parser;
}

void Simulator::simulate(){
    spMatrix G(_nodeNum, _nodeNum);
    // spMatrix L(_nodeNum, _nodeNum);
    // spMatrix C(_nodeNum, _nodeNum);
    initMatrix(*_parser->getResistors(), G);
    // cout << G << endl;
    // initMatrix(*_parser->getInductors(), L);
    // initMatrix(*_parser->getCapacitors(), C);
}

void Simulator::initMatrix(const vector<component>& comps, spMatrix& mat){
    for(int i = 0; i < comps.size(); i++){
        string np = (comps[i])._Np, nn = (comps[i])._Nn;
        float val = (comps[i])._val;
        if(np != "0" && np != "gnd" && nn != "0" && nn != "gnd"){
            int Np = _parser->getNode(np), Nn = _parser->getNode(nn);
            mat.insert(Np, Nn) = -1 / val;
            mat.insert(Nn, Np) = -1 / val;
            mat.coeffRef(Np, Np) += 1 / val;
            mat.coeffRef(Nn, Nn) += 1 / val;
        }
        else if(np != "0" && np != "gnd"){
            int Np = _parser->getNode(np);
            mat.coeffRef(Np, Np) += 1 / val;
        }
        else{
            int Nn = _parser->getNode(nn);
            mat.coeffRef(Nn, Nn) += 1 / val;
        }
    }
}

int main(){
    clock_t mstart = clock();
    Simulator simulator("../python/tstCir.sp");
    simulator.simulate();
    clock_t mend = clock();
    cout << "total time: " << (mend - mstart) / (double) CLOCKS_PER_SEC << "s" << endl;
}
