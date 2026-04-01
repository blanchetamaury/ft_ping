/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 19:58:07 by amaury            #+#    #+#             */
/*   Updated: 2026/04/01 20:57:55 by amaury           ###   ########.fr       */
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
    printf("%d bytes from %s: ", size, p->dns);
    printf("icmp_seq=%d ", p->param.icmp_seq);
    printf("ttl=%d ", p->param.ttl);
    printf("time=%.1f ms", p->param.time);
    printf("\n");
}

void    print_end(t_ping *p)
{
    printf("--- %s ping statistics ---\n", p->dns);
    printf("%d packets transmitted, %d received, %d%% packet loss, time %.1fms\n", p->param.icmp_seq, p->param.icmp_seq, 100, p->param.total_time);
    printf("rtt min/avg/max/mdev = %.1f/%.1f/%.1f/%.1d ms\n", p->param.min_time, p->param.avg, p->param.max_time, 100);
}
