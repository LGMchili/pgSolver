#include "netListParser.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include "ThreadPool.h"
using namespace std;

Parser::Parser():_debug(false), _threadNum(16), _steps(1){
    _resistors = new vector<component>;
    _inductors = new vector<component>;
    _capacitors = new vector<component>;
    _currentSource = new vector<component>;
    _voltageSource = new vector<component>;
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
        _lines.push_back(sLine);
    }
    clock_t mend = clock();
    cout << "time for reading: " << (mend - mstart) / (double) CLOCKS_PER_SEC << "s" << endl;
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
    cout << "total resistors: " << _resistors->size() << endl;
    cout << "total inductors: " << _inductors->size() << endl;
    cout << "total capacitors: " << _capacitors->size() << endl;
    cout << "total current source: " << _currentSource->size() << endl;
    // cout << "processed lines: " << _tst.size() << endl;
}

void Parser::parseLine(const string& line){
    if(line[0] == '.'){
        addDirective(line);
    }
    else{
        // TODO: build nodeMap
        if(line[0] == 'i'){
            addCurrentSource(line);
        }
        else{
            addPassive(line);
        }
    }
}

void Parser::addDirective(const string& line){
    vector<string> words;
    split(words, line);
    float start = stof(words[1]);
    float end = stof(words[2]);
    float delta = stof(words[3]);
    // unique_lock<mutex> lck(_mtx);
    _steps = (end - start) / delta + 1;
}

void Parser::addCurrentSource(const string& line){
    vector<string> words;
    split(words, line);
    if(words.size() == 4){
        // dc source
        addDcCurrent(words[0], words[1], words[2], words[3]);
    }
    else if(words.size() > 4){
        string name = words[0];
        string Np = words[1];
        string Nn = words[2];
        words.erase(words.begin(), words.begin() + 4);
        addPwlCurrent(name, Np, Nn, words);
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
        addResistor(words[0], words[1], words[2], words[3]);
    }
    else if(words[0][0] == 'l'){
        addInductor(words[0], words[1], words[2], words[3]);
    }
    else if(words[0][0] == 'c'){
        addCapacitor(words[0], words[1], words[2], words[3]);
    }
}
