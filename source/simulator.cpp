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
    _delta = _parser->getDelta();
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
    clock_t mstart = clock();

    int steps = _parser->getSteps();
    spMatrix G(_nodeNum, _nodeNum);
    spMatrix L(_nodeNum, _nodeNum);
    spMatrix C(_nodeNum, _nodeNum);
    initMatrix(*_parser->getResistors(), G);
    initMatrix(*_parser->getInductors(), L);
    initMatrix(*_parser->getCapacitors(), C);
    // the equation is Ax(t+1) = Dx(t) - il
    spMatrix A = G + C + L;
    spMatrix D = -G + C - L;
    VectorXd curr_x(_nodeNum), prev_x(_nodeNum),
                ii(_nodeNum), iv(_nodeNum), il(_nodeNum);
    clock_t mend = clock();
    cout << "total time for build circuit: " << (mend - mstart) / (double) CLOCKS_PER_SEC << "s" << endl;
    il.setZero();
    prev_x.setOnes();
    // cout << iv.rows() << ' ' << iv.cols() << endl;
    Eigen::SimplicialLDLT<spMatrix> solver;
    solver.compute(A);
    if(solver.info() != Eigen::Success) {
      // decomposition failed
      throw std::invalid_argument( "broken nets" );
    }
    ofstream out("./output.txt");
    for(auto t : _parser->_axis_x) out << to_string(t) + ' ';
    out << '\n';
    // change node in getNode for recording
    out << to_string(prev_x[_parser->getNode("n1")])  + ' ';
    for(int i = 1; i < steps; i++){
        ii = getCurrentVector(i) + getCurrentVector(i-1);
        iv = D * prev_x;
        VectorXd rhs = ii + iv - il;
        curr_x = solver.solve(rhs);
        out << to_string(curr_x[_parser->getNode("n1")]) + ' ';
        il = il + (2 * L * (curr_x + prev_x));
        prev_x = curr_x;
    }
    out.close();
}
vector<vector<float>> Simulator::initCurrVector(const vector<component>& srcs){
    int n = srcs.size();
    vector<vector<float>> vec(_nodeNum, vector<float>(0));
    for(int i = 0; i < n; i++){
        int Np = _parser->getNode(srcs[i]._Np);
        vec[Np] = srcs[i]._waveform;
    }
    // for(auto v : vec){
    //     if(v.size() > 1) cout << v.size() << endl;
    // }
    return vec;
}
VectorXd Simulator::getCurrentVector(int step){
    VectorXd vec(_nodeNum);
    vec.setZero();
    vector<component>* csrc = _parser->getCurrentSource();
    for(int i = 0; i < _currNum; i++){
        int Np = _parser->getNode((*csrc)[i]._Np);
        vec(Np) = (*csrc)[i]._waveform[step];
    }
    return vec;
}
void Simulator::initMatrix(const vector<component>& comps, spMatrix& mat){
    int n = comps.size();
    spMatrix A(n, _nodeNum), diagG(n, n);
    for(int i = 0; i < n; i++){
        string np = (comps[i])._Np, nn = (comps[i])._Nn;
        float val = (comps[i])._val;
        diagG.insert(i, i) = val;
        if(np != "0" && np != "gnd"){
            int Np = _parser->getNode(np);
            A.insert(i, Np) = 1;
        }
        if(nn != "0" && nn != "gnd"){
            int Nn = _parser->getNode(nn);
            A.insert(i, Nn) = -1;
        }
    }
    mat = A.transpose() * diagG * A;
    // cout << mat.rows() << endl;
    // cout << mat.cols() << endl;
}

int main(){
    clock_t mstart = clock();
    Simulator simulator("../python/tstCir2.sp");
    simulator.simulate();
    clock_t mend = clock();
    cout << "total time: " << (mend - mstart) / (double) CLOCKS_PER_SEC << "s" << endl;
}
