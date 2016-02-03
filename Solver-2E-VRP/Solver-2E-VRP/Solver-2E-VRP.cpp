// Solver-2E-VRP.cpp : Defines the entry point for the console application.
//

// Librairies standard
#include<iostream>
#include <fstream>
#include <string>
#include <sstream>
// Fichiers perso
#include "./Model/Problem.h"
#include "./Model/Solution.h"


using namespace std;

int main()
{
    /* Problem p;
    p.readBreunigFile("../Input/Data/Test.dat");
    cout << "Dimension : " << p.getDimension() << endl;
    cout << "Distance : " << p.getDistance(p.getSatellite(0), p.getDepot()) << endl;
    cout << "Distance : " << p.getDistance(p.getClient(0), p.getSatellite(0)) << endl;
    */
    Problem p;
    p.readBreunigFile("../Input/Data/Test2.dat");
    cout << "Satellites : " << p.getSatellites().size() << endl;
    cout << "Clients : " << p.getClients().size() << endl;
    cout << "Dimension : " << p.getDimension() << endl;
    //Solution s(p);


    //s.print();


    return 0;
}

