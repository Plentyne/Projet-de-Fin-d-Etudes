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
#include "Utility.h"

using namespace std;

void buildTestSolution(Solution &s, Problem *p);


int main()
{
    Problem p;
    //p.readBreunigFile("../Input/Data/Set2b_E-n51-k5-s32-37.dat");
    //p.readBreunigFile("../Input/Data/Test2.dat");
    p.readBreunigFile("../Input/Data/Set5_100-5-1.dat");
    //p.readBreunigFile("../Input/Data/Set5_200-10-1.dat");
    Solution s(&p);
    cout << "s = " << s.getProblem()->getClients().size() << endl;
    //buildTestSolution(s, &p);
    //s.saveHumanReadable("Test.sol", "Solution Test2.dat", false);
    //Heuristic::simpleHeuristic(p, s);
    Insertion insert(&p);
    insert.GreedyInsertionHeuristic(s);
    //MoleJamesonHeuristic solver(&p, 1, 1);
    //solver.solve(s);
    s.print();

    //s.saveHumanReadable("Test.sol","Solution Set5_100-5_1 avec 2opt",false);
    //char x ;
    //cin >> x;
    /* vector<int> v{1,2,3,4,5,6,7,8,9,10};
     vector<int> v1 = v;
     v1.erase(v1.begin()+1,v1.begin()+1+2);
     cout << "v1 : ";
     for(int i = 0; i< v1.size() ; i++) cout << v1[i] << " ";
     cout << endl;
     vector<int> v2(v.begin()+1,v.begin()+1+2);
     cout << "v2 : ";
     for(int i = 0; i< v2.size() ; i++) cout << v2[i] << " ";
     cout << endl;
     v1.insert(v1.begin()+3, v2.begin(),v2.end());
     cout << "insertion v1 : ";
     for(int i = 0; i< v1.size() ; i++) cout << v1[i] << " ";
     cout << endl; */
    cout << "Validity : " << p.isValidSolution(s) << endl;

    return 0;
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