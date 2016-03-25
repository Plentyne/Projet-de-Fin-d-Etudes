//
// Created by Youcef on 04/02/2016.
//

#include "Utility.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Static attributes
int Utility::intSeed = 1;
double Utility::dblSeed = 1;
std::ranlux48_base Utility::intRng;
std::ranlux48_base Utility::dblRng;


// COSTANTS
const string Utility::DOUBLE_RX = ("([[:d:]]+)(\\.[[:d:]]*)?");
const string Utility::INTEGER_RX("([[:d:]]+)");
const string Utility::TRUCK_DATA_RX = INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX;
const string Utility::CITY_FREIGHTER_DATA_RX =
        INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX;
const string Utility::STORES_DATA_RX = INTEGER_RX + "," + INTEGER_RX;
const string Utility::CLIENTS_DATA_RX = INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX;


void Utility::initializeRandomGenerators() {
    srand(time(NULL));
    intSeed = rand();
    dblSeed = rand() / RAND_MAX;
    intRng.seed(intSeed);
    dblRng.seed(dblSeed);
}


int Utility::randomInt(int min, int max) {
    std::uniform_int_distribution<std::ranlux48_base::result_type> int_dist(min, max - 1);
    return int_dist(intRng);
    /*srand(time(NULL)); // initialisation de rand
    return ((max - min > 0) ? (rand() % (max - min)) : 0) + min;
     */
}


double Utility::randomDouble(double min, double max) {
    // Todo changer pour utiliser les genérateur de c++11
    std::uniform_real_distribution<> dist(min, max);
    return dist(dblRng);
    /*if (max - min < 0.0001) return min;
    else {
        srand(time(NULL)); // initialisation de rand
        return min + (rand() / (RAND_MAX / (max - min)));
    }*/
}

void Utility::exitOnError(const string &location, const string &message) {
    cerr << "Exception on " << location << ": " << message << endl;
    exit(-1);
}
