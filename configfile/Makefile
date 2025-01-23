NAME = config_file
CFLAGS = -g3 -std=c++98 -fsanitize=address

OBJS = $(SRCS:.cpp=.o)
SRCS = config_file.cpp error_pages.cpp server_data.cpp location.cpp
HEDERS = config_file.hpp error_pages.hpp server_data.hpp location.hpp


all : $(NAME)


run : re
	 ./$(NAME)
%.o : %.cpp 
	@c++ -c $(CFLAGS)  -o $@ $^


$(NAME) : $(OBJS) $(HEDERS)
	@c++ $(CFLAGS) $(OBJS) -o $(NAME)

clean : 
	@rm -f $(OBJS)
fclean : clean
	@rm -f $(NAME)
re : fclean all
