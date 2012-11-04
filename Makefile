TARGET=pong
OBJS=pong.o
SRC= $(OBJS:.o=.c)
LIBS= -lGLU -lglut

CC=gcc
LD=$(CC)
CFLAGS=

$(TARGET):$(OBJS)
	$(LD) -o $@ $(LIBS) $^

$(OBJS):$(SRC)
	$(CC) -c $(CFLAGS) $<


clean:
	rm -Rf $(TARGET) $(OBJS)
