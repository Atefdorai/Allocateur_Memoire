CC = gcc  # C compiler
CFLAGS = -fPIC -Wall -Wextra -O2 # -g  :  C flags
LDFLAGS = -shared   # linking flags
RM = rm -f   # rm command

TARGET_LIB = liballoc.so  # target lib

SRCS = main.c   # source files
OBJS = $(SRCS:.c=.o)

.PHONY: all

all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} .L -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS)  $< >$@   # -MM

include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)
