CC = gcc
CFLAGS = -Wall
EXEC_NAME = levitation
INCLUDES = -I/usr/include/SDL -I/usr/include/SDL_image
LIBS = -lSDL -lSDL_image 
OBJ_FILES = messages_CAN.o message_liste.o mySDLlib.o levitation.o
INSTALL_DIR = ./

all : $(EXEC_NAME)

clean :
	rm $(EXEC_NAME) $(OBJ_FILES)

$(EXEC_NAME) : $(OBJ_FILES)
	$(CC) -o $(EXEC_NAME) $(OBJ_FILES) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

%.o: %.cc
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

%.o: %.c
	gcc $(CFLAGS) $(INCLUDES) -o $@ -c $<

install :
	cp $(EXEC_NAME) $(INSTALL_DIR)
