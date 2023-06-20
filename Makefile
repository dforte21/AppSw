NAME=AppSw
SRC_DIR=./source
SRC_FILE =	$(SRC_DIR)/appsw.cpp \
			$(SRC_DIR)/main.cpp	\
			$(SRC_DIR)/crc.cpp
FLAGS=-c -Wall -Wextra -Werror
CFLAGS=-lssl -lcrypto -o

$(NAME):
		c++ -c $(SRC_FILE)
		mv *.o ./objs
		c++ ./objs/*.o $(CFLAGS) $(NAME)

all:	$(NAME)

clean:
		rm -rf ./objs/*.o

fclean:	clean
		rm -rf $(NAME)

re:		fclean all

.PHONY:	all clean fclean re