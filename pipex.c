#include "pipex.h"

static void	spawn_child(char **argv, char **envp, int *fd)
{
	int	filein;

	filein = open(argv[1], O_RDONLY, 0777);
	if (filein == -1)
		write(1,"(null)",6) ;
	dup2(fd[1], 1);
	dup2(filein, 0);
	close(fd[0]);
	execute(argv[2], envp);
}

static void	ret_parent(char **argv, char **envp, int *fd)
{
	int	fileout;

	fileout = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fileout == -1)
		write(1,"(null)",6) ;
	dup2(fd[0], 0);
	dup2(fileout, 1);
	close(fd[1]);
	execute(argv[3], envp);
}

int	main(int argc, char **argv, char **envp)
{
	int		fd[2];
	pid_t	pid;

	if (argc == 5)
	{
		if (pipe(fd) == -1)
			return (0);
		pid = fork();
		if (pid == -1)
			return (0);
		if (pid == 0)
			spawn_child(argv, envp, fd);
		waitpid(pid, NULL, 0);
		ret_parent(argv, envp, fd);
	}
	return (0);
}
