// Solver-2E-VRP.cpp : Defines the entry point for the console application.
//

// Librairies standard
#include<iostream>
#include <fstream>
#include <string>
#include <sstream>
// Fichiers perso
#include "./Model/Problem.h"


using namespace std;

int main()
{
    Problem p;
    p.readBreunigFile("../Input/Data/Set4a_Instance50-37.dat");
    cout << "Dimension : " << p.getDimension() << endl;
    cout << "Distance : " << p.getDistance(p.getSatellite(0), p.getDepot()) << endl;
    cout << "Distance : " << p.getDistance(p.getClient(0), p.getSatellite(0)) << endl;

    return 0;
}

