#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

void find_shortest_path(Graph* g, int start, int end);
int compute_path(Graph* g, int start, int end, int* path_out);

extern int final_path[MAX_NODES];
extern int final_path_count;

#endif
