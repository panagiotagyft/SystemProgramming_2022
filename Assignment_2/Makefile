MAKE-DIR = $(PWD)

SRC := ./src
INCLUDE := $(MAKE-DIR)/include

BLD = ./obj
SERVER_EXE = $(BLD)/dataServer
CLIENT_EXE = $(BLD)/remoteClient

DATA_SERVER = $(SRC)/dataServer
REMOTE_CLIENT = $(SRC)/remoteClient

CC := gcc
CFLAGS := -Wall -I $(INCLUDE)

DATASERVER_PROGR = ./dataServer
REMOTECLIENT_PROGR = ./remoteClient

SERVER_OBJS = $(DATA_SERVER)/dataServer.o $(DATA_SERVER)/dataServer_Implementation.o $(DATA_SERVER)/DoublyList_Implementation.o
CLIENT_OBJS = $(REMOTE_CLIENT)/remoteClient.o

all: $(DATASERVER_PROGR) $(REMOTECLIENT_PROGR)
		mkdir -p $(SERVER_EXE)
		mkdir -p $(CLIENT_EXE)
		mv -f $(SERVER_OBJS) $(SERVER_EXE)
		mv -f $(CLIENT_OBJS) $(CLIENT_EXE)

$(DATASERVER_PROGR): $(SERVER_OBJS)
		$(CC) -pthread $(CFLAGS) $(SERVER_OBJS) -o $(DATASERVER_PROGR) 

$(REMOTECLIENT_PROGR): $(CLIENT_OBJS)
		$(CC) $(CFLAGS) $(CLIENT_OBJS) -o $(REMOTECLIENT_PROGR) 


clean:
	rm -f $(SERVER_EXE)
	rm -f $(CLIENT_EXE)
	rm -rf $(BLD)

