// Solver-2E-VRP.cpp : Defines the entry point for the console application.
//

// Librairies standard
#include<iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
// Fichiers perso
#include "./Model/Problem.h"
#include "./Model/Solution.h"
#include "./Solver/Heuristic.h"
#include "./Solver/Insertion.h"
#include "./Solver/MoleJamesonHeuristic.h"
#include "./Solver/IDCH.h"
#include "Utility.h"
#include "Config.h"
#include "ProcessClock.h"
#include "./Solver/Sequence.h"
#include "Statistic.h"
#include <algorithm>

using namespace std;

void buildTestSolution(Solution &s, Problem *p);

void testIDCH(const int &exec);
void testFastIDCH(const int &exec);
void testBestInsertion(const int &exec);
void testGreedyInsertion(const int &exec);

int main(int argc, const char *argv[])
{
    if (argc < 3) {
        cout << "Messing arguments !!" << endl;
        cout << "Command form :  " << argv[0] << " CONFIG_FILE -f INSTANCES_FILE" << endl;
        cout << "             or " << argv[0] << " CONFIG_FILE -i INSTANCE_FILE" << endl;
    }
    else {
        Config::read(string(argv[1]));// Parameters
        string option(argv[2]);
        if (option == "-f") {
            Config::readInstances(string(argv[3])); // instance set to read
        }
        else if (option == "-i") {
            Config::readSingleInstance(argv[3]);
        }
        else {
            cout << "Wrong argument" << endl;
            return 1;
        }

        //testBestInsertion(5);
        testIDCH(5);
        //testFastIDCH(5);
    }
    /*********
     *  Fonctions de Test
     * Config::read("config.txt");// Parameters
     * Config::_resumeFile = "resume.csv";
     * Config::readInstances("./Input/instances.txt"); // instance set to read
     * testFastIDCH(5);
     */
    //Config::test();

    //Config::read("config.txt");// Parameters
    //Config::readInstances("./Input/instances.txt"); // instance set to read
    //testIDCH(5);


    return 0;
}

void testIDCH(const int &exec) {
    ofstream fh;

    fh.open((Config::_outputDir + "/" + Config::_resumeFile).c_str());

    fh << "instance;ltime;";
    fh << ";maxscore;minscore;avgscore;";
    fh << ";maxcpu;mincpu;avgcpu;";
    fh << ";nshift;nswap;nldr;nsdr";
    fh << endl;


    for (list<Instance>::iterator iteri = Config::_fileList.begin();
         iteri != Config::_fileList.end();
         iteri++) {
        //-----------------
        ofstream fd;
        fd.open((Config::_outputDir + "/" + iteri->_fileName + "_dmax_exec.csv" ).c_str());
        fd << "instance;" + iteri->_fileName << endl;
        //----------
        ProcessClock pclock;

        fh << iteri->_fileName << ";";
        cout << "Loading file : " << iteri->_fileName << " .. " << endl;
        Problem problem;

        pclock.start();
        problem.readBreunigFile(Config::_inputDir + "/" + iteri->_fileName);
        pclock.end();

        cout << "Ok" << endl;
        cout << "loading time = " << pclock.getCpuTime() << endl;
        fh << pclock.getCpuTime() << ";";

        vector<double> score;
        vector<double> cpu;
        Statistic::reset();

        for (unsigned int i = 0; i < Config::_numExec; i++) {
            cout << "execution = " << i << endl;
            //srand(rseed[i]);
            Utility::initializeRandomGenerators();

            //----------------
            Statistic::dmaxs.clear();
            //----------------

            pclock.start();
            IDCH solver(&problem);
            Solution sol(&problem);
            solver.heuristicIDCH(sol);
            pclock.end();

            //sol.recomputeCost();
            cout << "\tscore = " << sol.getTotalCost() << endl;
            cout << "\tresolution time = " << pclock.getCpuTime() << endl;
            score.push_back(sol.getTotalCost());
            cpu.push_back(pclock.getCpuTime());

            cout << "\tchecking solution .. ";
            if (problem.isValidSolution(sol)) cout << "Ok";
            else cout << "failed";
            cout << endl;

            cout << "\tSaving solution to " << Config::_outputDir + "/" + iteri->_fileName + ".sol" << " .. ";
            stringstream header;
            header << "execution = " << i << endl;
            //header<<"seed = "<<rseed[i]<<endl;
            //header<<"cpu = "<<pclock.getCpuTime()<<endl;
            sol.saveHumanReadable(Config::_outputDir + "/" + iteri->_fileName + ".sol", header.str(), false);
            cout << "Ok" << endl;

            // Todo enlever
            fd << "execution " << i << ";" ;
            for (int j = 0; j < Statistic::dmaxs.size(); ++j) {
                fd << Statistic::dmaxs[j] << ";";
            }
            fd << endl;


        }

        fd.close();

        fh << ";";
        double maxscore, minscore;
        double avgscore,
                maxcpu, mincpu, avgcpu;

        Statistic::calMaxMinAvg(score, maxscore, minscore, avgscore);
        Statistic::calMaxMinAvg(cpu, maxcpu, mincpu, avgcpu);

        fh << maxscore << ";" << minscore << ";" << avgscore << ";;";
        fh << maxcpu << ";" << mincpu << ";" << avgcpu << ";;";

        fh << Statistic::_nshift << ";";
        fh << Statistic::_nswap << ";";
        fh << Statistic::_nldr << ";";
        fh << Statistic::_nsdr;

        fh << endl;
    }

    fh.close();
}

void testFastIDCH(const int &exec) {
    ofstream fh;

    fh.open((Config::_outputDir + "/" + Config::_resumeFile).c_str());

    fh << "instance;ltime;";
    fh << ";maxscore;minscore;avgscore;";
    fh << ";maxcpu;mincpu;avgcpu;";
    fh << ";nshift;nswap;nldr;nsdr";
    fh << endl;


    Utility::initializeRandomGenerators();

    for (list<Instance>::iterator iteri = Config::_fileList.begin();
         iteri != Config::_fileList.end();
         iteri++) {
        ProcessClock pclock;

        fh << iteri->_fileName << ";";
        cout << "Loading file : " << iteri->_fileName << " .. " << endl;
        Problem problem;

        pclock.start();
        problem.readBreunigFile(Config::_inputDir + "/" + iteri->_fileName);
        pclock.end();

        cout << "Ok" << endl;
        cout << "loading time = " << pclock.getCpuTime() << endl;
        fh << pclock.getCpuTime() << ";";

        vector<double> score;
        vector<double> cpu;
        Statistic::reset();

        /*if (Config::_usingBound) {
            if(iteri->_exist_UB) problem.setUB(iteri->_UB);
            if(iteri->_exist_SOL) problem.setSOL(iteri->_SOL);
        }*/

        /*vector<unsigned int> rseed;
        generateRandomSeed(rseed, Config::_numExec);*/
        for (unsigned int i = 0; i < Config::_numExec; i++) {
            cout << "execution = " << i << endl;
            //srand(rseed[i]);

            pclock.start();
            IDCH solver(&problem);
            Solution sol(&problem);
            solver.heuristicFastIDCH(sol);
            sol.recomputeCost();
            pclock.end();

            cout << "\tscore = " << sol.getTotalCost() << endl;
            cout << "\tresolution time = " << pclock.getCpuTime() << endl;
            score.push_back(sol.getTotalCost());
            cpu.push_back(pclock.getCpuTime());

            cout << "\tchecking solution .. ";
            if (problem.isValidSolution(sol)) cout << "Ok";
            else cout << "failed";
            cout << endl;

            cout << "\tSaving solution to " << Config::_outputDir + "/" + iteri->_fileName + ".sol" << " .. ";
            stringstream header;
            header << "execution = " << i << endl;
            //header<<"seed = "<<rseed[i]<<endl;
            //header<<"cpu = "<<pclock.getCpuTime()<<endl;
            sol.saveHumanReadable(Config::_outputDir + "/" + iteri->_fileName + ".sol", header.str(), false);
            cout << "Ok" << endl;

        }

        fh << ";";
        double maxscore, minscore;
        double avgscore,
                maxcpu, mincpu, avgcpu;

        Statistic::calMaxMinAvg(score, maxscore, minscore, avgscore);
        Statistic::calMaxMinAvg(cpu, maxcpu, mincpu, avgcpu);

        fh << maxscore << ";" << minscore << ";" << avgscore << ";;";
        fh << maxcpu << ";" << mincpu << ";" << avgcpu << ";;";

        fh << Statistic::_nshift << ";";
        fh << Statistic::_nswap << ";";
        fh << Statistic::_nldr << ";";
        fh << Statistic::_nsdr;

        fh << endl;

    }

    fh.close();
}

void testGreedyInsertion(const int &exec) {
    ofstream fh;

    fh.open((Config::_outputDir + "/" + Config::_resumeFile).c_str());

    fh << "instance;ltime;";
    fh << ";maxscore;minscore;avgscore;";
    fh << ";maxcpu;mincpu;avgcpu;";
    fh << ";nshift;nswap;nldr;nsdr";
    fh << endl;


    Utility::initializeRandomGenerators();

    for (list<Instance>::iterator iteri = Config::_fileList.begin();
         iteri != Config::_fileList.end();
         iteri++) {
        ProcessClock pclock;

        fh << iteri->_fileName << ";";
        cout << "Loading file : " << iteri->_fileName << " .. " << endl;
        Problem problem;

        pclock.start();
        problem.readBreunigFile(Config::_inputDir + "/" + iteri->_fileName);
        pclock.end();

        cout << "Ok" << endl;
        cout << "loading time = " << pclock.getCpuTime() << endl;
        fh << pclock.getCpuTime() << ";";

        vector<double> score;
        vector<double> cpu;
        Statistic::reset();

        /*if (Config::_usingBound) {
            if(iteri->_exist_UB) problem.setUB(iteri->_UB);
            if(iteri->_exist_SOL) problem.setSOL(iteri->_SOL);
        }*/

        /*vector<unsigned int> rseed;
        generateRandomSeed(rseed, Config::_numExec);*/
        for (unsigned int i = 0; i < Config::_numExec; i++) {
            cout << "execution = " << i << endl;
            //srand(rseed[i]);

            pclock.start();
            Insertion insert(&problem);
            Solution sol(&problem);
            insert.GreedyInsertionHeuristic(sol);
            sol.recomputeCost();
            pclock.end();

            cout << "\tscore = " << sol.getTotalCost() << endl;
            cout << "\tresolution time = " << pclock.getCpuTime() << endl;
            score.push_back(sol.getTotalCost());
            cpu.push_back(pclock.getCpuTime());

            cout << "\tchecking solution .. ";
            if (problem.isValidSolution(sol)) cout << "Ok";
            else cout << "failed";
            cout << endl;

            cout << "\tSaving solution to " << Config::_outputDir + "/" + iteri->_fileName + ".sol" << " .. ";
            stringstream header;
            header << "execution = " << i << endl;
            //header<<"seed = "<<rseed[i]<<endl;
            //header<<"cpu = "<<pclock.getCpuTime()<<endl;
            sol.saveHumanReadable(Config::_outputDir + "/" + iteri->_fileName + ".sol", header.str(), false);
            cout << "Ok" << endl;
        }

        fh << ";";
        double maxscore, minscore;
        double avgscore,
                maxcpu, mincpu, avgcpu;

        Statistic::calMaxMinAvg(score, maxscore, minscore, avgscore);
        Statistic::calMaxMinAvg(cpu, maxcpu, mincpu, avgcpu);

        fh<<maxscore<<";"<<minscore<<";"<<avgscore<<";;";
        fh<<maxcpu<<";"<<mincpu<<";"<<avgcpu<<";;";

        fh<<Statistic::_nshift<<";";
        fh<<Statistic::_nswap<<";";
        fh<<Statistic::_nldr<<";";
        fh<<Statistic::_nsdr;

        fh << endl;
    }

    fh.close();
}


void testBestInsertion(const int &exec) {
    ofstream fh;

    fh.open((Config::_outputDir + "/" + Config::_resumeFile).c_str());

    fh << "instance;ltime;";
    fh << ";maxscore;minscore;avgscore;";
    fh << ";maxcpu;mincpu;avgcpu;";
    fh << ";nshift;nswap;nldr;nsdr";
    fh << endl;


    Utility::initializeRandomGenerators();

    for (list<Instance>::iterator iteri = Config::_fileList.begin();
         iteri != Config::_fileList.end();
         iteri++) {
        ProcessClock pclock;

        fh << iteri->_fileName << ";";
        cout << "Loading file : " << iteri->_fileName << " .. " << endl;
        Problem problem;

        pclock.start();
        problem.readBreunigFile(Config::_inputDir + "/" + iteri->_fileName);
        pclock.end();

        cout << "Ok" << endl;
        cout << "loading time = " << pclock.getCpuTime() << endl;
        fh << pclock.getCpuTime() << ";";

        vector<double> score;
        vector<double> cpu;
        Statistic::reset();

        /*if (Config::_usingBound) {
            if(iteri->_exist_UB) problem.setUB(iteri->_UB);
            if(iteri->_exist_SOL) problem.setSOL(iteri->_SOL);
        }*/

        /*vector<unsigned int> rseed;
        generateRandomSeed(rseed, Config::_numExec);*/
        for (unsigned int i = 0; i < Config::_numExec; i++) {
            cout << "execution = " << i << endl;
            //srand(rseed[i]);

            pclock.start();
            Insertion insert(&problem);
            Solution sol(&problem);
            insert.solveBestInsertion(sol);
            sol.recomputeCost();
            pclock.end();

            cout << "\tscore = " << sol.getTotalCost() << endl;
            cout << "\tresolution time = " << pclock.getCpuTime() << endl;
            score.push_back(sol.getTotalCost());
            cpu.push_back(pclock.getCpuTime());

            cout << "\tchecking solution .. ";
            if (problem.isValidSolution(sol)) cout << "Ok";
            else cout << "failed";
            cout << endl;

            cout << "\tSaving solution to " << Config::_outputDir + "/" + iteri->_fileName + ".sol" << " .. ";
            stringstream header;
            header << "execution = " << i << endl;
            //header<<"seed = "<<rseed[i]<<endl;
            //header<<"cpu = "<<pclock.getCpuTime()<<endl;
            sol.saveHumanReadable(Config::_outputDir + "/" + iteri->_fileName + ".sol", header.str(), false);
            cout << "Ok" << endl;
        }

        fh << ";";
        double maxscore, minscore;
        double avgscore,
                maxcpu, mincpu, avgcpu;

        Statistic::calMaxMinAvg(score, maxscore, minscore, avgscore);
        Statistic::calMaxMinAvg(cpu, maxcpu, mincpu, avgcpu);

        fh<<maxscore<<";"<<minscore<<";"<<avgscore<<";;";
        fh<<maxcpu<<";"<<mincpu<<";"<<avgcpu<<";;";

        fh<<Statistic::_nshift<<";";
        fh<<Statistic::_nswap<<";";
        fh<<Statistic::_nldr<<";";
        fh<<Statistic::_nsdr;

        fh << endl;
    }

    fh.close();
}


/*
void buildTestSolution(Solution &s, Problem *p) {

    s = Solution(p);
    s.setTotalCost(417.07);
    s.getDeliveredQ()[0] = 11000;
    s.getDeliveredQ()[1] = 11500;
    s.getSatelliteDemands()[0] = 11000;
    s.getSatelliteDemands()[1] = 11500;
    // Construction manuelle d'une solution : seulement pour tester
    E1Route e1;
    e1.cost = 62.032249450683594;
    e1.load = 11000;
    e1.departure = p->getDepot();
    e1.tour.push_back(p->getSatellite(0));
    s.getE1Routes().push_back(e1);
    e1.cost = 44.18144226074219;
    e1.load = 11500;
    e1.departure = p->getDepot();
    e1.tour.clear();
    e1.tour.push_back(p->getSatellite(1));
    s.getE1Routes().push_back(e1);

    E2Route e2;
    e2.cost = 73.21856832504272;
    e2.load = 5600;
    e2.departure = p->getSatellite(0); // 11,9,7,4,3,8
    e2.tour.push_back(p->getClient(8));
    e2.tour.push_back(p->getClient(6));
    e2.tour.push_back(p->getClient(4));
    e2.tour.push_back(p->getClient(1));
    e2.tour.push_back(p->getClient(0));
    e2.tour.push_back(p->getClient(5));
    s.getE2Routes().push_back(e2);
    e2.cost = 102.20066428184509;
    e2.load = 5400;
    e2.departure = p->getSatellite(0);
    e2.tour.clear();
    e2.tour.push_back(p->getClient(7)); // 10,12,15,13,6,5
    e2.tour.push_back(p->getClient(9));
    e2.tour.push_back(p->getClient(12));
    e2.tour.push_back(p->getClient(10));
    e2.tour.push_back(p->getClient(3));
    e2.tour.push_back(p->getClient(2));
    s.getE2Routes().push_back(e2);
    e2.cost = 58.23817253112793;
    e2.load = 6000;
    e2.departure = p->getSatellite(1); //21,23,22,19
    e2.tour.clear();
    e2.tour.push_back(p->getClient(18));
    e2.tour.push_back(p->getClient(20));
    e2.tour.push_back(p->getClient(19));
    e2.tour.push_back(p->getClient(16));
    s.getE2Routes().push_back(e2);
    e2.cost = 77.1982045173645;
    e2.load = 5500;
    e2.departure = p->getSatellite(1); // 18,16,14,17,20
    e2.tour.clear();
    e2.tour.push_back(p->getClient(15));
    e2.tour.push_back(p->getClient(13));
    e2.tour.push_back(p->getClient(11));
    e2.tour.push_back(p->getClient(14));
    e2.tour.push_back(p->getClient(17));
    s.getE2Routes().push_back(e2);
    // Fin construction de la solution
}
 */