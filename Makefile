NAME = webserv
CPP = c++ -g
FLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address 
SRC_FILES = main.cpp server/server.cpp server/main_loop.cpp response/response.cpp \
			request/request.cpp client/client.cpp \
			aubb/aubServer.cpp aubb/Location.cpp aubb/Config.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)
DEP  = server.hpp request/request.hpp aubb/Location.hpp aubb/Config.hpp

all: $(NAME)

$(NAME): $(OBJ_FILES)
	$(CPP) $(FLAGS) $(OBJ_FILES) -o $(NAME)

%.o: %.cpp $(DEP)
	$(CPP) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_FILES)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all fclean re clean