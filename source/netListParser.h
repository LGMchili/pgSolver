#ifndef __PARSER_H__
#define __PARSER_H__
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <thread>
#include <mutex>
#include <sstream>
using namespace std;
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;
    }
};
typedef unsigned u32;
typedef pair<int, int> node_pair;
typedef unordered_map<node_pair, float, pair_hash> pairMap;
struct component
{
    component(string name, string Np, string Nn, float val){
        _name = name;
        _Np = Np;
        _Nn = Nn;
        _val = val;
    }
    component(string name, string Np, string Nn, vector<float>& val){
        _name = name;
        _Np = Np;
        _Nn = Nn;
        // _val = val;
        _isDC = false;
        _waveform = val;
    }
    bool isDC(){ return _isDC; }
    string _name, _Np, _Nn;
    float _val;
    bool _isDC = true;
    vector<float> _waveform;
};
class Parser{
public:
    Parser();
    ~Parser();
    void parse(string fileName);
    void parseLine(const string& line);
    void addDirective(const string& line);
    void addPassive(const string& line);
    void addCurrentSource(const string& line);
    void initCurrentSource();
    inline vector<component>* getResistors(){ return _resistors; }
    inline vector<component>* getInductors(){ return _inductors; }
    inline vector<component>* getCapacitors(){ return _capacitors; }
    inline vector<component>* getCurrentSource(){ return _currentSource; }
    inline vector<component>* getVoltageSource(){ return _voltageSource; }
    // inline pairMap getResistorsMap(){ return _resistorsMap; }
    inline unordered_map<string, u32>* getNodeMap(){ return _nodeMap; }
    inline int getNode(string str) { return (*_nodeMap)[str]; }
    inline float getDelta() { return _delta; }
    inline int getSteps(){ return _steps; }

    vector<float> _axis_x;
private:
    void split(vector<string>& words, const string& line){
        string str(line);
        string buf;                 // Have a buffer string
        stringstream ss(str);       // Insert the string into a stream
        while (ss >> buf){
            words.push_back(buf);
        }
    }
    void addNode(const string& line);
    void addComponent(string Np, string Nn, float val, pairMap& comps);
    void interp(vector<float>& result, vector<float>& xp, vector<float>& fp);
    inline void toLower(string& str){
        for(char& cr : str){
            if(cr <= 'Z' && cr >= 'A'){
                cr += 32; // 'Z' - 'z' = -32
            }
        }
    }
    inline void addResistor(string name, string Np, string Nn, float val){
        // unique_lock<mutex> lck(_mtx);
        _resistors->emplace_back(name, Np, Nn, 1 / val);
        // addComponent(Np, Nn, val, _resistorsMap);
        _resNum++;
    }
    inline void addCapacitor(string name, string Np, string Nn, float val){
        // unique_lock<mutex> lck(_mtx);
        _capacitors->emplace_back(name, Np, Nn, 2 / _delta * val);
        // addComponent(Np, Nn, val, _capacitorsMap);
        _capNum++;
    }
    inline void addInductor(string name, string Np, string Nn, float val){
        // unique_lock<mutex> lck(_mtx);
        _inductors->emplace_back(name, Np, Nn, _delta / (2 * val));
        // addComponent(Np, Nn, val, _inductorsMap);
        _indNum++;
    }
    inline void addDcCurrent(string name, string Np, string Nn, float val){
        // unique_lock<mutex> lck(_mtx);
        _currentSource->emplace_back(name, Np, Nn, val);
        return;
    }
    inline void addPwlCurrent(string name, string Np, string Nn, vector<float> val){
        // unique_lock<mutex> lck(_mtx);
        _currentSource->emplace_back(name, Np, Nn, val);
    }
    inline void addDirective(float start, float end, float delta){
        _steps = (end - start) / delta + 1;
    }
    bool _debug = false;
    unsigned _threadNum;
    int _steps;
    u32 _nodeSize, _resNum, _indNum, _capNum;
    float _delta;
    string _fileName;
    vector<string> _lines;
    vector<string> _tst;
    mutex _mtx;
    vector<function<void()> > _tasks;
    pairMap _resistorsMap;
    vector<component> *_resistors, *_capacitors, *_inductors;
    vector<component> *_voltageSource, *_currentSource;
    unordered_map<string, u32> *_nodeMap;
};

#endif
