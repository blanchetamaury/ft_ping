#include <ping.h>

int g_verif = 1;

void    print_loop(t_ping *p) {
    printf("%d bytes from %s: ", p->packageSize, p->DNS);
    printf("icmp_seq=%d ", p->param.icmp_seq);
    printf("ttl=%d ", p->param.ttl);
    printf("time=%.1f ms", p->param.time);
    printf("\n");
}

void    print_end(t_ping *p) {
    printf("--- %s ping statistics ---\n", p->DNS);
    printf("%d packets transmitted, %d received, %d%% packet loss, time %.1fms\n", p->param.icmp_seq, p->param.icmp_seq, 100, p->param.totaltime);
    printf("rtt min/avg/max/mdev = %.1f/%.1f/%.1f/%.1d ms\n", p->param.minTime, p->param.avg, p->param.maxTime, 100);
}

void print_type(t_ping *p) {
    void *addr;
    memset(p->DNS, '\0', sizeof(p->DNS));
    if (p->result->ai_family == AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)&p->result->ai_addr;
        p->param.port = ntohs(ipv4->sin_port);
        addr = &ipv4->sin_addr;
    }
    else if (p->result->ai_family == AF_INET6) {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&p->result->ai_addr;
        addr = &ipv6->sin6_addr;
        p->param.port = ntohs(ipv6->sin6_port);
    }
    else
        return ;
    if (p->result != NULL)
        inet_ntop(p->result->ai_family, addr, p->DNS, sizeof(p->DNS));
    printf("PING %s (%s) %d(%d) bytes of data\n", p->addrName, p->DNS, p->packageSize - 8, p->packageSize + 20);
}

void    init_param(t_ping *p) {
    p->param.icmp_seq = 0;
    p->param.minTime = 0;
    p->param.maxTime = 0;
    p->param.port = 0;
    p->param.avg = 0;
    p->param.mdev = 0;
    p->param.totaltime = 0;
    p->param.ttl = 255;
}

int    init_package(t_ping *p) {
    p->package.all = NULL;
    p->package.data = NULL;
    p->package.all = malloc(sizeof(char) * (p->packageSize + 20));
    if (p->package.all == NULL)
        return 1;
    p->package.data = malloc(sizeof(char) * (p->packageSize - 8));
    if (p->package.data == NULL) {
        free(p->package.all);
        return 1;
    }
    p->package.allSize = p->packageSize + 20;
    p->package.dataSize = p->packageSize - 8;
    return 0;
}

void    set_stats_time(t_ping *p) {
    if (p->param.maxTime < p->param.time) {
        p->param.maxTime = p->param.time;
    }
    if (p->param.minTime > p->param.time || p->param.minTime == 0) {
        p->param.minTime = p->param.time;
    }
    p->param.totaltime += p->param.time;
    p->param.avg = p->param.totaltime / p->param.icmp_seq;
}

void    get_header(t_ping *p) {
    unsigned char *buf = (unsigned char *)p->package.data;

    p->param.ttl = buf[8];
}

void    loop(t_ping *p) {
    int fdsocket;
    struct timeval start;
    struct timeval last;
    
    init_param(p);
    fdsocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    int count = 0;
    memset(p->package.data, '1', p->package.dataSize);
    memset(p->package.all, '1', p->package.allSize);
    
    sendto(fdsocket, p->package.data, p->package.dataSize, 0, p->result->ai_addr, p->result->ai_addrlen);
    gettimeofday(&start, NULL);
    gettimeofday(&last, NULL);
    while (g_verif) {
        int bytesread = recvfrom(fdsocket, p->package.all, p->package.allSize, 0, p->result->ai_addr, &p->result->ai_addrlen);
        if (bytesread <= 0) {
            count++;
            p->param.icmp_seq++;
            p->param.time = (last.tv_sec * (uint64_t)1000) + (last.tv_usec / 1000) - (start.tv_sec * (uint64_t)1000) + (start.tv_usec / 1000);
            get_header(p);
            set_stats_time(p);
            print_loop(p);
            usleep(USLEEP_ONE_SEC);
            gettimeofday(&start, NULL);
            sendto(fdsocket, p->package.data, p->package.dataSize, 0, p->result->ai_addr, p->result->ai_addrlen);
        }
        gettimeofday(&last, NULL);
    }
}

void init_hints(struct addrinfo *hints) {
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = 0;
    hints->ai_flags = AI_CANONNAME;
}

int check_addr(char *name, t_ping *p) {
    struct addrinfo hints;
    struct addrinfo *result;

    if (name == NULL)
        return 1;
    p->addrName = name;
    memset(&hints, 0, sizeof(struct addrinfo));
    init_hints(&hints);
    p->packageSize = PACKAGE_SIZE;
    if (getaddrinfo(p->addrName, "80", &hints, &result) != 0)
        return 1;
    for (struct addrinfo *i = result; i != NULL && g_verif == 1; i = i->ai_next) {
        p->result = i;
        print_type(p);
        loop(p);
    }
    return 0;
}

void check_signal(int signal) {
    if (signal == SIGINT) {
        g_verif = 0;
        printf("\n");
        return ;
    }
}

void create_signal() {
    struct sigaction s;

    signal(SIGQUIT, SIG_IGN);
    memset(&s, '\0', sizeof(s));
    s.sa_handler = &check_signal;
    sigaction(SIGINT, &s, NULL);
}

int main(int argc, char **argv)
{
    t_ping  p;

    if (argc <= 1)
    {
        dprintf(1, "ft_ping: usage error: Destination address required\n");
        exit(1);
    }
    create_signal();
    if (init_package(&p) == 1)
        return 1;
    if (check_addr(argv[argc - 1], &p) == 1)
        return 1;
    print_end(&p);
    freeaddrinfo(p.result);
}