/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 19:58:07 by amaury            #+#    #+#             */
/*   Updated: 2026/04/02 21:47:10 by amaury           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>

void	print_header(t_ping *p)
{
	printf("PING %s ", p->addr_name);
	printf("(%s) ", p->dns);
	printf(" %d(%d) bytes of data\n", p->package_size - 8, p->package_size + 20);
}


void    print_loop(t_ping *p, int size)
{
	if (p->param.timestamp == true)
		printf("[%f] ", (p->param.start.tv_sec * 1000000.0 + p->param.start.tv_usec) / 1000.0);
    printf("%d bytes from %s: ", size, p->dns);
    printf("icmp_seq=%d ", p->param.icmp_seq);
    printf("ttl=%d ", p->param.ttl);
    printf("time=%.2f ms", p->param.time);
    printf("\n");
	if (p->sound == true)
		write(1, "\a", 1);
}

void    print_end(t_ping *p)
{
	double		total_diff;
	double		diff;

	for (int i = 0; i < p->param.icmp_seq; i++) {
		diff = p->param.time_tab[i] - p->param.avg;
		if (diff < 0)
			diff *= -1;
		total_diff += diff;
	}
	
    printf("\n--- %s ping statistics ---\n", p->addr_name);
    printf("%d packets transmitted, %d received, %d%% packet loss, time %.1fms\n", p->param.icmp_seq, p->param.received, 100 - 100 * p->param.received / p->param.icmp_seq, p->param.lauch_time);
    printf("rtt min/avg/max/mdev = %.1f/%.1f/%.1f/%.1f ms\n", p->param.min_time, p->param.avg, p->param.max_time, total_diff / p->param.icmp_seq);
}

void print_help()
{
	printf("Usage\n");
	printf("	ping [options] <destination>\n\n");
	printf("Options:\n");
	printf("	<destination>      DNS name or IP address\n");
	printf("	-a                 use audible ping\n");
	printf("	-A                 use adaptive ping\n");
	printf("	-c <count>         stop after <count> replies\n");
	printf("	-D                 print timestamps\n");
	printf("	-d                 use SO_DEBUG socket option\n");
	printf("	-h                 print help and exit\n");
	printf("	-s <size>          use <size> as number of data bytes to be sent\n");
	printf("	-V                 print version and exit\n\n");
}