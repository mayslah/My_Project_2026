#define _POSIX_C_SOURCE 200809L

#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "graph_io.h"
#include "dijkstra.h"
#include "travelers.h"
#include "queue.h"
#include "scheduler.h"

#define PAUSE_TIME 1.0f
#define STEP_TIME 0.3f

typedef enum {
WAITING,
IN_NODE,
MOVING,
FINISHED
} TravelerStatus;

typedef struct {
int path[MAX_NODES];
int path_count;
int path_index;
int current_node;
int next_node;
TravelerStatus status;
float enter_time;
float move_start_time;
float move_duration;
float wait_start;
float total_wait;
Vector2 pos;
} TravelerState;

static Color colors[] = {
RED, BLUE, GREEN, PURPLE, ORANGE,
PINK, SKYBLUE, GOLD, MAROON, DARKGREEN
};

static void usage(const char* prog) {
printf("Usage:\n");
printf("%s -schd fcfs <file_name>\n", prog);
printf("%s -schd sjf <file_name>\n", prog);
}

static void draw_arrow(Vector2 start, Vector2 end) {
DrawLineV(start, end, DARKGRAY);

```
float angle = atan2f(end.y - start.y, end.x - start.x);
float size = 12.0f;

Vector2 p1 = {
    end.x - size * cosf(angle - PI / 6),
    end.y - size * sinf(angle - PI / 6)
};

Vector2 p2 = {
    end.x - size * cosf(angle + PI / 6),
    end.y - size * sinf(angle + PI / 6)
};

DrawTriangle(end, p1, p2, DARKGRAY);
```

}

static void create_positions(Vector2 pos[], int n) {
for (int i = 0; i < n; i++) {
pos[i] = (Vector2){
450 + 230 * cosf(i * 2 * PI / n),
330 + 210 * sinf(i * 2 * PI / n)
};
}
}

static void draw_graph(Graph* g, Vector2 node_pos[], int occupant[], NodeQueue queues[]) {
for (int i = 0; i < g->num_nodes; i++) {
for (int j = 0; j < g->num_nodes; j++) {
if (g->weights[i][j] != -1) {
draw_arrow(node_pos[i], node_pos[j]);

```
            DrawText(
                TextFormat("%d", g->weights[i][j]),
                (int)((node_pos[i].x + node_pos[j].x) / 2) + 5,
                (int)((node_pos[i].y + node_pos[j].y) / 2) + 5,
                15,
                MAROON
            );
        }
    }
}

for (int i = 0; i < g->num_nodes; i++) {
    Color node_color = occupant[i] == -1 ? DARKBLUE : DARKGREEN;

    DrawCircleV(node_pos[i], 24, node_color);
    DrawText(
        TextFormat("%d", i),
        (int)node_pos[i].x - 6,
        (int)node_pos[i].y - 8,
        20,
        WHITE
    );

    if (queues[i].count > 0) {
        DrawText(
            TextFormat("Q:%d", queues[i].count),
            (int)node_pos[i].x - 18,
            (int)node_pos[i].y + 30,
            16,
            ORANGE
        );
    }
}
```

}

static void draw_travelers(TravelerState travelers[], int count, Vector2 node_pos[]) {
for (int i = 0; i < count; i++) {
Vector2 p;

```
    if (travelers[i].status == MOVING) {
        p = travelers[i].pos;
    } else {
        p = node_pos[travelers[i].current_node];
    }

    p.x += (float)((i % 3) - 1) * 14;
    p.y += (float)(i / 3) * 14;

    Color c = colors[i % 10];

    if (travelers[i].status == WAITING) {
        c = ORANGE;
    }

    if (travelers[i].status == FINISHED) {
        c = GRAY;
    }

    DrawCircleV(p, 13, c);
    DrawText(
        TextFormat("T%d", i + 1),
        (int)p.x - 10,
        (int)p.y - 30,
        14,
        BLACK
    );
}
```

}

static void build_scheduler_view(
TravelerState travelers[],
SchedulerTravelerView view[],
int count
) {
for (int i = 0; i < count; i++) {
view[i].path_count = travelers[i].path_count;
view[i].path_index = travelers[i].path_index;

```
    for (int j = 0; j < travelers[i].path_count; j++) {
        view[i].path[j] = travelers[i].path[j];
    }
}
```

}

int main(int argc, char* argv[]) {
if (argc != 4 || strcmp(argv[1], "-schd") != 0) {
usage(argv[0]);
return 1;
}

```
SchedulerType scheduler = parse_scheduler_name(argv[2]);
const char* filename = argv[3];

Graph g;
Traveler input_travelers[MAX_TRAVELERS];
int num_travelers = 0;

if (!read_graph_with_travelers(filename, &g, input_travelers, &num_travelers)) {
    printf("Error reading input file.\n");
    return 1;
}

TravelerState travelers[MAX_TRAVELERS];
NodeQueue queues[MAX_NODES];
int occupant[MAX_NODES];

for (int i = 0; i < g.num_nodes; i++) {
    queue_init(&queues[i]);
    occupant[i] = -1;
}

Vector2 node_pos[MAX_NODES];
create_positions(node_pos, g.num_nodes);

for (int i = 0; i < num_travelers; i++) {
    travelers[i].path_count = compute_path(
        &g,
        input_travelers[i].src,
        input_travelers[i].dst,
        travelers[i].path
    );

    travelers[i].path_index = 0;
    travelers[i].current_node = input_travelers[i].src;
    travelers[i].next_node = -1;
    travelers[i].status = WAITING;
    travelers[i].enter_time = 0.0f;
    travelers[i].move_start_time = 0.0f;
    travelers[i].move_duration = 0.0f;
    travelers[i].wait_start = 0.0f;
    travelers[i].total_wait = 0.0f;
    travelers[i].pos = node_pos[input_travelers[i].src];

    if (travelers[i].path_count > 0) {
        queue_push(&queues[travelers[i].current_node], i);
    } else {
        travelers[i].status = FINISHED;
    }
}

InitWindow(900, 650, "Milestone 7 - Scheduling");
SetTargetFPS(60);

while (!WindowShouldClose()) {
    float now = (float)GetTime();

    SchedulerTravelerView view[MAX_TRAVELERS];
    build_scheduler_view(travelers, view, num_travelers);

    for (int node = 0; node < g.num_nodes; node++) {
        if (occupant[node] == -1 && !queue_is_empty(&queues[node])) {
            int chosen = choose_traveler_from_queue(
                &queues[node],
                scheduler,
                view,
                &g
            );

            if (chosen != -1) {
                occupant[node] = chosen;
                travelers[chosen].status = IN_NODE;
                travelers[chosen].current_node = node;
                travelers[chosen].enter_time = now;
                travelers[chosen].total_wait += now - travelers[chosen].wait_start;

                printf(
                    "[%s] T%d entered node %d\n",
                    scheduler_to_string(scheduler),
                    chosen + 1,
                    node
                );
            }
        }
    }

    for (int i = 0; i < num_travelers; i++) {
        if (travelers[i].status == IN_NODE) {
            int node = travelers[i].current_node;

            if (now - travelers[i].enter_time >= PAUSE_TIME) {
                if (travelers[i].path_index >= travelers[i].path_count - 1) {
                    travelers[i].status = FINISHED;
                    occupant[node] = -1;

                    printf(
                        "T%d finished | wait %.2f\n",
                        i + 1,
                        travelers[i].total_wait
                    );
                } else {
                    int u = travelers[i].path[travelers[i].path_index];
                    int v = travelers[i].path[travelers[i].path_index + 1];
                    int w = g.weights[u][v];

                    occupant[node] = -1;
                    travelers[i].status = MOVING;
                    travelers[i].next_node = v;
                    travelers[i].move_start_time = now;
                    travelers[i].move_duration = w * STEP_TIME;

                    if (travelers[i].move_duration <= 0.0f) {
                        travelers[i].move_duration = STEP_TIME;
                    }

                    printf("T%d moving %d -> %d\n", i + 1, u, v);
                }
            }
        } else if (travelers[i].status == MOVING) {
            int u = travelers[i].path[travelers[i].path_index];
            int v = travelers[i].path[travelers[i].path_index + 1];

            float progress =
                (now - travelers[i].move_start_time) / travelers[i].move_duration;

            if (progress >= 1.0f) {
                travelers[i].path_index++;
                travelers[i].current_node = v;
                travelers[i].pos = node_pos[v];
                travelers[i].status = WAITING;
                travelers[i].wait_start = now;

                queue_push(&queues[v], i);

                printf("T%d waiting for node %d\n", i + 1, v);
            } else {
                Vector2 a = node_pos[u];
                Vector2 b = node_pos[v];

                travelers[i].pos.x = a.x + (b.x - a.x) * progress;
                travelers[i].pos.y = a.y + (b.y - a.y) * progress;
            }
        }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText(
        TextFormat("Scheduler: %s", scheduler_to_string(scheduler)),
        20,
        20,
        26,
        DARKBLUE
    );

    DrawText(
        "Orange = waiting | Green node = occupied",
        20,
        55,
        18,
        DARKGRAY
    );

    draw_graph(&g, node_pos, occupant, queues);
    draw_travelers(travelers, num_travelers, node_pos);

    EndDrawing();
}

CloseWindow();
return 0;
```

}
