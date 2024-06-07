# PROGRAM INFOS
################################################################################
HOSTNAME	= $(shell hostname)
COMPILDATE	= "$(shell date)"

# COMMANDS
################################################################################
RM			= rm -f
RMRF		= rm -rf
CC			= c++
CD			= cd

# SOURCES
################################################################################
SRCS		= src/main.cpp \
			  src/server/Server.cpp \
		      src/server/exceptions.cpp \
			  src/channel/channel.cpp \
			  src/commands/join.cpp \
			  src/commands/part.cpp \
			  src/commands/invite.cpp \
			  src/commands/kick.cpp \
			  src/commands/topic.cpp \
			  src/commands/list.cpp \
			  src/commands/names.cpp \
			  src/user/User.cpp \
			  src/commands/cap.cpp \
			  src/commands/die.cpp \
			  src/commands/info.cpp \
			  src/commands/kill.cpp \
			  src/commands/mode.cpp \
			  src/commands/motd.cpp \
			  src/commands/nick.cpp \
			  src/commands/oper.cpp \
			  src/commands/pass.cpp \
			  src/commands/ping.cpp \
			  src/commands/pong.cpp \
			  src/commands/quit.cpp \
			  src/commands/service.cpp \
			  src/commands/time.cpp \
			  src/commands/user.cpp \
			  src/commands/privmsg.cpp \
			  src/commands/notice.cpp \
			  src/commands/version.cpp \
			  src/commands/who.cpp \
			  src/commands/whois.cpp \
			  src/utils/errors.cpp \
			  src/utils/parsing.cpp \
			  src/utils/welcome.cpp \
			  src/utils/command_utils.cpp \
			  src/parser/mask.cpp


OBJS		= $(SRCS:.cpp=.o)
BOTOBJS		= $(BOTSRCS:.cpp=.o)

# EXECUTABLES & LIBRARIES
################################################################################
NAME		= ircserv
BOTNAME		= ircbot

# DIRECTORIES
################################################################################
HEADERS		= -Iincludes -Isrcs/server

# FLAGS
################################################################################
CPPFLAGS		:= -Wall -Wextra -Werror -std=c++98

				   
ifeq ($(DEBUG), true)
	CPPFLAGS	+= -fsanitize=address -g3 -O0
endif

ifeq ($(VERBOSE), true)
	CPPFLAGS	+= -DVERBOSE
endif

# RULES
################################################################################
.cpp.o:
			$(CC) $(CPPFLAGS) -c $< -o $(<:.cpp=.o) $(HEADERS)

$(NAME):	$(OBJS)
			$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME) $(HEADERS)

$(BOTNAME):	$(BOTOBJS)
			$(CC) $(CPPFLAGS) $(BOTOBJS) -o $(BOTNAME) $(HEADERS)

all:		$(NAME)

clean:
			$(RM) $(OBJS)
			$(RM) $(BOTOBJS)

fclean:		clean
			$(RM) $(NAME)
			$(RM) $(BOTNAME)

re:			fclean all

bonus:		$(NAME) $(BOTNAME)

.PHONY:		all clean fclean cpp.o re bonus
