//
// Created by Youcef on 04/02/2016.
//

#include "Utility.h"

// COSTANTS
const string Utility::DOUBLE_RX = ("([[:d:]]+)(\\.[[:d:]]*)?");
const string Utility::INTEGER_RX("([[:d:]]+)");
const string Utility::TRUCK_DATA_RX = INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX;
const string Utility::CITY_FREIGHTER_DATA_RX =
        INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX;
const string Utility::STORES_DATA_RX = INTEGER_RX + "," + INTEGER_RX;
const string Utility::CLIENTS_DATA_RX = INTEGER_RX + "," + INTEGER_RX + "," + INTEGER_RX;