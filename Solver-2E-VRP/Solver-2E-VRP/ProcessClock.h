#ifndef PROCESSCLOCK_H_
#define PROCESSCLOCK_H_

#ifdef _WIN32

#include <windows.h>

struct ProcessTime {
    FILETIME tms_utime;
    FILETIME tms_stime;
    FILETIME tms_cutime;
    FILETIME tms_cstime;
};

#else

#include <sys/times.h>

typedef struct tms ProcessTime;

#endif


class ProcessClock {
    ProcessTime startTime;
    ProcessTime endTime;
    double userTime;
    double sysTime;
    double createTime;
    double exitTime;
    double cpuTime;

#ifndef _WIN32
    clock_t startClock;
    clock_t endClock;
#endif

public:
    void start();

    void end();

    double getUserTime() { return userTime; }

    double getSysTime() { return sysTime; }

    double getCreateTime() { return createTime; }

    double getExitTime() { return exitTime; }

    double getCpuTime() { return cpuTime; }
};

#endif /* PROCESSCLOCK_H_ */
