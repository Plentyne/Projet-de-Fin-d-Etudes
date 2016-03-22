#include "Statistic.h"

unsigned int Statistic::_nshift = 0;
unsigned int Statistic::_nsshift = 0;
unsigned int Statistic::_nswap = 0;
unsigned int Statistic::_nsswap = 0;
unsigned int Statistic::_nldr = 0;
unsigned int Statistic::_nsdr = 0;

void Statistic::reset() {
    Statistic::_nshift = 0;
    Statistic::_nsshift = 0;
    Statistic::_nswap = 0;
    Statistic::_nsswap = 0;
    Statistic::_nldr = 0;
    Statistic::_nsdr = 0;
}

void Statistic::calMaxMinAvg(const vector<unsigned int> &tab,
                             unsigned int &vmax, unsigned int &vmin, double &vavg) {
    if (tab.size() == 0) return;

    vavg = vmax = vmin = tab[0];
    for (unsigned int i = 1; i < tab.size(); i++) {
        if (tab[i] > vmax) vmax = tab[i];
        else if (tab[i] < vmin) vmin = tab[i];

        vavg += tab[i];
    }

    vavg = (double) vavg / tab.size();
}

void  Statistic::calMaxMinAvg(const vector<double> &tab,
                              double &vmax, double &vmin, double &vavg) {
    if (tab.size() == 0) return;

    vavg = vmax = vmin = tab[0];
    for (unsigned int i = 1; i < tab.size(); i++) {
        if (tab[i] > vmax) vmax = tab[i];
        else if (tab[i] < vmin) vmin = tab[i];

        vavg += tab[i];
    }

    vavg = (double) vavg / tab.size();
}
