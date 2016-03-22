#ifndef STATISTIC_H_
#define STATISTIC_H_

#include <vector>

using namespace std;

/*
 * Statistic
 */
class Statistic {
public:
    static unsigned int _nshift;
    static unsigned int _nsshift;
    static unsigned int _nswap;
    static unsigned int _nsswap;
    static unsigned int _nldr;
    static unsigned int _nsdr;

    static void reset();

    // other functions
    static void calMaxMinAvg(const vector<unsigned int> &tab,
                             unsigned int &vmax, unsigned int &vmin, double &vavg);

    static void calMaxMinAvg(const vector<double> &tab,
                             double &max, double &vmin, double &vavg);
};

#endif /*STATISTIC_H_*/
