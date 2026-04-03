/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 19:58:17 by amaury            #+#    #+#             */
/*   Updated: 2026/04/03 14:53:08 by amaury           ###   ########.fr       */
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
# include <ctype.h>
# include <stdbool.h>
# include <unistd.h>
# include <getopt.h>

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
	bool		timestamp;
	int			limit_icmp_seq;
	int			icmp_seq;
	int			received;
	u_int8_t	ttl;
	uint32_t	port;
	int			size_tab;
	double		*time_tab;

	double		time;
	double		lauch_time;
	double		total_time;
	double		min_time;
	double		max_time;
	double		avg;
	struct timeval	start;
}				t_param;

typedef struct s_packageIP
{
	char	*all;
	int		all_size;
	char	*data;
	int		data_size;
}			t_packageIP;

typedef struct s_ping
{
	int				socket;
	struct addrinfo	*result;
	char			*addr_name;
	char			dns[INET6_ADDRSTRLEN];
	int				package_size;
	bool			sound;
	bool			wait;
	bool			debug;
	bool			verbose;
	bool			reverse;
	char			hbuf[NI_MAXHOST];
	t_packageIP		package;
	t_param			param;
}					t_ping;

///  print.c
void	print_end(t_ping *p);
void	print_loop(t_ping *p, int size);
void	print_header(t_ping *p);
void	print_help();
///  signal.c
void	check_signal(int signal);
void	create_signal(void);
///  init.c
void	init_param(t_ping *p);
int		init_package(t_ping *p);
void	init_hints(struct addrinfo *hints);
void	init_ping(t_ping *p);
void	create_package(t_ping *p);
///  main.c
void	free_all(t_ping *p);
///  package.c
void		package_send(t_ping *p, struct timeval start, struct timeval last);
void		package_error(t_ping *p, struct timeval start, struct timeval last);
uint16_t	calculate_checksum(unsigned char* buffer, int bytes);
void		build_icmp_packet(t_ping *p);
void    	get_header(t_ping *p);
///  parsing.c
void	parsing(t_ping *p, char **argv, int argc);

#endif