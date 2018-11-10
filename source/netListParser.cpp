#include "netListParser.h"

using namespace std;

Parser::Parser():_debug(false), _threadNum(16), _steps(1), _nodeSize(0), _resNum(0), _indNum(0), _capNum(0){
    _resistors = new vector<component>;
    _inductors = new vector<component>;
    _capacitors = new vector<component>;
    _currentSource = new vector<component>;
    _voltageSource = new vector<component>;
    _nodeMap = new unordered_map<string, u32>;
    // clock_t mstart = clock();
    // parse(fileName);
    // clock_t mend = clock();
    // cout << "total time: " << (mend - mstart) / (double) CLOCKS_PER_SEC << "s" << endl;
    if(getenv("DEBUG"))
        _debug = true;
};

Parser::~Parser(){
    delete _resistors;
    delete _inductors;
    delete _capacitors;
    delete _currentSource;
    delete _voltageSource;
    delete _nodeMap;
}

void Parser::parse(string fileName){
    //
    clock_t mstart = clock();
    ifstream file;
    file.open(fileName);
    string sLine = "";
    while (!file.eof()) {
        getline(file, sLine);
        if(sLine.empty() || sLine[0] == '*'){
            continue;
        }
        toLower(sLine);
        if(sLine[0] == 'r') addNode(sLine);
        _lines.push_back(sLine);
    }
    // multi thread
    // ThreadPool* pool = new ThreadPool(_threadNum);
    // for(int i = 0; i < _lines.size(); ++i) {
    //     // cout << _lines[i] << endl;
    //     pool->enqueue([&, i]{
    //             // std::unique_lock<std::mutex> lock(mutex);
    //             parseLine(_lines[i]);
    //             // cout << _lines[i] << endl;
    //         }
    //     );
    // }
    // delete pool;
    // single thread
    for(string& l : _lines){
        parseLine(l);
    }
    cout << "total nodes: " << _nodeSize << endl;
    cout << "total resistors: " << _resNum << endl;
    cout << "total inductors: " << _indNum << endl;
    cout << "total capacitors: " << _capNum << endl;
    cout << "total current source: " << _currentSource->size() << endl;
    cout << "total simulation steps: " << _steps << endl;
    initCurrentSource();
    clock_t mend = clock();
    cout << "total time for parsing: " << (mend - mstart) / (double) CLOCKS_PER_SEC << "s" << endl;
    // vector<float> v(_steps, 0);
    // cout << "processed lines: " << _tst.size() << endl;
}

void Parser::parseLine(const string& line){
    if(line[0] == '.'){
        addDirective(line);
    }
    else{
        if(line[0] == 'i'){
            addCurrentSource(line);
        }
        else{
            addPassive(line);
        }
    }
}
void Parser::addNode(const string& line){
    vector<string> words;
    split(words, line);
    if(words[1] != "gnd" && words[1] != "0" && _nodeMap->find(words[1]) == _nodeMap->end()){
        (*_nodeMap)[words[1]] = _nodeSize;
        // cout << "add node: " << words[1] << " as " << _nodeSize << endl;
        _nodeSize++;
    }
    if(words[2] != "gnd" && words[2] != "0" && _nodeMap->find(words[2]) == _nodeMap->end()){
        (*_nodeMap)[words[2]] = _nodeSize;
        // cout << "add node: " << words[2] << " as " << _nodeSize << endl;
        _nodeSize++;
    }
}
void Parser::addComponent(string Np, string Nn, float val, pairMap& comps){
    // unique_lock<mutex> lck(_mtx);
    // build map for diagonal elements
    if(Np != "0" && Np != "gnd" && Nn != "0" && Nn != "gnd"){
        int np = getNode(Np), nn = getNode(Nn);
        pair<int, int> npnn(np, nn), nnnp(nn, np), nnnn(nn, nn), npnp(np, np);
        comps[npnn] = -1 / val;
        comps[nnnp] = -1 / val;
        comps[nnnn] += 1 / val;
        comps[npnp] += 1 / val;
    }
    else if(Np != "0" && Np != "gnd"){
        int np = (*_nodeMap)[Np];
        pair<int, int> npnp(np, np);
        comps[npnp] += 1 / val;
    }
    else{
        int nn = (*_nodeMap)[Nn];
        pair<int, int> nnnn(nn, nn);
        comps[nnnn] += 1 / val;
    }
}
void Parser::addDirective(const string& line){
    vector<string> words;
    split(words, line);
    float start = stof(words[1]);
    float end = stof(words[2]);
    _delta = stof(words[3]);
    // unique_lock<mutex> lck(_mtx);
    _steps = (end - start) / _delta + 1;
    for(int i = 0; i < _steps; i++)
        _axis_x.push_back(i * _delta);
}

void Parser::addCurrentSource(const string& line){
    vector<string> words;
    split(words, line);
    if(words.size() == 4){
        // dc source
        addDcCurrent(words[0], words[1], words[2], stof(words[3]));
    }
    else if(words.size() > 4){
        string name = words[0];
        string Np = words[1];
        string Nn = words[2];
        words.erase(words.begin(), words.begin() + 4);
        vector<float> vals;
        for(auto& word : words) vals.push_back(stof(word));
        addPwlCurrent(name, Np, Nn, vals);
    }
}

void Parser::addPassive(const string& line){
    string str(line);
    string buf;                 // Have a buffer string
    stringstream ss(str);       // Insert the string into a stream
    vector<string> words; // Create vector to hold our words
    while (ss >> buf){
        words.push_back(buf);
    }
    if(words[0][0] == 'r'){
        addResistor(words[0], words[1], words[2], stof(words[3]));
    }
    else if(words[0][0] == 'l'){
        addInductor(words[0], words[1], words[2], stof(words[3]));
    }
    else if(words[0][0] == 'c'){
        addCapacitor(words[0], words[1], words[2], stof(words[3]));
    }
}

void Parser::interp(vector<float>& result, vector<float>& xp, vector<float>& fp){
    // TODO: error when global delta less than delta in .tran
    if(xp.size() <= 1) return;
    vector<float> k;
    if(xp[0] != 0){
        xp.insert(xp.begin(), 0);
        fp.insert(fp.begin(), 0);
    }
    if(xp.back() != 0){
        xp.push_back(_axis_x.back());
        fp.push_back(0);
    }
    for(int i = 0; i < xp.size() - 1; i++){
        // cout << xp[i] << ", " << fp[i] << endl;
        float slope = (fp[i+1] - fp[i]) / (xp[i+1] - xp[i]);
        k.push_back(slope);
        // cout << slope << endl;
    }
    int curr = 0;
    for(int j = 0; j < _steps; j++){
        if(_axis_x[j] > xp[curr + 1]) curr++;
        result[j] = (fp[curr] + (_axis_x[j] - xp[curr]) * k[curr]);
    }
    // for(auto val : result) cout << val <<endl;
}

void Parser::initCurrentSource(){
    for(auto& csrc : *_currentSource){
        vector<float> t, v, y(_steps, 0);
        if(!csrc.isDC()){
            // PWL source
            for(int i = 0; i < csrc._waveform.size(); i+=2){
                t.push_back(-csrc._waveform[i]);
                v.push_back(-csrc._waveform[i+1]);
            }
            interp(y, t, v);
            // ofstream out("./output.txt");
            // for(auto t : _axis_x) out << to_string(t) + ' ';
            // out << '\n';
            // for(auto val : y) out << to_string(val) + ' ';
            // out.close();
        }
        else{
            // DC source
            vector<float> y;
            for(int i = 0; i < _steps; i++){
                y.push_back(csrc._val);
            }
            // for(auto v : y) cout << v << endl;
            // cout << y.size() << endl;
        }
        csrc._waveform = y;
    }
}
