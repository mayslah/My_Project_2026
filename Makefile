# ===============================================================
# Project: Operating Systems Simulation - Traffic on Graph
# ===============================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

INC = -Isrc -I$(HOME)/raylib/include
LDFLAGS = -L$(HOME)/raylib/lib
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

CORE_SRC = src/graph.c src/graph_io.c src/dijkstra.c
M7_SRC = src/queue.c src/scheduler.c

milestone1:
	$(CC) $(CFLAGS) src/main_dijkstra.c $(CORE_SRC) $(INC) $(LDFLAGS) -o dijkstra $(LIBS)

milestone2:
	$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

milestone3:
	$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

milestone4:
	$(CC) $(CFLAGS) src/main_m4.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

milestone5:
	$(CC) $(CFLAGS) src/main_m5.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

milestone6:
	$(CC) $(CFLAGS) src/main_m6.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

milestone7:
	$(CC) $(CFLAGS) src/main_m7.c $(CORE_SRC) $(M7_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

final:
	$(CC) $(CFLAGS) src/main_m7.c $(CORE_SRC) $(M7_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

clean:
	rm -f dijkstra sim *.o

.PHONY: milestone1 milestone2 milestone3 milestone4 milestone5 milestone6 milestone7 final clean
