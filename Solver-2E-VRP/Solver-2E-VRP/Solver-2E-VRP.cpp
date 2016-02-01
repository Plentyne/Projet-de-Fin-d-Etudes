// Solver-2E-VRP.cpp : Defines the entry point for the console application.
//

// Librairies standard
#include<iostream>

// Fichiers perso
#include"./Model/Node.h"
#include "./Model/Client.h"


using namespace std;

int main()
{
	Node n{ 1. ,1. }, m{2.,2.};
    Client c {1. , 1., 2};
	cout << Node::distance(m,n) << endl;
    cout << (n==c) << endl;
    return 0;
}

