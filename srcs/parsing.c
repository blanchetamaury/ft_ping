/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 17:45:27 by amaury            #+#    #+#             */
/*   Updated: 2026/04/03 10:41:06 by amaury           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>

int	check_value(char *arg)
{
	for (int i = 0; arg[i]; i++) {
		if (isdigit(arg[i]) == 0)
			return (1);
	}
	return (0);
}

void	parsing(t_ping *p, char **argv, int argc)
{	
	int opt;

	while ((opt = getopt(argc, argv, "?aAchVDdsv")) != -1)
	{
		switch (opt)
		{
			case 'a':
				p->sound = true;
				break ;
			case 'A':
				p->wait = false;
				break ;
			case 'c':
				if (optarg == NULL) {
					printf("ft_ping : l'option requiert un argument -- c\n\n");
					print_help();
					free_all(p);
					exit (1);
				}
				if (check_value(optarg) == 0)
					p->param.limit_icmp_seq = atoi(optarg);
				else {
					printf("ft_ping: invalid argument: '%s'\n", optarg);
					free_all(p);
					exit(1);
				}
				break ;
			case 'h':
				print_help();
				free_all(p);
				exit (1);
				break ;
			case 'V':
				printf("ft_ping from amblanch v.20260402\n");
				free_all(p);
				exit (1);
				break ;
			case 'v':
				p->verbose = true;
				break ;
			case 'D':
				p->param.timestamp = true;
				break ;
			case 'd':
				p->debug = true;
				break ;
			case 's':
				if (optarg == NULL) {
					printf("ft_ping : l'option requiert un argument -- s\n\n");
					print_help();
					free_all(p);
					exit (1);
				}
				if (check_value(optarg) == 0)
					p->package_size = atoi(optarg) + 8;
				else {
					printf("ft_ping: invalid argument: '%s'\n", optarg);
					free_all(p);
					exit(1);
				}
				break ;
			default:
				printf("ft_ping : option invalide -- '%s'\n\n", optarg);
				print_help();
				free_all(p);
				exit (1);
				break ;
		}
	}
}
