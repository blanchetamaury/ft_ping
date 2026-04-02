/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 19:58:10 by amaury            #+#    #+#             */
/*   Updated: 2026/04/02 15:38:21 by amaury           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>

int	g_verif = 1;

void	free_all(t_ping *p)
{
	free(p->package.all);
	free(p->package.data);
	free(p->param.time_tab);
	freeaddrinfo(p->result);
}

void	set_ipv(t_ping *p)
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

void	resize_time_tab(t_ping *p)
{
	double	*new;

	new = calloc(p->param.size_tab * 2, sizeof(double));
	if (new == NULL) {
		printf("ft_ping Error memory allocation\n");
		free_all(p);
		exit(1);
	}
	for (int i = 0; i < p->param.size_tab; i++) {
		new[i] = p->param.time_tab[i];
	}
	free(p->param.time_tab);
	p->param.time_tab = new;
	p->param.size_tab *= 2;
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
		if (p->param.icmp_seq > p->param.size_tab)
			resize_time_tab(p);
		build_icmp_packet(p);
		
		sendto(p->socket, p->package.all, p->package.all_size, 0, p->result->ai_addr, p->result->ai_addrlen);
		gettimeofday(&start, NULL);
		
		bytesread = recvfrom(p->socket, p->package.all, p->package.all_size, 0, NULL, NULL);
		gettimeofday(&last, NULL);
		
		if (bytesread > 0)
			package_send(p, start, last);
		else
			package_error(p, start, last);
		set_stats_time(p);
		usleep(USLEEP_ONE_SEC);
	}
	p->param.lauch_time = ((last.tv_sec - first.tv_sec) * 1000000.0 + (last.tv_usec - first.tv_usec)) / 1000.0;
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
		set_ipv(p);
		loop(p);
	}
	return (0);
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
