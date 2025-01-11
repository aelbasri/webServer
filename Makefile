CXX = c++
CFLAGS = -g3 -Wall -Wextra -Werror -std=c++98 -fsanitize=address 
SRCS = server.cpp main.cpp Request.cpp
OBJS = $(SRCS:.cpp=.o)
HEDERS = server.hpp Request.hpp
NAME = webserv

all : $(NAME)

run: re
	./$(NAME)

%.o : %.cpp 
	$(CXX) -c $(CFLAGS)  -o $@ $^

$(NAME) : $(OBJS) $(HEDERS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(NAME)

clean : 
	rm -f $(OBJS)
fclean : clean
	rm -f $(NAME)
re : fclean all
