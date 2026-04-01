
#ifndef PING_H
#define PING_H

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

#define PACKAGE_SIZE 64
#define USLEEP_ONE_SEC 1000000



typedef struct s_param {
    int         icmp_seq;
    u_int8_t    ttl;
    uint32_t    port;

    double time;
    double totaltime;
    double minTime;
    double maxTime;
    double avg;
    double mdev;
}   t_param;

typedef struct s_packageIP {
    char *all;
    int  allSize;
    char *data;
    int  dataSize;
    char headerIp[20];
    char headerIcmp[8];
}   t_packageIP;

typedef struct s_ping {
    struct addrinfo *result;
    char            *addrName;
    char            DNS[INET6_ADDRSTRLEN];

    int         packageSize;
    t_packageIP package;
    
    t_param     param;

}   t_ping;

#endif