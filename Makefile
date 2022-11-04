SRCS			= src/webserv.cpp src/parse.cpp src/http.cpp src/socket.cpp  src/main.cpp  src/server.cpp
OBJS			= $(SRCS:.cpp=.o)

CXX				= clang++
RM				= rm -f
INCL			= src/webserv.hpp src/parse.hpp src/http.hpp src/socket.hpp src/server.hpp 
CXXFLAGS		= -Wall -Wextra -Werror -std=c++98

NAME			= webserv

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CXX) $(CXXFLAGS)  -o $(NAME)  $(OBJS)

clean:
				$(RM) $(OBJS)
				$(RM) www/logs/* www/uploads/*

fclean:			clean
				$(RM) $(NAME)

re:				fclean $(NAME)



.PHONY:			all clean fclean re test