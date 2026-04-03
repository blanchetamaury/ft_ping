/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 20:24:29 by amaury            #+#    #+#             */
/*   Updated: 2026/04/03 10:40:55 by amaury           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>

void	init_param(t_ping *p)
{
	p->param.icmp_seq = 0;
	p->param.min_time = 0;
	p->param.max_time = 0;
	p->param.port = 0;
	p->param.avg = 0;
	p->param.total_time = 0;
	p->param.lauch_time = 0;
	p->param.ttl = 255;
	p->param.received = 0;
	p->param.size_tab = 10;
	p->param.time_tab = NULL;
	p->param.limit_icmp_seq = -1;
	p->param.timestamp = false;
	p->param.time_tab = calloc(p->param.size_tab, sizeof(double));
	if (p->param.time_tab == NULL) {
		printf("ft_ping Error memory allocation\n");
		free_all(p);
		exit(1);
	}
}

int	init_package(t_ping *p)
{
	p->package_size = PACKAGE_SIZE;
	p->package.all = NULL;
	p->package.data = NULL;
	return (0);
}


void	create_package(t_ping *p)
{
	p->package.all = malloc(sizeof(char) * (p->package_size));
	if (p->package.all == NULL)
	{
		free_all(p);
		exit (1);
	}
	p->package.data = malloc(sizeof(char) * (p->package_size ));
	if (p->package.data == NULL)
	{
		free_all(p);
		exit (1);
	}
	p->package.all_size = p->package_size;
	p->package.data_size = p->package_size;
	memset(p->package.data, '1', p->package.data_size);
	memset(p->package.all, '1', p->package.all_size);
}

void init_ping(t_ping *p)
{
	p->sound = false;
	p->wait = true;
	p->result = NULL;
	p->debug = false;
	p->verbose = false;
}

void	init_hints(struct addrinfo *hints)
{
	hints->ai_family = AF_UNSPEC;
	hints->ai_socktype = 0;
	hints->ai_flags = AI_CANONNAME;
}