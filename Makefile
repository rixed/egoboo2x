SRC	= camera.c	\
	  char.c	\
	  enchant.c	\
	  game.c	\
	  graphic.c	\
	  input.c	\
	  menu.c	\
	  module.c	\
	  network.c	\
	  particle.c	\
	  passage.c	\
	  script.c	\
	  sound.c	\
	  lin-file.c	\
	  gltexture.c	\
	  mathstuff.c	\
	  graphicfan.c	\
	  graphicmad.c	\
	  graphicprt.c	\
	  configfile.c

GPUDIR = /home/rixed/src/gpu940
OBJS    = $(SRC:.c=.o)
NAME	= egoboo
CFLAGS += -W -Wall -D_LINUX -D_LITTLE_ENDIAN -ffast-math
CFLAGS += $(shell sdl-config --cflags)
LIBS = -lGL -lgpu940
LIBS += $(shell $(GP2XDEVDIR)/bin/sdl-config --static-libs) -static
#LIBS += $(shell sdl-config --libs)
INCDIR = -I$(GPUDIR)/include
INCDIR += -I$(GP2XDEVDIR)/include
LIBDIR = -L$(GPUDIR)/GL/.libs -L$(GPUDIR)/lib/.libs
LIBDIR += -L$(GP2XDEVDIR)/lib

all: $(NAME)

$(NAME): $(OBJS) $(SRC)
	$(CC) $(OBJS) $(CFLAGS) $(LDFLAGS) $(INCDIR) $(LIBDIR) $(LIBS) -o $(NAME)

.c.o:
	$(CC) $< $(CFLAGS) $(INCDIR) -c 

clean:
	rm -f $(NAME) *.o *~ depends.mak

put: $(NAME)
#	$(GP2XDEVDIR)/bin/gp2x-strip egoboo
	ncftpput -u root -p root gp2x /mnt/sd/egoboo2x egoboo

cscope:
	cscope -Rb *.c *.h

depends.mak: *.c
	$(CC) -M $(CFLAGS) $(INCDIR) $^ > $@

include depends.mak
