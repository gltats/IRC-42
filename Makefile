# COMMANDS
################################################################################
RM			= rm -f
RMRF		= rm -rf
CC			= c++
CD			= cd
MKDIR		= mkdir
GCLONE		= git clone

# SOURCES
################################################################################
SRCS		= src/main.cpp \
			  src/server/Server.cpp \
		      src/server/exceptions.cpp \
			  src/channel/Channel.cpp \
			  src/commands/join.cpp \
			  src/commands/kick.cpp \
			  src/user/User.cpp \
			  src/commands/user.cpp \
			  src/commands/topic.cpp \
			  src/commands/join.cpp \
			  src/utils/command_utils.cpp \
			  src/utils/parsing.cpp \
			  src/utils/welcome.cpp \
			  src/utils/errors.cpp \


OBJS		= $(SRCS:.cpp=.o)

# EXECUTABLES & LIBRARIES
################################################################################
NAME		= irc

# DIRECTORIES
################################################################################
HEADERS		= -Iincludes -Isrcs/server

# FLAGS
################################################################################
CPPFLAGS		:= -Wall -Wextra -Werror -std=c++98

PROGRAMVAR		:= -DHOSTNAME=\"$(HOSTNAME)\" -DVERSION=\"$(VERSION)\" \
				   -DVCOMMENT=\"$(VCOMMENT)\" -DCOMPILDATE=\"$(COMPILDATE)\" \
				   -DSERVERINFO=\"$(SERVERINFO)\"
				   
ifeq ($(DEBUG), true)
	CPPFLAGS	+= -fsanitize=address -g3 -O0
endif

ifeq ($(VERBOSE), true)
	CPPFLAGS	+= -DVERBOSE
endif

# RULES
################################################################################
.cpp.o:
			$(CC) $(CPPFLAGS) -c $< -o $(<:.cpp=.o) $(HEADERS) $(PROGRAMVAR)

$(NAME):	$(OBJS)
			$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME) $(HEADERS)


all:		$(NAME)

clean:
			$(RM) $(OBJS)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all


.PHONY:		all clean fclean cpp.o re
