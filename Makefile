CXX = c++
CFLAGS = -g3 -Wall -Wextra -Werror -std=c++98 -fsanitize=address 

SRC_DIR := srcs
SUBDIRS := CGI Config Connection Errors Location Log Request Response Server
INC_DIR := includes

INCLUDES = -I "includes/"

CONFIGFILE = ./conf/config_file.yaml

SRCS := $(addprefix $(SRC_DIR)/, main.cpp \
	$(addprefix CGI/, cgi_data.cpp cgi_utils.cpp) \
	$(addprefix Config/, Conf.cpp utils.cpp Conf2.cpp) \
	$(addprefix Connection/, Connection.cpp) \
	$(addprefix Errors/, error_pages.cpp) \
	$(addprefix Location/, location.cpp) \
	$(addprefix Log/, log.cpp) \
	$(addprefix Request/, Request.cpp) \
	$(addprefix Response/, Response.cpp response_utils.cpp) \
	$(addprefix Server/, server_data.cpp))

OBJS = $(SRCS:.cpp=.o)


HEADERS := $(addprefix $(INC_DIR)/, \
	cgi_data.hpp \
	Conf.hpp \
	error_pages.hpp \
	log.hpp \
	Response.hpp \
	colors.hpp \
	Connection.hpp \
	location.hpp \
	Request.hpp \
	server_data.hpp)

NAME = webserv

all : $(NAME)

memory: all
	clear
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) $(CONFIGFILE)

valgrind: all
	clear
	valgrind ./$(NAME) $(CONFIGFILE)

hrun: re
	clear
	./$(NAME) $(CONFIGFILE)

run: all
	clear
	./$(NAME) $(CONFIGFILE)

%.o : %.cpp $(HEADERS)
	$(CXX) -c $(INCLUDES) $(CFLAGS)  -o $@ $<

$(NAME) : $(OBJS) $(HEADERS)
	$(CXX)   $(CFLAGS) $(OBJS) -o $(NAME)

clean : 
	rm -f $(OBJS)
fclean : clean
	rm -f $(NAME)
re : fclean all
