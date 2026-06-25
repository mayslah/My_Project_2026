#include "dijkstra.h"
int compute_path(Graph* g, int start, int end, int* path_out) {
    int dist[MAX_NODES], prev[MAX_NODES], visited[MAX_NODES];

    for (int i = 0; i < MAX_NODES; i++) {
        dist[i] = INF;
        prev[i] = -1;
        visited[i] = 0;
    }

    if (start == end) {
        path_out[0] = start;
        return 1;
    }

    dist[start] = 0;

    for (int i = 0; i < g->num_nodes; i++) {
        int u = -1;

        for (int j = 0; j < g->num_nodes; j++) {
            if (!visited[j] && (u == -1 || dist[j] < dist[u])) {
                u = j;
            }
        }

        if (u == -1 || dist[u] == INF) break;

        visited[u] = 1;

        for (int v = 0; v < g->num_nodes; v++) {
            if (g->weights[u][v] != -1) {
                int alt = dist[u] + g->weights[u][v];
                if (alt < dist[v]) {
                    dist[v] = alt;
                    prev[v] = u;
                }
            }
        }
    }

    if (dist[end] == INF) return 0;

    int temp[MAX_NODES];
    int count = 0;
    int curr = end;

    while (curr != -1) {
        temp[count++] = curr;
        curr = prev[curr];
    }

    for (int i = 0; i < count; i++) {
        path_out[i] = temp[count - 1 - i];
    }

    return count;
}
                if (alt < dist[v]) {
                    dist[v] = alt;
                    prev[v] = u;
                }
            }
        }
    }

    if (dist[end] == INF) return 0;

    int temp[MAX_NODES];
    int count = 0;
    int curr = end;

    while (curr != -1) {
        temp[count++] = curr;
        curr = prev[curr];
    }

    for (int i = 0; i < count; i++) {
        path_out[i] = temp[count - 1 - i];
    }

    return count;
}
