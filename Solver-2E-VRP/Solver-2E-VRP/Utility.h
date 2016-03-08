//
// Created by Youcef on 04/02/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_UTILITY_H
#define PROJET_DE_FIN_D_ETUDES_UTILITY_H

#include <regex>
#include <string>

using namespace std;

class Utility {
public:
    static const string DOUBLE_RX;
    static const string INTEGER_RX;
    static const string TRUCK_DATA_RX;
    static const string CITY_FREIGHTER_DATA_RX;
    static const string STORES_DATA_RX;
    static const string CLIENTS_DATA_RX;

    static int randomInt(int min, int max);
};


#endif //PROJET_DE_FIN_D_ETUDES_UTILITY_H
