#include "ProcessClock.h"

#ifdef _WIN32

union b64 {
    FILETIME time;
    __int64 i64;
};

void ProcessClock::start() {
    GetProcessTimes(GetCurrentProcess(), &startTime.tms_cutime, &startTime.tms_cstime, &startTime.tms_stime,
                    &startTime.tms_utime);
}

void ProcessClock::end() {
    GetProcessTimes(GetCurrentProcess(), &endTime.tms_cutime, &endTime.tms_cstime, &endTime.tms_stime,
                    &endTime.tms_utime);

    b64 start, end;
    start.time = startTime.tms_utime;
    end.time = endTime.tms_utime;
    userTime = (double) (end.i64 - start.i64) / 10000000U;
    start.time = startTime.tms_stime;
    end.time = endTime.tms_stime;
    sysTime = (double) (end.i64 - start.i64) / 10000000U;
    start.time = startTime.tms_cutime;
    end.time = endTime.tms_cutime;
    createTime = (double) (end.i64 - start.i64) / 10000000U;
    start.time = startTime.tms_cstime;
    end.time = endTime.tms_cstime;
    exitTime = (double) (end.i64 - start.i64) / 10000000U;
    cpuTime = userTime + sysTime;
}

#else

#include <unistd.h>
#include <time.h>
#include <iostream>

void ProcessClock::start() {
    startClock = times(&startTime);
}
void ProcessClock::end() {
    endClock = times(&endTime);

    long tick_per_sec = sysconf(_SC_CLK_TCK);
    userTime = (double)(endTime.tms_utime - startTime.tms_utime)/tick_per_sec;
    sysTime = (double)(endTime.tms_stime - startTime.tms_stime)/tick_per_sec;
    createTime = (double)(endTime.tms_cutime - startTime.tms_cutime)/tick_per_sec;
    exitTime = (double)(endTime.tms_cstime - startTime.tms_cstime)/tick_per_sec;
    cpuTime = userTime + sysTime;
//	realTime = (double)(endClock - startClock)/tick_per_sec; // TODO: implement real elapsed time
}

#endif
