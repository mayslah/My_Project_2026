# ===============================================================

# Project: Operating Systems Simulation - Traffic on Graph

# ===============================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# ===============================================================

# Raylib

# ===============================================================

INC = -Isrc -I$(HOME)/raylib/include
LDFLAGS = -L$(HOME)/raylib/lib
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# ===============================================================

# Common Source Files

# ===============================================================

CORE_SRC = src/graph.c src/graph_io.c src/dijkstra.c

# ===============================================================

# Milestone 1

# ===============================================================

milestone1:
$(CC) $(CFLAGS) src/main_dijkstra.c $(CORE_SRC) $(INC) $(LDFLAGS) -o dijkstra $(LIBS)

# ===============================================================

# Milestone 2

# ===============================================================

milestone2:
$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ===============================================================

# Milestone 3

# ===============================================================

milestone3:
$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ===============================================================

# Milestone 4

# ===============================================================

milestone4:
$(CC) $(CFLAGS) src/main_m4.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ===============================================================

# Milestone 5

# ===============================================================

milestone5:
$(CC) $(CFLAGS) src/main_m5.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ===============================================================

# Milestone 6

# ===============================================================

milestone6:
$(CC) $(CFLAGS) src/main_m6.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ===============================================================

# Milestone 7 (Final)

# ===============================================================

milestone7:
$(CC) $(CFLAGS) 
src/main_m7.c 
src/queue.c 
src/scheduler.c 
$(CORE_SRC) 
$(INC) 
$(LDFLAGS) 
-o sim 
$(LIBS)

# ===============================================================

# Final

# ===============================================================

final:
$(CC) $(CFLAGS) 
src/main_m7.c 
src/queue.c 
src/scheduler.c 
$(CORE_SRC) 
$(INC) 
$(LDFLAGS) 
-o sim 
$(LIBS)

# ===============================================================

# Clean

# ===============================================================

clean:
rm -f dijkstra sim *.o

# ===============================================================

# PHONY

# ===============================================================

.PHONY: 
milestone1 
milestone2 
milestone3 
milestone4 
milestone5 
milestone6 
milestone7 
final 
clean
milestone4:
	$(CC) $(CFLAGS) src/main_m4.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ===============================================================
# Milestone 5
# ===============================================================
milestone5:
	$(CC) $(CFLAGS) src/main_m5.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ===============================================================
# Milestone 6
# ===============================================================
milestone6:
	$(CC) $(CFLAGS) src/main_m6.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ===============================================================
# Milestone 7
# ===============================================================
milestone7:
	$(CC) $(CFLAGS) src/main_m7.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ===============================================================
# Clean
# ===============================================================
clean:
	rm -f dijkstra sim *.o

# ===============================================================
# PHONY
# ===============================================================
.PHONY: milestone1 milestone2 milestone3 milestone4 milestone5 milestone6 milestone7 clean
