/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfagri <mfagri@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/18 22:54:53 by mfagri            #+#    #+#             */
/*   Updated: 2022/10/18 23:21:52 by mfagri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int ft_printerror(char *s,char *arg)
{
    while(*s)
        write(2,s++,1);
    if(arg)
        while(*arg)
            write(2,arg++,1);
    write(2,"\n",1);
    return (1);
}

int ft_execve(int i,char **av,int tmp_fd,char **env)
{
    av[i] = NULL;
    dup2(tmp_fd,0);
    close(tmp_fd);
    execve(av[0],av,env);
    return(ft_printerror("error: cannot execute",av[0]));
}

int main(int ac,char **av,char **env)
{
    int i;
    int fd[2];
    int tmp_fd;

    (void)ac;
    tmp_fd = dup(0);
    i = 0;
    while (av[i] && av[i + 1])
    {
        av = &av[i + 1];
        i = 0;
        while(av[i] && strcmp(av[i],";") && strcmp(av[i],"|"))
            i++;
        if(strcmp(av[0],"cd") == 0)
        {
            if(i != 2)
                ft_printerror("error: cd: bad arguments",NULL);
            if(chdir(av[1]) != 0)
                ft_printerror("error: cd: cannot change directory to",av[1]);
        }
        else if (i != 0 && (av[i] == NULL || strcmp(av[i],";") == 0))
        {
            if(fork() == 0)
            {
                if(ft_execve(i,av,tmp_fd,env))
                    return (1);
            }
            else
            {
                close(tmp_fd);
                while (waitpid(-1,NULL,WUNTRACED) != -1);
                tmp_fd = dup(0);
            }
        }
        else if(i != 0 && strcmp(av[i],"|") == 0)
        {
            pipe(fd);
            if(fork() == 0)
            {
                dup2(fd[1],1);
                close(fd[1]);
                close(fd[0]);
                if(ft_execve(i,av,tmp_fd,env))
                    return(1);
            }
            else
            {
                close(fd[1]);
                close(tmp_fd);
                tmp_fd = fd[0];
            }
        }
    }
    
    close(tmp_fd);
    return(0);
}