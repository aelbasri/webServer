CXX = c++
CFLAGS = -g3 -Wall -Wextra -Werror -std=c++98 -fsanitize=address 
SRCS = main.cpp Request.cpp Conf.cpp Response.cpp utils.cpp CGI.cpp \
		$(addprefix configfile/,  error_pages.cpp server_data.cpp location.cpp utils.cpp)
OBJS = $(SRCS:.cpp=.o)
HEDERS =  Request.hpp Response.hpp Conf.hpp Response.hpp CGI.hpp
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
