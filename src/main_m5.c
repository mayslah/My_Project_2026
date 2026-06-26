#define _POSIX_C_SOURCE 200809L

#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "graph.h"
#include "graph_io.h"
#include "dijkstra.h"
#include "travelers.h"
#include "ipc_types.h"

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
{140, 100, 200, 255},
};

static void DrawArrow(Vector2 start, Vector2 end, Color color, float thick) {
DrawLineEx(start, end, thick, color);


float angle = atan2f(end.y - start.y, end.x - start.x);
float sz = 14.0f;

Vector2 p1 = {
    end.x - sz * cosf(angle - PI / 6),
    end.y - sz * sinf(angle - PI / 6)
};

Vector2 p2 = {
    end.x - sz * cosf(angle + PI / 6),
    end.y - sz * sinf(angle + PI / 6)
};

DrawTriangle(end, p1, p2, color);


}

typedef struct {
pid_t pid;
int read_fd;
int ack_write_fd;
int current_node;
bool finished;
} TravelerState;

int main(int argc, char* argv[]) {
if (argc < 2) {
printf("Usage: %s <input_file>\n", argv[0]);
return 1;
}

```
Graph g;
Traveler travelers[MAX_TRAVELERS];
int num_travelers = 0;

if (!read_graph_with_travelers(argv[1], &g, travelers, &num_travelers)) {
    printf("Error: Could not read file.\n");
    return 1;
}

/*
 * pipes[i]: child -> parent
 * ack_pipes[i]: parent -> child
 *
 * This implements the exam requirement:
 * The child sends a message to the parent and waits for an ACK
 * before continuing to the next node.
 */
int pipes[MAX_TRAVELERS][2];
int ack_pipes[MAX_TRAVELERS][2];

for (int i = 0; i < num_travelers; i++) {
    if (pipe(pipes[i]) == -1) {
        perror("pipe");
        return 1;
    }

    if (pipe(ack_pipes[i]) == -1) {
        perror("ack pipe");
        return 1;
    }
}

pid_t pids[MAX_TRAVELERS];

for (int i = 0; i < num_travelers; i++) {
    pid_t pid = fork();

    if (pid == 0) {
        /*
         * Child process:
         * - writes to pipes[i][1]
         * - reads ACK from ack_pipes[i][0]
         */
        for (int j = 0; j < num_travelers; j++) {
            close(pipes[j][0]);
            close(ack_pipes[j][1]);

            if (j != i) {
                close(pipes[j][1]);
                close(ack_pipes[j][0]);
            }
        }

        int wfd = pipes[i][1];
        int ack_read_fd = ack_pipes[i][0];

        int path[MAX_NODES];
        int path_count = compute_path(&g, travelers[i].src, travelers[i].dst, path);

        TravelerMsg msg;
        msg.pid = getpid();

        for (int step = 0; step < path_count; step++) {
            msg.msg_type = MSG_ARRIVED;
            msg.node = path[step];
            msg.next_node = (step < path_count - 1) ? path[step + 1] : -1;

            write(wfd, &msg, sizeof(msg));

            /*
             * New exam change:
             * If there is a next node, wait for approval from parent.
             * The content of the ACK does not matter.
             */
            if (msg.next_node != -1) {
                char ack;
                read(ack_read_fd, &ack, sizeof(ack));
            }

            if (step < path_count - 1) {
                sleep(1);
                usleep(g.weights[path[step]][path[step + 1]] * 300000);
            }
        }

        msg.msg_type = MSG_FINISHED;
        msg.node = path[path_count - 1];
        msg.next_node = -1;

        write(wfd, &msg, sizeof(msg));

        close(wfd);
        close(ack_read_fd);
        exit(0);
    }

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    pids[i] = pid;

    /*
     * Parent process:
     * - reads from pipes[i][0]
     * - writes ACK to ack_pipes[i][1]
     */
    close(pipes[i][1]);
    close(ack_pipes[i][0]);
}

TravelerState states[MAX_TRAVELERS];

for (int i = 0; i < num_travelers; i++) {
    states[i].pid = pids[i];
    states[i].read_fd = pipes[i][0];
    states[i].ack_write_fd = ack_pipes[i][1];
    states[i].current_node = travelers[i].src;
    states[i].finished = false;

    fcntl(pipes[i][0], F_SETFL, O_NONBLOCK);
}

const int screenWidth = 800;
const int screenHeight = 600;

InitWindow(screenWidth, screenHeight, "Bingo Logic - Graph Simulation (M5)");
SetTargetFPS(60);

Vector2 node_pos[MAX_NODES];

for (int i = 0; i < g.num_nodes; i++) {
    node_pos[i] = (Vector2){
        screenWidth / 2 + 200 * cosf(i * 2 * PI / g.num_nodes),
        screenHeight / 2 + 200 * sinf(i * 2 * PI / g.num_nodes)
    };
}

while (!WindowShouldClose()) {
    for (int i = 0; i < num_travelers; i++) {
        if (states[i].finished) {
            continue;
        }

        TravelerMsg msg;

        while (read(states[i].read_fd, &msg, sizeof(msg)) == (ssize_t)sizeof(msg)) {
            if (msg.msg_type == MSG_ARRIVED) {
                states[i].current_node = msg.node;

                if (msg.next_node == -1) {
                    printf("[PID=%d] arrived at node %d | DESTINATION\n", msg.pid, msg.node);
                } else {
                    printf("[PID=%d] arrived at node %d | next node: %d\n",
                           msg.pid,
                           msg.node,
                           msg.next_node);

                    /*
                     * New exam change:
                     * Parent sends approval back to the child.
                     * The ACK content is not important.
                     */
                    char ack = 'A';
                    write(states[i].ack_write_fd, &ack, sizeof(ack));
                }

                fflush(stdout);
            } else if (msg.msg_type == MSG_FINISHED) {
                printf("[PID=%d] finished\n", msg.pid);
                fflush(stdout);

                states[i].finished = true;
                close(states[i].read_fd);
                close(states[i].ack_write_fd);
                break;
            }
        }
    }

    BeginDrawing();
    ClearBackground((Color){245, 245, 250, 255});

    for (int i = 0; i < g.num_nodes; i++) {
        for (int j = 0; j < g.num_nodes; j++) {
            if (g.weights[i][j] != -1) {
                DrawArrow(node_pos[i], node_pos[j], (Color){160, 160, 175, 255}, 2.5f);
            }
        }
    }

    for (int i = 0; i < g.num_nodes; i++) {
        for (int j = 0; j < g.num_nodes; j++) {
            if (g.weights[i][j] != -1) {
                DrawText(
                    TextFormat("%d", g.weights[i][j]),
                    (int)((node_pos[i].x + node_pos[j].x) / 2) + 6,
                    (int)((node_pos[i].y + node_pos[j].y) / 2) + 6,
                    15,
                    (Color){80, 80, 100, 255}
                );
            }
        }
    }

    for (int i = 0; i < g.num_nodes; i++) {
        DrawCircleV(node_pos[i], 22, (Color){50, 70, 120, 255});
        DrawText(
            TextFormat("%d", i),
            (int)node_pos[i].x - 6,
            (int)node_pos[i].y - 8,
            20,
            WHITE
        );
    }

    for (int i = 0; i < num_travelers; i++) {
        Vector2 pos = node_pos[states[i].current_node];

        pos.x += (float)((i % 3) - 1) * 10;
        pos.y += (float)(i / 3) * 10;

        DrawCircleV(pos, 13, TRAVELER_COLORS[i % 10]);
        DrawCircleLines((int)pos.x, (int)pos.y, 13, WHITE);
    }

    int lx = screenWidth - 190;
    int ly = 10;

    DrawRectangle(
        lx - 8,
        ly - 6,
        188,
        num_travelers * 28 + 12,
        (Color){230, 230, 240, 220}
    );

    for (int i = 0; i < num_travelers; i++) {
        DrawCircle(lx + 8, ly + 10 + i * 28, 9, TRAVELER_COLORS[i % 10]);
        DrawCircleLines(lx + 8, ly + 10 + i * 28, 9, WHITE);

        DrawText(
            TextFormat("%d -> %d", travelers[i].src, travelers[i].dst),
            lx + 22,
            ly + 3 + i * 28,
            18,
            (Color){30, 30, 30, 255}
        );
    }

    bool all_done = true;

    for (int i = 0; i < num_travelers; i++) {
        if (!states[i].finished) {
            all_done = false;
            break;
        }
    }

    if (all_done) {
        DrawText("ALL TRAVELERS DONE!", 255, 45, 24, DARKGREEN);
        DrawText("Press ESC to close", 285, 75, 18, (Color){100, 100, 100, 255});
    }

    EndDrawing();
}

CloseWindow();

for (int i = 0; i < num_travelers; i++) {
    waitpid(pids[i], NULL, 0);
}

return 0;


}
