#define _POSIX_C_SOURCE 200809L

#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

```
int current_node;
int next_node;

TravelerStatus status;

float enter_time;
float move_start_time;
float move_duration;
float wait_start;
float total_wait;

Vector2 pos;
```

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
    DrawText(TextFormat("%d", i), node_pos[i].x - 6, node_pos[i].y - 8, 20, WHITE);

    if (queues[i].count > 0) {
        DrawText(
            TextFormat("Q:%d", queues[i].count),
            node_pos[i].x - 18,
            node_pos[i].y + 30,
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

    if (travelers[i].status == WAITING) c = ORANGE;
    if (travelers[i].status == FINISHED) c = GRAY;

    DrawCircleV(p, 13, c);
    DrawText(TextFormat("T%d", i + 1), p.x - 10, p.y - 30, 14, BLACK);
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
    travelers[i].enter_time = 0;
    travelers[i].move_start_time = 0;
    travelers[i].move_duration = 0;
    travelers[i].wait_start = 0;
    travelers[i].total_wait = 0;
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

                printf("[%s] T%d entered node %d\n",
                       scheduler_to_string(scheduler),
                       chosen + 1,
                       node);
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

                    printf("T%d finished | wait %.2f\n",
                           i + 1,
                           travelers[i].total_wait);
                } else {
                    int u = travelers[i].path[travelers[i].path_index];
                    int v = travelers[i].path[travelers[i].path_index + 1];
                    int w = g.weights[u][v];

                    occupant[node] = -1;

                    travelers[i].status = MOVING;
                    travelers[i].next_node = v;
                    travelers[i].move_start_time = now;
                    travelers[i].move_duration = w * STEP_TIME;

                    if (travelers[i].move_duration <= 0) {
                        travelers[i].move_duration = STEP_TIME;
                    }

                    printf("T%d moving %d -> %d\n", i + 1, u, v);
                }
            }
        } else if (travelers[i].status == MOVING) {
            int u = travelers[i].path[travelers[i].path_index];
            int v = travelers[i].path[travelers[i].path_index + 1];

            float progress = (now - travelers[i].move_start_time) / travelers[i].move_duration;

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

    DrawText("Orange = waiting | Green node = occupied", 20, 55, 18, DARKGRAY);

    draw_graph(&g, node_pos, occupant, queues);
    draw_travelers(travelers, num_travelers, node_pos);

    EndDrawing();
}

CloseWindow();
return 0;
```

}

typedef struct {
    int path[MAX_NODES];
    int path_count;
    int path_index;

    int current_node;
    int next_node;

    TravelerStatus status;
    int waiting_for;

    float enter_time;
    float move_start_time;
    float move_duration;

    Vector2 pos;

    float total_wait;
    float wait_start;

    pid_t pid;
} TravelerState;

static Color TRAVELER_COLORS[] = {
    {220, 80, 80, 255},
    {70, 130, 220, 255},
    {60, 180, 90, 255},
    {170, 80, 200, 255},
    {220, 160, 50, 255},
    {200, 90, 150, 255},
    {80, 190, 190, 255},
    {210, 130, 60, 255},
    {100, 160, 220, 255},
    {140, 100, 200, 255}
};

static void DrawArrow(Vector2 start, Vector2 end, Color color, float thick) {
    DrawLineEx(start, end, thick, color);

    float angle = atan2f(end.y - start.y, end.x - start.x);
    float size = 14.0f;

    Vector2 p1 = {
        end.x - size * cosf(angle - PI / 6),
        end.y - size * sinf(angle - PI / 6)
    };

    Vector2 p2 = {
        end.x - size * cosf(angle + PI / 6),
        end.y - size * sinf(angle + PI / 6)
    };

    DrawTriangle(end, p1, p2, color);
}

static void enqueue(NodeQueue* q, int traveler_id) {
    if (q->count < MAX_QUEUE) {
        q->items[q->count++] = traveler_id;
    }
}

static int remaining_cost(Graph* g, TravelerState* st) {
    int cost = 0;

    for (int i = st->path_index; i < st->path_count - 1; i++) {
        int u = st->path[i];
        int v = st->path[i + 1];
        int w = g->weights[u][v];

        if (w > 0) cost += w;
    }

    return cost;
}

static int choose_from_queue(NodeQueue* q, SchedulerType scheduler, TravelerState states[], Graph* g) {
    if (q->count == 0) return -1;

    int chosen_index = 0;

    if (scheduler == SCHD_SJF) {
        int best_cost = remaining_cost(g, &states[q->items[0]]);

        for (int i = 1; i < q->count; i++) {
            int traveler_id = q->items[i];
            int cost = remaining_cost(g, &states[traveler_id]);

            if (cost < best_cost) {
                best_cost = cost;
                chosen_index = i;
            }
        }
    }

    int chosen_traveler = q->items[chosen_index];

    for (int i = chosen_index; i < q->count - 1; i++) {
        q->items[i] = q->items[i + 1];
    }

    q->count--;

    return chosen_traveler;
}

static void print_usage(const char* program) {
    printf("Usage:\n");
    printf("%s -schd fcfs <file_name>\n", program);
    printf("%s -schd sjf <file_name>\n", program);
}

int main(int argc, char* argv[]) {
    if (argc != 4 || strcmp(argv[1], "-schd") != 0) {
        print_usage(argv[0]);
        return 1;
    }

    SchedulerType scheduler;

    if (strcmp(argv[2], "fcfs") == 0) {
        scheduler = SCHD_FCFS;
    } else if (strcmp(argv[2], "sjf") == 0) {
        scheduler = SCHD_SJF;
    } else {
        print_usage(argv[0]);
        return 1;
    }

    const char* filename = argv[3];

    Graph g;
    Traveler travelers[MAX_TRAVELERS];
    int num_travelers = 0;

    if (!read_graph_with_travelers(filename, &g, travelers, &num_travelers)) {
        printf("Error: could not read input file.\n");
        return 1;
    }

    TravelerState states[MAX_TRAVELERS];
    NodeQueue queues[MAX_NODES];
    int node_occupant[MAX_NODES];

    for (int i = 0; i < g.num_nodes; i++) {
        queues[i].count = 0;
        node_occupant[i] = -1;
    }

    for (int i = 0; i < num_travelers; i++) {
        states[i].path_count = compute_path(&g, travelers[i].src, travelers[i].dst, states[i].path);

        if (states[i].path_count == 0) {
            printf("Traveler %d: No path found from %d to %d\n", i, travelers[i].src, travelers[i].dst);
            states[i].status = STATE_FINISHED;
            states[i].current_node = travelers[i].src;
            states[i].waiting_for = -1;
            states[i].pid = -1;
            continue;
        }

        states[i].path_index = 0;
        states[i].current_node = states[i].path[0];
        states[i].next_node = -1;
        states[i].status = STATE_WAITING;
        states[i].waiting_for = states[i].current_node;
        states[i].enter_time = 0.0f;
        states[i].move_start_time = 0.0f;
        states[i].move_duration = 0.0f;
        states[i].total_wait = 0.0f;
        states[i].wait_start = 0.0f;
        states[i].pid = -1;

        enqueue(&queues[states[i].current_node], i);
    }

    for (int i = 0; i < num_travelers; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            while (1) {
                pause();
            }
        } else if (pid > 0) {
            states[i].pid = pid;
        }
    }

    const int screenWidth = 900;
    const int screenHeight = 650;

    InitWindow(screenWidth, screenHeight, "Milestone 7 - Scheduling Algorithms");
    SetTargetFPS(60);

    Vector2 node_pos[MAX_NODES];

    for (int i = 0; i < g.num_nodes; i++) {
        node_pos[i] = (Vector2) {
            screenWidth / 2 + 230 * cosf(i * 2 * PI / g.num_nodes),
            screenHeight / 2 + 210 * sinf(i * 2 * PI / g.num_nodes)
        };
    }

    while (!WindowShouldClose()) {
        float now = (float)GetTime();

        for (int n = 0; n < g.num_nodes; n++) {
            if (node_occupant[n] == -1 && queues[n].count > 0) {
                int chosen = choose_from_queue(&queues[n], scheduler, states, &g);

                if (chosen != -1) {
                    node_occupant[n] = chosen;
                    states[chosen].status = STATE_IN_NODE;
                    states[chosen].waiting_for = -1;
                    states[chosen].current_node = n;
                    states[chosen].enter_time = now;
                    states[chosen].total_wait += now - states[chosen].wait_start;

                    printf("[scheduler=%s] traveler %d entered node %d\n",
                           scheduler == SCHD_FCFS ? "fcfs" : "sjf",
                           chosen,
                           n);
                }
            }
        }

        for (int i = 0; i < num_travelers; i++) {
            if (states[i].status == STATE_IN_NODE) {
                int node = states[i].current_node;

                if (now - states[i].enter_time >= PAUSE_TIME) {
                    if (states[i].path_index >= states[i].path_count - 1) {
                        states[i].status = STATE_FINISHED;
                        node_occupant[node] = -1;

                        printf("traveler %d finished | total wait = %.2f seconds\n",
                               i,
                               states[i].total_wait);
                    } else {
                        int u = states[i].path[states[i].path_index];
                        int v = states[i].path[states[i].path_index + 1];
                        int w = g.weights[u][v];

                        states[i].next_node = v;
                        states[i].status = STATE_MOVING;
                        states[i].move_start_time = now;
                        states[i].move_duration = w * STEP_TIME;

                        if (states[i].move_duration <= 0) {
                            states[i].move_duration = STEP_TIME;
                        }

                        node_occupant[node] = -1;

                        printf("traveler %d leaving node %d -> %d\n", i, u, v);
                    }
                }
            } else if (states[i].status == STATE_MOVING) {
                int u = states[i].path[states[i].path_index];
                int v = states[i].path[states[i].path_index + 1];

                float progress = (now - states[i].move_start_time) / states[i].move_duration;

                if (progress >= 1.0f) {
                    states[i].path_index++;
                    states[i].current_node = v;
                    states[i].status = STATE_WAITING;
                    states[i].waiting_for = v;
                    states[i].wait_start = now;

                    enqueue(&queues[v], i);

                    printf("traveler %d waiting for node %d\n", i, v);
                } else {
                    Vector2 start = node_pos[u];
                    Vector2 end = node_pos[v];

                    states[i].pos.x = start.x + (end.x - start.x) * progress;
                    states[i].pos.y = start.y + (end.y - start.y) * progress;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText(TextFormat("Milestone 7 Scheduler: %s",
                            scheduler == SCHD_FCFS ? "FCFS" : "SJF"),
                 20, 15, 24, DARKBLUE);

        DrawText("Orange = waiting outside node", 20, 45, 18, ORANGE);
        DrawText("Colored = inside node / moving", 20, 70, 18, DARKGRAY);

        for (int i = 0; i < g.num_nodes; i++) {
            for (int j = 0; j < g.num_nodes; j++) {
                if (g.weights[i][j] != -1) {
                    DrawArrow(node_pos[i], node_pos[j], DARKGRAY, 2.0f);

                    DrawText(TextFormat("%d", g.weights[i][j]),
                             (int)((node_pos[i].x + node_pos[j].x) / 2) + 5,
                             (int)((node_pos[i].y + node_pos[j].y) / 2) + 5,
                             16,
                             MAROON);
                }
            }
        }

        for (int i = 0; i < g.num_nodes; i++) {
            Color node_color = (node_occupant[i] == -1) ? DARKBLUE : DARKGREEN;

            DrawCircleV(node_pos[i], 24, node_color);
            DrawCircleLines((int)node_pos[i].x, (int)node_pos[i].y, 24, WHITE);

            DrawText(TextFormat("%d", i),
                     (int)node_pos[i].x - 6,
                     (int)node_pos[i].y - 9,
                     20,
                     WHITE);

            if (queues[i].count > 0) {
                DrawText(TextFormat("Q:%d", queues[i].count),
                         (int)node_pos[i].x - 18,
                         (int)node_pos[i].y + 30,
                         16,
                         ORANGE);
            }
        }

        for (int i = 0; i < num_travelers; i++) {
            if (states[i].status == STATE_FINISHED) {
                Vector2 pos = node_pos[states[i].current_node];
                pos.x += (float)((i % 3) - 1) * 12;
                pos.y += (float)(i / 3) * 12;

                DrawCircleV(pos, 12, GRAY);
                DrawText(TextFormat("T%d", i), (int)pos.x - 9, (int)pos.y - 30, 14, DARKGRAY);
            } else if (states[i].status == STATE_WAITING) {
                int node = states[i].waiting_for;
                Vector2 pos = node_pos[node];

                pos.x += 40 + (i % 3) * 18;
                pos.y += -30 + (i / 3) * 18;

                DrawCircleV(pos, 12, ORANGE);
                DrawCircleLines((int)pos.x, (int)pos.y, 12, WHITE);
                DrawText(TextFormat("T%d", i), (int)pos.x - 9, (int)pos.y - 28, 14, DARKGRAY);
            } else {
                Vector2 pos;

                if (states[i].status == STATE_MOVING) {
                    pos = states[i].pos;
                } else {
                    pos = node_pos[states[i].current_node];
                }

                pos.x += (float)((i % 3) - 1) * 10;
                pos.y += (float)(i / 3) * 10;

                DrawCircleV(pos, 13, TRAVELER_COLORS[i % 10]);
                DrawCircleLines((int)pos.x, (int)pos.y, 13, WHITE);
                DrawText(TextFormat("T%d", i), (int)pos.x - 9, (int)pos.y - 30, 14, DARKGRAY);
            }
        }

        int lx = screenWidth - 230;
        int ly = 20;

        DrawRectangle(lx - 10, ly - 10, 220, 35 + num_travelers * 25, (Color){235, 235, 245, 230});
        DrawText("Travelers", lx, ly, 18, DARKBLUE);

        for (int i = 0; i < num_travelers; i++) {
            DrawCircle(lx + 10, ly + 35 + i * 25, 8, TRAVELER_COLORS[i % 10]);
            DrawText(TextFormat("T%d: %d -> %d | wait %.1f",
                                i,
                                travelers[i].src,
                                travelers[i].dst,
                                states[i].total_wait),
                     lx + 25,
                     ly + 27 + i * 25,
                     15,
                     DARKGRAY);
        }

        bool all_done = true;

        for (int i = 0; i < num_travelers; i++) {
            if (states[i].status != STATE_FINISHED) {
                all_done = false;
                break;
            }
        }

        if (all_done) {
            DrawText("ALL TRAVELERS DONE!", 310, 50, 26, DARKGREEN);
            DrawText("Compare total wait times between FCFS and SJF", 250, 82, 18, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();

    for (int i = 0; i < num_travelers; i++) {
        if (states[i].pid > 0) {
            kill(states[i].pid, SIGTERM);
            waitpid(states[i].pid, NULL, 0);
        }
    }

    return 0;
}
