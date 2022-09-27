CFLAGS = -I./include

LDFLAGS = -L./extern
LDFLAGS += -lglfw3
LDFLAGS += -lGL
LDFLAGS += -lX11
LDFLAGS += -lpthread
LDFLAGS += -lXrandr
LDFLAGS += -lXi
LDFLAGS += -ldl


main: main.o glad.o
	g++ -o main main.o glad.o $(LDFLAGS)

glad.o: src/glad.c
	g++ $(CFLAGS) -c src/glad.c

main.o: src/main.cpp
	g++ $(CFLAGS) -c src/main.cpp 




