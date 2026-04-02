/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 20:24:29 by amaury            #+#    #+#             */
/*   Updated: 2026/04/02 12:38:41 by amaury           ###   ########.fr       */
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
	p->package.all = malloc(sizeof(char) * (p->package_size + 20));
	if (p->package.all == NULL)
		return (1);
	p->package.data = malloc(sizeof(char) * (p->package_size - 8));
	if (p->package.data == NULL)
	{
		free(p->package.all);
		return (1);
	}
	p->package.all_size = p->package_size + 20;
	p->package.data_size = p->package_size - 8;
	memset(p->package.data, '1', p->package.data_size);
	memset(p->package.all, '1', p->package.all_size);
	return (0);
}

void	init_hints(struct addrinfo *hints)
{
	hints->ai_family = AF_UNSPEC;
	hints->ai_socktype = 0;
	hints->ai_flags = AI_CANONNAME;
}