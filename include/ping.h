/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 19:58:17 by amaury            #+#    #+#             */
/*   Updated: 2026/04/02 10:07:06 by amaury           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <netdb.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <arpa/inet.h>
# include <sys/time.h>
# include <unistd.h>
# include <signal.h>

# define PACKAGE_SIZE 64
# define USLEEP_ONE_SEC 1000000

typedef struct s_icmp
{
    uint8_t		type;
    uint8_t		code;
    uint16_t	checksum;
    uint16_t	ident;
    uint16_t	seq;
} 			t_icmp;

typedef struct s_param
{
	int			icmp_seq;
	int			received;
	u_int8_t	ttl;
	uint32_t	port;

	double		time;
	double		total_time;
	double		min_time;
	double		max_time;
	double		avg;
	double		mdev;
}				t_param;

typedef struct s_packageIP
{
	char	*all;
	int		all_size;
	char	*data;
	int		data_size;
	char	header_ip[20];
	char	header_icmp[8];
}			t_packageIP;

typedef struct s_ping
{
	int				socket;
	struct addrinfo	*result;
	char			*addr_name;
	char			dns[INET6_ADDRSTRLEN];
	int				package_size;
	t_packageIP		package;
	t_param			param;
}					t_ping;

///  print.c
void	print_end(t_ping *p);
void	print_loop(t_ping *p, int size);
void	print_header(t_ping *p);
///  signal.c
void	check_signal(int signal);
void	create_signal(void);
///  init.c
void	init_param(t_ping *p);
int		init_package(t_ping *p);
void	init_hints(struct addrinfo *hints);
#endif