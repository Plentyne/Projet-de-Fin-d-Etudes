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

using namespace std;

void buildTestSolution(Solution &s, Problem *p);

void testFastIDCH(const int &exec);

int main()
{

    //Config::test();
    Config::read("config.txt");// Parameters

    Config::_resumeFile = "resume.csv";
    Config::readInstances("../Input/Data/instances.txt"); // instance set to read
    testFastIDCH(5);
    //Problem p;
    //p.readBreunigFile("../Input/Data/Set2b_E-n51-k5-s32-37.dat");
    //p.readBreunigFile("../Input/Data/Test2.dat");
    //p.readBreunigFile("../Input/Data/Set5_100-5-1.dat");
    //p.readBreunigFile("../Input/Data/Set5_200-10-1.dat");
    //Solution s(&p);
    //cout << "s = " << s.getProblem()->getClients().size() << endl;
    //buildTestSolution(s, &p);
    //s.saveHumanReadable("Test.sol", "Solution Test2.dat", false);
    //Heuristic::simpleHeuristic(p, s);
    //Insertion insert(&p);
    //insert.GreedyInsertionHeuristic(s);
    //insert.GreedyInsertionNoiseHeuristic(s);
    //MoleJamesonHeuristic solver(&p, 1, 1);
    //solver.solve(s);
    //IDCH idchSolver(&p);
    //idchSolver.heuristicIDCH(s);
    //idchSolver.heuristicFastIDCH(s);
    /*s.print();
    Sequence seq(s);
    seq.extractSolution(s);
    s.print();
    for (int i = 0; i < s.getE2Routes().size() ; ++i) {
        E2Route e2route = s.getE2Routes()[i];
        e2route.cost =p.getDistance(p.getSatellite(e2route.departureSatellite),
                                                 p.getClient(e2route.tour[0]))
                       +p.getDistance(p.getSatellite(e2route.departureSatellite),
                                                   p.getClient(
                                                            e2route.tour[e2route.tour.size() - 1]));
        for (int k = 0; k < e2route.tour.size() - 1; ++k) {
            e2route.cost += p.getDistance(p.getClient(e2route.tour[k]),
                                                       p.getClient(e2route.tour[k + 1]));
        }
        cout << "Route " << i << " cost : " << e2route.cost <<endl;
    }
    cout << "Validity : " << p.isValidSolution(s) << endl;*/
    //s.saveHumanReadable("Test.sol","Solution Set5_100-5_1 avec 2opt",false);
    //char x ;
    //cin >> x;
    /* vector<int> u{1,2,3,4,5,6};
    vector<int> v{7,8,9,10,11,12};
    int p = 2;
    int q = 3;
    vector<int> u1(u.begin(),u.begin()+p+1);
    u1.insert(u1.end(),v.begin()+q+1,v.end());

     vector<int> v1(v.begin(),v.begin() + q + 1);
     v1.insert(v1.end(),u.begin()+p+1,u.end());


     cout << "v1 : ";
     for(int i = 0; i< v1.size() ; i++) cout << v1[i] << " ";
     cout << endl;
     cout << "u1 : ";
     for(int i = 0; i< u1.size() ; i++) cout << u1[i] << " ";
     cout << endl;
     v1.insert(v1.begin()+3, u1.begin(),u1.end());
     cout << "insertion v1 : ";
     for(int i = 0; i< v1.size() ; i++) cout << v1[i] << " ";
     cout << endl;*/

    return 0;
}
// TODO Rajouter le calcul des temps de résolution
// TODO Rajouter le calcul des statistiques et le remplissage du fichier résumé.csv
// TODO remettre la fonction FastIDCH

void testFastIDCH(const int &exec) {
    ofstream fh;

    fh.open((Config::_outputDir + "/" + Config::_resumeFile).c_str());

    fh << "instance;ltime;";
    fh << ";maxscore;minscore;avgscore;";
    fh << ";maxcpu;mincpu;avgcpu;";
    fh << ";nshift;nswap;nldr;nsdr";
    fh << endl;

    vector<unsigned int> score;
    vector<double> cpu;

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
        
        vector<unsigned int> score;
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
            //MoleJamesonHeuristic solver(&problem, 1, 1);
            Solution sol(&problem);
            solver.heuristicFastIDCH(sol);
            //solver.solve(sol);
            //solver.DPSOScheme(sol, 3600);  1h test
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
        unsigned int maxscore, minscore;
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