/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbouzalm <tbouzalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 11:43:40 by tbouzalm          #+#    #+#             */
/*   Updated: 2023/03/23 19:52:30 by tbouzalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libc.h>

int msg_err(char *msg, char *av)
{
    while (msg && *msg)
        write(2, msg++, 1);
    if (av)
        while (*av)
            write(2, av++, 1);
    write(2, "\n", 1);
    return (1);
}

int execution(char **av, int i, int tmp, char **env)
{
    av[i] = NULL;
    dup2(tmp, 0);
    close(tmp);
    execve(av[0],av, env);
    return (msg_err("error: cannot execute ", av[0]));
}

int main(int ac, char **av, char **env)
{
    int i;
    int tmp;
    int fd[2];
    (void)ac;

    i = 0;
    tmp = dup(0);
    if (ac == 1)
        return (1);
    while (av[i] && av[i + 1])
    {
        av = &av[i + 1];
        i = 0;
        while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
            i++;
        if (strcmp(av[0], "cd") == 0)
        {
            if (i != 2)
                msg_err("error: cd: bad argumenet", NULL);
            else if (chdir(av[1]) != 0)
                msg_err("error: cd: cannot change directory to ", av[1]);
        }
        else if (i != 0 && (av[i] == NULL || strcmp(av[i], ";") == 0))
        {
            if (fork () == 0)
            {
                if (execution(av, i, tmp, env))
                    return (1);
            }
            else
            {
                close(tmp);
                while (waitpid(-1, NULL, 0) != -1)
                    ;
                tmp = dup(0);
            }
        }
        else if (i != 0 && strcmp(av[i], "|") == 0)
        {
            pipe(fd);
            if (fork() == 0)
            {
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                if (execution(av, i, tmp, env))
                    return (1);
            }
            else
            {
                close(fd[1]);
                close(tmp);
                tmp = fd[0];
            }
        }
    }
    close(tmp);
    return (0);
}