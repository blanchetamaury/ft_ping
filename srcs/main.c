/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 19:58:10 by amaury            #+#    #+#             */
/*   Updated: 2026/04/01 20:37:48 by amaury           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>

int	g_verif = 1;

void	print_type(t_ping *p)
{
	void				*addr;
	struct sockaddr_in	*ipv4;
	struct sockaddr_in6	*ipv6;

	memset(p->dns, '\0', sizeof(p->dns));
	if (p->result->ai_family == AF_INET)
	{
		ipv4 = (struct sockaddr_in *)&p->result->ai_addr;
		p->param.port = ntohs(ipv4->sin_port);
		addr = &ipv4->sin_addr;
	}
	else if (p->result->ai_family == AF_INET6)
	{
		ipv6 = (struct sockaddr_in6 *)&p->result->ai_addr;
		addr = &ipv6->sin6_addr;
		p->param.port = ntohs(ipv6->sin6_port);
	}
	if (p->result != NULL)
		inet_ntop(p->result->ai_family, addr, p->dns, sizeof(p->dns));
}

void    set_stats_time(t_ping *p)
{
	if (p->param.max_time < p->param.time)
		p->param.max_time = p->param.time;
	if (p->param.min_time > p->param.time || p->param.min_time == 0)
		p->param.min_time = p->param.time;
	p->param.total_time += p->param.time;
	p->param.avg = p->param.total_time / p->param.icmp_seq;
}

void    get_header(t_ping *p) {
	unsigned char *buf;

	buf = (unsigned char *)p->package.data;
	p->param.ttl = buf[8];
}

void	loop(t_ping *p)
{
	int				fdsocket;
	struct timeval	start;
	struct timeval	last;

	init_param(p);
	print_header(p);
	
	fdsocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	
	sendto(fdsocket, p->package.data, p->package.data_size, 0, p->result->ai_addr, p->result->ai_addrlen);
	gettimeofday(&start, NULL);
	gettimeofday(&last, NULL);
	while (g_verif)
	{
		int bytesread = recvfrom(fdsocket, p->package.all, p->package.all_size, 0, p->result->ai_addr, &p->result->ai_addrlen);
		if (bytesread <= 0)
		{
			p->param.icmp_seq++;
			p->param.time = (last.tv_sec * (uint64_t)1000) + (last.tv_usec / 1000) - (start.tv_sec * (uint64_t)1000) + (start.tv_usec / 1000);
			get_header(p);
			set_stats_time(p);
			print_loop(p);
			usleep(USLEEP_ONE_SEC);
			gettimeofday(&start, NULL);
			sendto(fdsocket, p->package.data, p->package.data_size, 0, p->result->ai_addr, p->result->ai_addrlen);
		}
		gettimeofday(&last, NULL);
	}
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
	if (getaddrinfo(p->addr_name, "80", &hints, &result) != 0)
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
