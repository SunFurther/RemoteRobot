TOPDIR=$(shell pwd)
INCDIR=$(TOPDIR)/include
COMPILE=gcc
OBJ=Robot
DEP= $(TOPDIR)/common/socket_thread.c $(TOPDIR)/common/video_get_thread.c\
	$(TOPDIR)/common/video_send_thread.c $(TOPDIR)/common/move_thread.c\
	$(TOPDIR)/common/cmd.c $(TOPDIR)/common/env.c $(TOPDIR)/drivers/move.c\
	main.c

all :
	$(COMPILE) -Wall $(DEP) -o $(OBJ) -lpthread -I $(INCDIR)
clean:
	rm -rf *.o Robot
