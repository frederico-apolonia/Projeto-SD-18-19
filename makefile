CC       = gcc
BIN_DIR  = binary
SRC_DIR  = source
INC_DIR  = include
OBJ_DIR  = object

all: data.o entry.o list.o ; make test_data

test_data: $(OBJ_DIR)/*.o
	$(CC) -Wall -I $(INC_DIR) $(SRC_DIR)/test_data.c $(OBJ_DIR)/* -o $(BIN_DIR)/test_data

test_entry: $(OBJ_DIR)/*.o
	$(CC) -Wall -I $(INC_DIR) $(OBJ_DIR)/test_entry.c $(OBJ_DIR)/* -o $(BIN_DIR)/test_entry

test_list: $(OBJ_DIR)/*.o
	$(CC) -Wall -I $(INC_DIR) $(OBJ_DIR)/test_list.c $(OBJ_DIR)/* -o $(BIN_DIR)/test_list

test_table: $(OBJ_DIR)/*.o
	$(CC) -Wall -I $(INC_DIR) $(OBJ_DIR)/test_table.c $(OBJ_DIR)/* -o $(BIN_DIR)/test_table


data.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -c $(SRC_DIR)/data.c -I $(INC_DIR) -o $(OBJ_DIR)/data.o

entry.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -c $(SRC_DIR)/entry.c -I $(INC_DIR) -o $(OBJ_DIR)/entry.o

list.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -c $(SRC_DIR)/list.c -I $(INC_DIR) -o $(OBJ_DIR)/list.o

table.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -c $(SRC_DIR)/table.c -I $(INC_DIR) -o $(OBJ_DIR)/table.o

clean:
	rm -f ./$(OBJ_DIR)/*.o
	rm -f ./$(BIN_DIR)/*