TOPDIR=$(shell pwd)
INCDIR=$(TOPDIR)/include
COMPILE=gcc
OBJ=Robot
DEP= $(TOPDIR)/common/socket_process.c $(TOPDIR)/common/video_get_thread.c\
	$(TOPDIR)/common/info_conm_thread.c $(TOPDIR)/common/socket_process.c\
	$(TOPDIR)/common/video_send_thread.c $(TOPDIR)/common/move_thread.c\
	$(TOPDIR)/common/cmd.c $(TOPDIR)/common/env.c $(TOPDIR)/drivers/move.c\
	$(TOPDIR)/common/cmd_process.c main.c

all :
	$(COMPILE) -Wall $(DEP) -o $(OBJ) -lpthread -I $(INCDIR)
clean:
	rm -rf *.o Robot
