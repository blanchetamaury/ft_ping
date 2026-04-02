/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   package.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 15:34:08 by amaury            #+#    #+#             */
/*   Updated: 2026/04/02 15:38:31 by amaury           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>

void    get_header(t_ping *p)
{
	unsigned char *buf;

	buf = (unsigned char *)p->package.all;
	p->param.ttl = buf[8];
}

void	package_send(t_ping *p, struct timeval start, struct timeval last)
{
	t_icmp	*reply;
	int		ip_header_len;

	if (p->result->ai_family == AF_INET6)
	{
		reply = (t_icmp *)p->package.all;
		p->param.ttl = 0;
	}
	else
	{
		ip_header_len = (p->package.all[0] & 0x0F) * 4;
		reply = (t_icmp *)(p->package.all + ip_header_len);
		get_header(p);
	}

	if (reply->ident != htons(getpid()))
		return ;

	p->param.received++;
	p->param.time = ((last.tv_sec - start.tv_sec) * 1000000.0 + (last.tv_usec - start.tv_usec)) / 1000.0;
	p->param.total_time += p->param.time;
	p->param.time_tab[p->param.icmp_seq - 1] = p->param.time;
	print_loop(p, p->package_size);
}

void	package_error(t_ping *p, struct timeval start, struct timeval last)
{
	p->param.time = ((last.tv_sec - start.tv_sec) * 1000000.0 + (last.tv_usec - start.tv_usec)) / 1000.0;
	p->param.total_time += p->param.time;
	p->param.time_tab[p->param.icmp_seq - 1] = p->param.time;
	print_loop(p, 0);
}

uint16_t calculate_checksum(unsigned char* buffer, int bytes)
{
    uint32_t checksum = 0;
    unsigned char* end = buffer + bytes;

    if (bytes % 2 == 1) {
        end = buffer + bytes - 1;
        checksum += (*end) << 8;
    }

    while (buffer < end) {
		checksum += *(uint16_t *)buffer;
		buffer += 2;
	}

    uint32_t carray = checksum >> 16;
    while (carray) {
        checksum = (checksum & 0xffff) + carray;
        carray = checksum >> 16;
    }

    checksum = ~checksum;
    return checksum & 0xffff;
}

void build_icmp_packet(t_ping *p)
{
    t_icmp *hdr;

    hdr = (t_icmp *)p->package.all;
    hdr->code = 0;
    hdr->checksum = 0;
    hdr->ident = htons(getpid());
	hdr->seq = htons(p->param.icmp_seq);

	if (p->result->ai_family == AF_INET6)
        hdr->type = 128;
    else
    {
        hdr->type = 8;
		hdr->checksum = calculate_checksum((unsigned char *)p->package.all, p->package.all_size);
    }
}
