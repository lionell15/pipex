NAME	=	pipex
SRCS	=	pipex.c utils.c
OBJS	=	${SRCS:.c=.o}
HEADER	=	-Iincludes
CC		=	gcc
CFLAGS	=	-Wall -Wextra -Werror -g

.c.o:		%.o : %.c
					@gcc ${CFLAGS} ${HEADER} -c $< -o $(<:.c=.o)

all: 		${NAME}

${NAME}:	${OBJS}
					@make re -C ./libft
					@$(CC) ${CFLAGS}  ${OBJS} -Llibft -lft -o ${NAME}
clean:
					@make clean -C ./libft
					@rm -f ${OBJS} ${OBJS_B}

fclean: 	clean
					@make fclean -C ./libft
					@rm -f ${NAME}

re:			fclean all

.PHONY: all clean fclean re
