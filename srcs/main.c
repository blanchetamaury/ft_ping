/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 19:58:10 by amaury            #+#    #+#             */
/*   Updated: 2026/04/02 10:53:46 by amaury           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>

int	g_verif = 1;

uint16_t calculate_checksum(unsigned char* buffer, int bytes) {
    uint32_t checksum = 0;
    unsigned char* end = buffer + bytes;

    // Handle odd bytes by adding the last byte to the checksum
    if (bytes % 2 == 1) {
        end = buffer + bytes - 1;
        checksum += (*end) << 8;
    }

    // Add words of two bytes
    while (buffer < end) {
		checksum += *(uint16_t *)buffer;  // lecture native
		buffer += 2;
	}

    // Fold carry bits
    uint32_t carray = checksum >> 16;
    while (carray) {
        checksum = (checksum & 0xffff) + carray;
        carray = checksum >> 16;
    }

    // Return one's complement
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


void	print_type(t_ping *p)
{
	void				*addr;
	struct sockaddr_in	*ipv4;
	struct sockaddr_in6	*ipv6;
	struct timeval		timeout;
	int					on;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	memset(p->dns, '\0', sizeof(p->dns));
	if (p->result->ai_family == AF_INET)
	{
		ipv4 = (struct sockaddr_in *)p->result->ai_addr;
		p->param.port = ntohs(ipv4->sin_port);
		addr = &ipv4->sin_addr;
		p->socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
		setsockopt(p->socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	}
	else if (p->result->ai_family == AF_INET6)
	{
		ipv6 = (struct sockaddr_in6 *)p->result->ai_addr;
		addr = &ipv6->sin6_addr;
		p->param.port = ntohs(ipv6->sin6_port);
		p->socket = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
		on = 1;
		setsockopt(p->socket, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &on, sizeof(on));
	}
	else
		return ;
	if (p->result != NULL)
		inet_ntop(p->result->ai_family, addr, p->dns, sizeof(p->dns));
}

void    set_stats_time(t_ping *p)
{
	if (p->param.max_time < p->param.time)
		p->param.max_time = p->param.time;
	if (p->param.min_time > p->param.time || p->param.min_time == 0)
		p->param.min_time = p->param.time;
	p->param.avg = p->param.total_time / p->param.icmp_seq;
}

void    get_header(t_ping *p) {
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
	set_stats_time(p);
	print_loop(p, p->package_size);
}

void	package_error(t_ping *p, struct timeval start, struct timeval last)
{
	p->param.time = ((last.tv_sec - start.tv_sec) * 1000000.0 + (last.tv_usec - start.tv_usec)) / 1000.0;
	set_stats_time(p);
	print_loop(p, 0);
}

void	loop(t_ping *p)
{
	struct timeval	start;
	struct timeval	last;
	struct timeval	first;
	int				bytesread;

	init_param(p);
	print_header(p);
	
	gettimeofday(&first, NULL);
	while (g_verif)
	{
		p->param.icmp_seq++;
		build_icmp_packet(p);
		
		sendto(p->socket, p->package.all, p->package.all_size, 0, p->result->ai_addr, p->result->ai_addrlen);
		gettimeofday(&start, NULL);
		
		bytesread = recvfrom(p->socket, p->package.all, p->package.all_size, 0, NULL, NULL);
		gettimeofday(&last, NULL);
		
		if (bytesread > 0)
			package_send(p, start, last);
		else
			package_error(p, start, last);
		usleep(USLEEP_ONE_SEC);
	}
	p->param.total_time = ((last.tv_sec - first.tv_sec) * 1000000.0 + (last.tv_usec - first.tv_usec)) / 1000.0;
}

int	check_addr(char *name, t_ping *p)
{
	struct addrinfo	hints;
	struct addrinfo	*result;

	if (name == NULL)
		return (1);
	p->addr_name = name;
	memset(&hints, 0, sizeof(struct addrinfo));
	init_hints(&hints);
	if (getaddrinfo(p->addr_name, NULL, &hints, &result) != 0)
		return (1);
	for (struct addrinfo *i = result; i != NULL && g_verif == 1; i = i->ai_next)
	{
		p->result = i;
		print_type(p);
		loop(p);
	}
	return (0);
}

void	free_all(t_ping *p)
{
	free(p->package.all);
	free(p->package.data);
	freeaddrinfo(p->result);
}

int	main(int argc, char **argv)
{
	t_ping	p;

	if (argc <= 1)
	{
		dprintf(1, "ft_ping: usage error: Destination address required\n");
		exit(1);
	}
	create_signal();
	if (init_package(&p) == 1)
		return (1);
	if (check_addr(argv[argc - 1], &p) == 1)
		return (1);
	print_end(&p);
	free_all(&p);
}
