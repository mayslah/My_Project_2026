
#include <stdio.h>
#include <string.h>
#include "graph_io.h"

int read_graph_with_travelers(const char* filename, Graph* g, Traveler* travelers, int* num_travelers) {
    FILE* file = fopen(filename, "r");
    if (!file) return 0;

    char line[256];
    int num_nodes = 0, num_edges = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        if (sscanf(line, "%d %d", &num_nodes, &num_edges) == 2) break;
    }

    if (num_nodes <= 0 || num_nodes > MAX_NODES) {
        fclose(file);
        return 0;
    }

    init_graph(g, num_nodes);

    int edges_read = 0;
    while (edges_read < num_edges && fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        int u, v, w;
        if (sscanf(line, "%d %d %d", &u, &v, &w) == 3) {
            add_edge(g, u, v, w);
            edges_read++;
        }
    }

    *num_travelers = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        int n;
        if (sscanf(line, "%d", &n) == 1) {
            *num_travelers = n;
            break;
        }
    }

    if (*num_travelers > MAX_TRAVELERS) {
        *num_travelers = MAX_TRAVELERS;
    }

    int t = 0;
    while (t < *num_travelers && fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        if (sscanf(line, "%d %d", &travelers[t].src, &travelers[t].dst) == 2) {
            t++;
        }
    }

    *num_travelers = t;
    fclose(file);
    return 1;
}
