/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaury <amaury@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 20:16:10 by amaury            #+#    #+#             */
/*   Updated: 2026/04/02 19:15:05 by amaury           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>

extern int	g_verif;

void	check_signal(int signal)
{
	if (signal == SIGINT)
	{
		g_verif = 0;
		return ;
	}
}

void	create_signal(void)
{
	struct sigaction	s;

	signal(SIGQUIT, SIG_IGN);
	memset(&s, '\0', sizeof(s));
	s.sa_handler = &check_signal;
	sigaction(SIGINT, &s, NULL);
}
