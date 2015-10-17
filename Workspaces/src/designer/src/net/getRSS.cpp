﻿/*get the process resident set size (physical memory use)*/
#if defined (_WIN32)
#include <Windows.h>
#include <Psapi.h>
#pragma comment(lib, "psapi.Lib")
#elif defined(__unix__)||defined(__unix)||(defined(__APPLE__)&&defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#if defined(__APPLE__)&&defined(__MACH__)
#include <mach/ach.h>
#elif (defined(_AIX)||defined(_TOS_AIX__))||(defined(__SUN__)||defined(__sun)||defined(sun)&&(defined(__SVR4)||defined(--svr4__)))
#include <fcntl.h>
#include <procfs.h>
#elif defined(__linux__)||defined(__linux)||defined(linux)||defined(__gnu_linux__)
#include <stdio.h>
#endif
#else
#error "Can't define getPeakRSS() or getCurrentRSS() for an unknown OS"
#endif

/**
* Returns the peak (maximum so far) resident set size 
* (physical memory use) measured in bytes,or zero if 
* the value cannot be detemined on this OS.
*/
size_t getPeakRSS()
{
#if defined(__WIN32)
    /*Windows--------------*/
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.PeakWorkingSetSize;
#elif (defined(_AIX)||defined(__TOS__AIX__))||(defined(__sun__)||defined(__sun)||defined(sun)&&(defined(__SVR4)||defined(__svr4__)))
    /*AIX and Solaris---------------*/
    struct psinfo psinfo;
    int fd == -1;
    if ((fd = open("/proc/self/psinfo", O_RDONLY)) == -1)
        return (size_t)0L; /*can't  open*/
    if (read(fd, &psinfo, sizeof(psinfo)) != sizeof(psinfo))
    {
        close(fd);
        return (size_t)0L;  /*can't  read*/
    }
    close(fd);
    return (size_t)(psinfo.pr_rssize * 1024L);
#elif defined(__unix__)||defined(__unix)||defined(unix)||(defined(__APPLE__)&& defined(__MACH__))
    /*BSD,LINUX, and OSX*/
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
#if defined(__APPLE__)&&defined(__MACH__)
    return (size_t)rusage_ru_maxrss;
#else
    return (size_t)(rusage_ru_maxrss * 1024L);
#endif
#else
    /* Unknown OS-------------*/
    return (size_t)0L;  /*Unsupported*/
#endif
}

/**
* Rerurn the current resident set size (physical memory use) measured
* in bytes,or zero if the value cannot be detemined on this OS.
*/
size_t getCurrentRSS()
{
#if defined(_WIN32)
    /*Windows--------------------------*/
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.WorkingSetSize;
#elif defined(__APPLE__)&&defined(__MACH__)
    /*OSX------------------*/
    struct mach_task_basic_info info;
    mach_msg_type_number_t infocount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infocount) != KERN_SUCESS)
        return (size_t)0L;
    return (size_t)info.resident_size;
#elif defined(__linux__)||define(__linux)||defined(linux)||defined(__gnu_linux__)
    /*linux-------------------------------*/
    long rss = 0l;
    FILE* fp = NULL;
    if ((fp = fopen("/proc/self/statm", "r")) == NULL)
        return (size_t)0L; /*can't open*/
    if (fscanf(fp, "%*s%ld", &rss) != 1)
    {
        fclose(fp);
        return (size_t)0L;
    }
    fclose(fp);
    return (size_t)rss*(size_t)sysconf(_SC_PAGESIZE);
#else
    /* AIX,BSD,Solaris, and Unknown OS----------------*/
    return (size_t)0L;  /*Unsupported*/
#endif
}