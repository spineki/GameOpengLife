CC = g++

CFLAGS = -I./include

LDFLAGS = -L./extern
LDFLAGS += -lglfw3
LDFLAGS += -lGL
LDFLAGS += -lX11
LDFLAGS += -lpthread
LDFLAGS += -lXrandr
LDFLAGS += -lXi
LDFLAGS += -ldl

run: main
	./main

main: main.o glad.o
	$(CC) -o main main.o glad.o $(LDFLAGS)

glad.o: src/glad.c
	$(CC) $(CFLAGS) -c src/glad.c

main.o: src/main.cpp
	$(CC) $(CFLAGS) -c src/main.cpp 




