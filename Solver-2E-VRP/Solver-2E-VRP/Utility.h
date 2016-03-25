//
// Created by Youcef on 04/02/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_UTILITY_H
#define PROJET_DE_FIN_D_ETUDES_UTILITY_H

#include <string>
#include <random>

using namespace std;

class Utility {
private:
    static int intSeed;
    static double dblSeed;
    static std::ranlux48_base intRng;
    static std::ranlux48_base dblRng;

public:
    static const string DOUBLE_RX;
    static const string INTEGER_RX;
    static const string TRUCK_DATA_RX;
    static const string CITY_FREIGHTER_DATA_RX;
    static const string STORES_DATA_RX;
    static const string CLIENTS_DATA_RX;

    static void initializeRandomGenerators();

    static int randomInt(int min, int max);

    static double randomDouble(double min, double max);

    /** show error message and exit */
    static void exitOnError(const string &location, const string &message);
};


#endif //PROJET_DE_FIN_D_ETUDES_UTILITY_H
