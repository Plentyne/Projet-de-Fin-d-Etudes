//
// Created by Youcef on 04/02/2016.
//

#include "Utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <random>

// COSTANTS
const string Utility::DOUBLE_RX = ("([[:d:]]+)(\\.[[:d:]]*)?");
const string Utility::INTEGER_RX("([[:d:]]+)");
const string Utility::TRUCK_DATA_RX = INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX;
const string Utility::CITY_FREIGHTER_DATA_RX =
        INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX;
const string Utility::STORES_DATA_RX = INTEGER_RX + "," + INTEGER_RX;
const string Utility::CLIENTS_DATA_RX = INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX;

int Utility::randomInt(int min, int max) {
    /*std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> int_dist(min, max - 1); // distribution in range [1, 6]
    return int_dist(rng);*/
    srand(time(NULL)); // initialisation de rand
    return ((max - min > 0) ? (rand() % (max - min)) : 0) + min;
}
