CC       = gcc
BIN_DIR  = binary
SRC_DIR  = source
INC_DIR  = include
OBJ_DIR  = object

OBJECTS = data.o entry.o list.o

all: $(OBJECTS)

data.o: $(SRC_DIR) $(OBJ_DIR)
	$(CC) -c $(SRC_DIR)/data.c -I $(INC_DIR) -o $(OBJ_DIR)/data.o

entry.o: $(SRC_DIR) $(OBJ_DIR)
	$(CC) -c $(SRC_DIR)/entry.c -I $(INC_DIR) -o $(OBJ_DIR)/entry.o

list.o: $(SRC_DIR) $(OBJ_DIR)
	$(CC) -c $(SRC_DIR)/list.c -I $(INC_DIR) -o $(OBJ_DIR)/list.o

test.o: $(SRC_DIR) $(OBJ_DIR)
	$(CC) -c $(SRC_DIR)/test.c -I $(INC_DIR) -o $(OBJ_DIR)/test.o

clean: 
	rm -f ./$(OBJ_DIR)/*.o
	rm -f ./$(BIN_DIR)/*