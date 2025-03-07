CXX = c++
# CFLAGS = -g3 -Wall -Wextra -Werror -std=c++98 -fsanitize=address 
# CFLAGS = -g3 -Wall -Wextra -Werror -std=c++98
CFLAGS = -g3 #-Wall -Wextra -Werror 
SRCS = main.cpp Connection.cpp Request.cpp Conf.cpp Response.cpp response_utils.cpp log.cpp \
		$(addprefix configfile/,  error_pages.cpp server_data.cpp location.cpp utils.cpp cgi_data.cpp)
OBJS = $(SRCS:.cpp=.o)
HEDERS =  Connection.hpp Request.hpp Response.hpp Conf.hpp Response.hpp colors.hpp log.hpp \
		$(addprefix configfile/,  error_pages.hpp server_data.hpp location.hpp cgi_data.hpp)
NAME = webserv

all : $(NAME)

debug: re
	clear
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME)

hrun: re
	clear
	./$(NAME)

run: all
	clear
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
