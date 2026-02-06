## ███████╗████████╗     ██████╗ ██╗   ██╗████████╗ ██████╗██╗  ██╗ █████╗ ██████╗
## ██╔════╝╚══██╔══╝     ██╔══██╗██║   ██║╚══██╔══╝██╔════╝██║  ██║██╔══██╗██╔══██╗
## █████╗     ██║        ██████╔╝██║   ██║   ██║   ██║     ███████║███████║██████╔╝
## ██╔══╝     ██║        ██╔═══╝ ██║   ██║   ██║   ██║     ██╔══██║██╔══██║██╔══██╗
## ██║        ██║███████╗██║     ╚██████╔╝   ██║   ╚██████╗██║  ██║██║  ██║██║  ██║
## ╚═╝        ╚═╝╚══════╝╚═╝      ╚═════╝    ╚═╝    ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
##
## <<Makefile>>

NAME	=	libcont
STATIC	=	$(NAME).a
SHARED	=	$(NAME).so

BUILD	=	normal

## COMPILER FLAGS

CC				=	gcc
cflags.common	=	-Wall -Wextra -Werror -Wpedantic -std=gnu2x -pedantic-errors -fPIC -I$(INCDIR)
cflags.debug	=	-g
cflags.fsan		=	$(cflags.debug) -fsanitize=address,undefined
cflags.normal	=	-s -O1
cflags.extra	=	
CFLAGS			=	$(cflags.common) $(cflags.$(BUILD)) $(cflags.extra)

## LD FLAGS

LD		=	ld
LDFLAGS	=	-shared

## DIRECTORIES

SRCDIR	=	src
OBJDIR	=	obj
INCDIR	=	inc

INSTALL_PATH	=	/usr

## SOURCE FILES

FILES	=	lc_darray.c


SRCS	=	$(addprefix $(SRCDIR)/, $(FILES))
OBJS	=	$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

## TARGETS

all: $(STATIC) $(SHARED)

static: $(STATIC)

shared: $(SHARED)

install: $(STATIC) $(SHARED)
	@printf "\e[1;38;5;85mTI42 >\e[m Installing ti42\n"
	@mkdir -p $(INSTALL_PATH)/lib
	@cp $(STATIC) $(SHARED) $(INSTALL_PATH)/lib/
	@printf "\e[1;38;5;85mTI42 >\e[m \e[1mDone!\e[m\n"

install-headers:
	@printf "\e[1;38;5;85mTI42 >\e[m Installing headers\n"
	@mkdir -p $(INSTALL_PATH)/include/ti42
	@cp $(STATIC) $(SHARED) $(INSTALL_PATH)/include/ti42/
	@printf "\e[1;38;5;85mTI42 >\e[m \e[1mDone!\e[m\n"

$(STATIC): $(OBJDIR) $(OBJS)
	@printf "\e[1;38;5;85mTI42 >\e[m Creating %s\n" $@
	@ar -crs $@ $(OBJS)
	@printf "\e[1;38;5;85mTI42 >\e[m \e[1mDone!\e[m\n"

$(SHARED): $(OBJDIR) $(OBJS)
	@printf "\e[1;38;5;85mTI42 >\e[m Linking %s\n" $@
	@$(LD) $(LDFLAGS) $(OBJS) -o $@
	@printf "\e[1;38;5;85mTI42 >\e[m \e[1mDone!\e[m\n"

$(OBJDIR):
	@printf "\e[1;38;5;85mTI42 >\e[m Creating objdir\n"
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@printf "\e[1;38;5;85mTI42 >\e[m Compiling %s\n" $@
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -rf $(OBJDIR)
	@rm -f $(STATIC)
	@rm -f $(SHARED)

re: fclean all

db:
	@printf "\e[1;38;5;85mTI42 >\e[m Creating compilation command database\n" $@
	@compiledb make --no-print-directory BUILD=$(BUILD) cflags.extra=$(cflags.extra) | sed -E '/^##.*\.\.\.$$|^[[:space:]]*$$/d'
	@printf "\e[1;38;5;85mTI42 >\e[m \e[1mDone!\e[m\n"

.PHONY: all static shared install install-headers clean fclean re db
