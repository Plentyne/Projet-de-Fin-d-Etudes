//
// Created by Youcef on 02/02/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_CONFIG_H
#define PROJET_DE_FIN_D_ETUDES_CONFIG_H

#include <string>
#include <list>

/* Config File Sample
 *
 * inputdir;../Data/Chao            // Directory containig test data
 * outputdir;./Output               // Directory containig test results
 * resumefile;resume.csv            // File containing test results
 * idchsize;5
 * stopcondition;40                 // Stop confition for the meta-heuristique
 * numberexec;3                     // Number of executions for each instance
 * evalmethod;interval
 * idchprofitweight;1
 * maxshift;0
 * maxswap;0
 * maxlargedr;1
 * maxshuffledshift;1
 * maxshuffledswap;1
 * maxsmalldr;0
 * maxshuffledshiftOptEval;0
 * maxshuffledswapOptEval;0
 * EOF                              // Marks the end of file
 * */




using namespace std;

struct Instance {
    string _fileName;
    bool _exist_SOL;
    unsigned int _SOL;

    Instance();
};

class Config {
public:
    // enum type
    enum InsertionCriteria {
        DefaultInsertion, NichingInsertion
    };
    enum EvaluationMethod {
        IntervalModelEvaluation, SaturatedTourEvaluation, UnsaturatedTourEvaluation, QuickEvaluation
    };
    enum CrossoverMethod {
        LOXCrossover, OXCrossover, COXCrossover
    };
    enum LocalSearchRateFormula {
        ConstantRate, IntenseFirstRate, IntenseLastRate
    };

    // external parameters
    static string _inputDir;
    static string _outputDir;
    static string _resumeFile;
    static unsigned int _numExec;

    static list<Instance> _fileList;

    // common parameters
    static unsigned int _idchSize;

    static EvaluationMethod _evaluationMethod;

    static unsigned int _stopCondition;

    // idch parameters (for initialization of metaheuristics)
    static InsertionCriteria _idchInsertionCriteria;
    static unsigned int _idchDestructionStep;
    static double _alpha;

    // Destroy operators parametres
    static double p1; // Proportion of customers to be removed by RandomRemoval
    static double p2; // Proportion of customers to be removed by WorstRemoval
    static double p3; // Proportion of customers to be removed by RelatedRemoval
    static int p4; // Maximum number of routes to be removed by RouteRemoval
    static double p5; // Probability for using SingleNodeRouteRemoval
    static double p6; // Probability for using Satellite Removal
    static double p7; // Probability for using Open Satellite
    static double p8; // Probability for using Open All Satellites

    // local search parameters
    static LocalSearchRateFormula _lsRateFormula;
    static double _lsRateValue;
    static double _lsRateBending;

    static unsigned int _lsSwapUse;
    static unsigned int _lsShiftUse;
    static unsigned int _lsLargeDestructionRepairUse;

    static unsigned int _lsShuffledSwapUse;
    static unsigned int _lsShuffledShiftUse;
    static unsigned int _lsSmallDestructionRepairUse;

    static unsigned int _lsShuffledShiftUseOptimalEval;
    static unsigned int _lsShuffledSwapUseOptimalEval;

    // constants
    static const double DOUBLE_INFINITY;
    static unsigned int _infUnsignedInt;
    static double _infDouble;
    static double _espDouble;

    // functions
    static bool checkKey(const string &line, const string &key);

    static bool getKeyValue(const string &line, const string &key, string &value, const char &sep = ';');

    static bool getFieldValue(const string &line, const unsigned int &field, string &value, const char &sep = ';');

    static void readInstances(const string &fn);

    static void readSingleInstance(const string &s);

    static void read(const string &fn);

    // TEST & DEBUG
    static void test();

};


#endif //PROJET_DE_FIN_D_ETUDES_CONFIG_H
