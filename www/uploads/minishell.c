/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mea <mea@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 12:14:09 by narnaud           #+#    #+#             */
/*   Updated: 2022/05/27 09:35:57 by narnaud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	termios(int ctl)
{
	struct termios	termios_p;
	int				tty;

	tty = ttyslot();
	tcgetattr(tty, &termios_p);
	if (ctl)
	{
		termios_p.c_lflag |= ECHOCTL;
		signal(SIGINT, nothing);
		signal(SIGQUIT, nothing);
	}
	else
	{
		termios_p.c_lflag &= ~(ECHOCTL);
		signal(SIGINT, sigs_handler);
		signal(SIGQUIT, sigs_handler);
	}
	tcsetattr(tty, TCSANOW, &termios_p);
}

void	free_cmd(t_command *cmd)
{
	int	i;

	if (!cmd)
		return ;
	i = 0;
	while (cmd->argv[i])
	{
		free(cmd->argv[i]);
		i++;
	}
	free(cmd->argv);
	free_cmd(cmd->next);
	free(cmd);
}

void	prompt(t_datas *datas, char *line)
{
	t_command	*cmd;

	termios(0);
	if (!line)
		line = readline("$ ");
	termios(1);
	if (line == NULL)
		halt(datas, EXIT_SUCCESS, datas->silent);
	if (is_empty(line))
		return ;
	cmd = parser(datas, lexer(datas, line), NULL);
	if (cmd)
	{
		caller(datas, cmd);
		free_cmd(cmd);
		add_history(line);
	}
}

int	main(int argc, char **argv, char **envp)
{
	static t_datas	datas;
	static int		i = 0;

	datas.silent = (argc > 2 && !ft_strncmp(argv[1], "-c", 3));
	datas.envp = ft_calloc(ENVP_MAX_SIZE, sizeof(char *));
	while (*envp)
		datas.envp[i++] = ft_strdup(*(envp++));
	if (!*get_var_value(&datas, "MINISH", 7))
		datas.envp[i] = ft_strjoin_with("MINISH=", "/minishell",
				getcwd(NULL, 0));
	datas.exit_code = 0;
	if (datas.silent)
		prompt(&datas, argv[2]);
	else
		while (1)
			prompt(&datas, NULL);
	halt(&datas, EXIT_SUCCESS, datas.silent);
}