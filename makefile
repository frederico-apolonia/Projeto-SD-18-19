CC       = gcc
BIN_DIR  = binary
SRC_DIR  = source
INC_DIR  = include
OBJ_DIR  = object
LIB_DIR  = library

all: data.o entry.o list.o table.o message.o table_skel.o network_server.o read_write.o\
	 network_client.o client_stub.o base64.o persistence_manager.o persistent_table.o\
	 server-lib.o client-lib.o

table-server:
	$(CC) -g -Wall -I $(INC_DIR) $(SRC_DIR)/table-server.c $(LIB_DIR)/server-lib.o -o $(BIN_DIR)/table-server

table-client:
	$(CC) -g -Wall -I $(INC_DIR) $(SRC_DIR)/table-client.c $(LIB_DIR)/client-lib.o -o $(BIN_DIR)/table-client

test_data: $(OBJ_DIR)/*.o
	$(CC) -g -Wall -I $(INC_DIR) $(SRC_DIR)/test_data.c $(OBJ_DIR)/data.o -o $(BIN_DIR)/test_data

test_entry: $(OBJ_DIR)/*.o
	$(CC) -g -Wall -I $(INC_DIR) $(SRC_DIR)/test_entry.c $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o -o \
	$(BIN_DIR)/test_entry

test_list: $(OBJ_DIR)/*.o
	$(CC) -g -Wall -I $(INC_DIR) $(SRC_DIR)/test_list.c $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/list.o \
	-o $(BIN_DIR)/test_list

test_table: $(OBJ_DIR)/*.o
	$(CC) -g -Wall -I $(INC_DIR) $(SRC_DIR)/test_table.c $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/list.o \
	$(OBJ_DIR)/table.o -o $(BIN_DIR)/test_table

test_message: $(OBJ_DIR)/*.o
	$(CC) -g -Wall -I $(INC_DIR) $(SRC_DIR)/test_message.c $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/message.o \
	$(OBJ_DIR)/base64.o -o $(BIN_DIR)/test_message

client-lib.o:
	ld -r $(OBJ_DIR)/client_stub.o $(OBJ_DIR)/network_client.o $(OBJ_DIR)/message.o $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o\
		  $(OBJ_DIR)/read_write.o $(OBJ_DIR)/base64.o -o $(LIB_DIR)/client-lib.o 
server-lib.o:
	ld -r $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/list.o $(OBJ_DIR)/table.o $(OBJ_DIR)/message.o $(OBJ_DIR)/table_skel.o\
		  $(OBJ_DIR)/network_server.o $(OBJ_DIR)/persistence_manager.o $(OBJ_DIR)/persistent_table.o $(OBJ_DIR)/base64.o\
		  $(OBJ_DIR)/read_write.o -o $(LIB_DIR)/server-lib.o


data.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/data.c -I $(INC_DIR) -o $(OBJ_DIR)/data.o

entry.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/entry.c -I $(INC_DIR) -o $(OBJ_DIR)/entry.o

list.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/list.c -I $(INC_DIR) -o $(OBJ_DIR)/list.o

table.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/table.c -I $(INC_DIR) -o $(OBJ_DIR)/table.o

message.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/message.c -I $(INC_DIR) -o $(OBJ_DIR)/message.o

client_stub.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/client_stub.c -I $(INC_DIR) -o $(OBJ_DIR)/client_stub.o

network_client.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/network_client.c -I $(INC_DIR) -o $(OBJ_DIR)/network_client.o

read_write.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/read_write.c -I $(INC_DIR) -o $(OBJ_DIR)/read_write.o

table_skel.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/table_skel.c -I $(INC_DIR) -o $(OBJ_DIR)/table_skel.o

network_server.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/network_server.c -I $(INC_DIR) -o $(OBJ_DIR)/network_server.o

persistence_manager.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/persistence_manager.c -I $(INC_DIR) -o $(OBJ_DIR)/persistence_manager.o

persistent_table.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/persistent_table.c -I $(INC_DIR) -o $(OBJ_DIR)/persistent_table.o

base64.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/base64.c -I $(INC_DIR) -o $(OBJ_DIR)/base64.o

clean:
	rm -f ./$(OBJ_DIR)/*.o
	rm -f ./$(BIN_DIR)/*
