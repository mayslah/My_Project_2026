#ifndef GRAPH_IO_H
#define GRAPH_IO_H

#include "graph.h"
#include "travelers.h"

int read_graph_from_file(const char* filename, Graph* g, int* start_node, int* end_node);
int read_graph_with_travelers(const char* filename, Graph* g, Traveler* travelers, int* num_travelers);

#endif
