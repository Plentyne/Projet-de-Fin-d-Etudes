//
// Created by Youcef on 02/02/2016.
//

#include "Config.h"
#include "Utility.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

const double Config::DOUBLE_INFINITY = 999999999;

string Config::_inputDir = "";
string Config::_outputDir = "";
string Config::_resumeFile = "";
unsigned int Config::_numExec = 3; // number of execution per instance

list<Instance> Config::_fileList;

unsigned int Config::_idchSize = 5;


Config::EvaluationMethod Config::_evaluationMethod = Config::SaturatedTourEvaluation; // evaluation method (QuickEvaluation does not work properly now)

unsigned int Config::_stopCondition = 10000; // stopping condition

// idch parameters (for initialization of metaheuristics)
Config::InsertionCriteria Config::_idchInsertionCriteria = Config::DefaultInsertion; // insertion criteria for IDCH
unsigned int Config::_idchDestructionStep = 3; // small destruction step for IDCH
double Config::_alpha = 1;;// profit idch weight

// local search parameters
Config::LocalSearchRateFormula Config::_lsRateFormula = Config::IntenseFirstRate; // formula for local search rate
double Config::_lsRateBending = 1.0; // bending measure, value k in 1-x^k if IntenseFirstRate is used (or 1-(1-x)^k if IntenseLastRate is used)
double Config::_lsRateValue = 1.0; // local search rate if ConstantRate is used

unsigned int Config::_lsSwapUse = 1; // maximal use of swap neighborhood (set to 0 to turn it off)
unsigned int Config::_lsShiftUse = 1; // maximal use of shift neighborhood (set to 0 to turn it off)
unsigned int Config::_lsLargeDestructionRepairUse = 1; // maximal use of large destruction/repair neighborhood (set to 0 to turn it off)

// (new implementation)
unsigned int Config::_lsShuffledSwapUse = 0; // maximal use of swap neighborhood (set to 0 to turn it off)
unsigned int Config::_lsShuffledShiftUse = 0; // maximal use of shift neighborhood (set to 0 to turn it off)
unsigned int Config::_lsSmallDestructionRepairUse = 0; // maximal use of small destruction/repair neighborhood (set to 0 to turn it off)


unsigned int Config::_lsShuffledShiftUseOptimalEval = 0; // Evaluation using interval model
unsigned int Config::_lsShuffledSwapUseOptimalEval = 0; // Evaluation using interval model

// constants
unsigned int Config::_infUnsignedInt = (unsigned int) 999999999;
double Config::_infDouble = (double) 999999999;
double Config::_espDouble = (double) 0.000001;

Instance::Instance() {
    this->_fileName = "";
    this->_exist_SOL = false;
    this->_SOL = Config::_infUnsignedInt;
}

/**************************
 *
 *     Functions
 *
 **************************/
bool Config::checkKey(const string &line, const string &key) {
    return (line.compare(0, min(key.size(), line.size()), key) == 0);
}

bool Config::getKeyValue(const string &line, const string &key, string &value, const char &sep) {
    if (!checkKey(line, key)) return false;
    if (line[min(key.size(), line.size())] != sep) return false;
    value = line.substr(min(key.size() + 1, line.size()), string::npos);

    return true;
}

bool Config::getFieldValue(const string &line, const unsigned int &field, string &value, const char &sep) {
    size_t i, pos1, pos2;

    pos1 = 0;
    for (i = 0; i < field; i++) pos1 = line.find(sep, pos1 + 1);
    if (pos1 == string::npos) return false;
    pos2 = line.find(sep, pos1 + 1);

    if (pos1 == 0) value = line.substr(pos1, pos2 - pos1);
    else value = line.substr(pos1 + 1, pos2 - pos1 - 1);

    return true;
}


void Config::readInstances(const string &fn) {
    ifstream fh;
    string line, value;

    try {
        cout << "loading instances from :  " << fn << " .. ";

        Config::_fileList.clear();

        fh.open(fn.c_str(), ifstream::in);
        if (!fh.good()) throw (string) "can not open file instance";

        while (getline(fh, line)) {
            if (line.compare("") != 0) {
                Instance tmp;

                if (Config::getFieldValue(line, 0, tmp._fileName)) {
                } else throw (string) "format error";


                if (Config::getFieldValue(line, 1, value)) {
                    if (value.compare("null") != 0) {
                        stringstream(value) >> tmp._SOL;
                        tmp._exist_SOL = true;
                    }
                } else throw (string) "format error";


                Config::_fileList.push_back(tmp);
            }
        }

        fh.close();

        cout << "ok" << endl;
    } catch (const string &emsg) {
        if (fh.is_open()) fh.close();
        Utility::exitOnError("Config::read()", emsg);
    }

}

void Config::read(const string &fn) {
    ifstream fh;
    string line, value;

    try {
        cout << "load config from " << fn << " .. ";

        fh.open(fn.c_str(), ifstream::in);
        if (!fh.good()) throw (string) "can not open file";

        while (getline(fh, line) && !checkKey(line, (string) "EOF")) {
            // common
            if (Config::getKeyValue(line, (string) "inputdir", Config::_inputDir)) {
            } else if (Config::getKeyValue(line, (string) "outputdir", Config::_outputDir)) {
            } else if (Config::getKeyValue(line, (string) "resumefile", Config::_resumeFile)) {
            } else if (Config::getKeyValue(line, (string) "idchsize", value)) {
                stringstream(value) >> Config::_idchSize;
            } else if (Config::getKeyValue(line, (string) "numberexec", value)) {
                stringstream(value) >> Config::_numExec;
            } else if (Config::getKeyValue(line, (string) "stopcondition", value)) {
                stringstream(value) >> Config::_stopCondition;

                // idch parametre
            } else if (Config::getKeyValue(line, (string) "idchprofitweight", value)) {
                stringstream(value) >> Config::_alpha;
                // evaluation
            } else if (Config::getKeyValue(line, (string) "evalmethod", value)) {
                if (value.compare("saturated") == 0) Config::_evaluationMethod = Config::SaturatedTourEvaluation;
                else if (value.compare("unsaturated") == 0)
                    Config::_evaluationMethod = Config::UnsaturatedTourEvaluation;
                else if (value.compare("interval") == 0) Config::_evaluationMethod = Config::IntervalModelEvaluation;
                else throw (string) "unknown data type";

                // local search
            } else if (Config::getKeyValue(line, (string) "maxshift", value)) {
                stringstream(value) >> Config::_lsShiftUse;
            } else if (Config::getKeyValue(line, (string) "maxswap", value)) {
                stringstream(value) >> Config::_lsSwapUse;
            } else if (Config::getKeyValue(line, (string) "maxlargedr", value)) {
                stringstream(value) >> Config::_lsLargeDestructionRepairUse;
            } else if (Config::getKeyValue(line, (string) "maxshuffledswap", value)) {
                stringstream(value) >> Config::_lsShuffledSwapUse;
            } else if (Config::getKeyValue(line, (string) "maxshuffledshift", value)) {
                stringstream(value) >> Config::_lsShuffledShiftUse;
            } else if (Config::getKeyValue(line, (string) "maxsmalldr", value)) {
                stringstream(value) >> Config::_lsSmallDestructionRepairUse;
            } else if (Config::getKeyValue(line, (string) "maxshuffledshiftOptEval", value)) {
                stringstream(value) >> Config::_lsShuffledShiftUseOptimalEval;
            } else if (Config::getKeyValue(line, (string) "maxshuffledswapOptEval", value)) {
                stringstream(value) >> Config::_lsShuffledSwapUseOptimalEval;

                // error
            } else {
                throw (string) "unknown parameter";
            }
        }

        fh.close();

        cout << "ok" << endl;
    } catch (const string &emsg) {
        if (fh.is_open()) fh.close();
        Utility::exitOnError("Config::read()", emsg);
    }
}

// TEST & DEBUG
void Config::test() {
    string value;

//	cout<<Config::checkKey("Test;12", "Test")<<endl;
//	cout<<Config::getKeyValue("Test;12", "Test",value)<<" - ["<<value<<"]"<<endl;
//	cout<<Config::getFieldValue("Test;12;13;14", 2, value)<<" - ["<<value<<"]"<<endl;

    Config::read("config.txt");
    cout << "input dir = " << Config::_inputDir << endl;
    cout << "output dir = " << Config::_outputDir << endl;
    cout << "resume file = " << Config::_resumeFile << endl;
    cout << "idch size = " << Config::_idchSize << endl;
    cout << "number of execution = " << Config::_numExec << endl;
    cout << "evaluation method = " << Config::_evaluationMethod << endl;
    cout << "shifting neighborhood = " << Config::_lsShiftUse << endl;
    cout << "swapping neighborhood = " << Config::_lsSwapUse << endl;
    cout << "large destroy-repair neighborhood = " << Config::_lsLargeDestructionRepairUse << endl;
    cout << "small destroy-repair neighborhood = " << Config::_lsSmallDestructionRepairUse << endl;

    list<Instance>::iterator it;
    for (it = Config::_fileList.begin(); it != Config::_fileList.end(); it++) {
        cout << it->_fileName << " - " << it->_exist_SOL << endl;
    }
}
