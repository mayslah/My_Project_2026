#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "scheduler.h"

SchedulerType parse_scheduler_name(const char* name) {
if (strcmp(name, "fcfs") == 0) {
return SCHD_FCFS;
}

```
if (strcmp(name, "sjf") == 0) {
    return SCHD_SJF;
}

printf("Error: unknown scheduler '%s'\n", name);
printf("Use: fcfs or sjf\n");
exit(1);
```

}

const char* scheduler_to_string(SchedulerType scheduler) {
if (scheduler == SCHD_FCFS) {
return "FCFS";
}

```
return "SJF";
```

}

static int remaining_path_cost(Graph* g, SchedulerTravelerView* traveler) {
int cost = 0;

```
for (int i = traveler->path_index; i < traveler->path_count - 1; i++) {
    int u = traveler->path[i];
    int v = traveler->path[i + 1];

    if (g->weights[u][v] > 0) {
        cost += g->weights[u][v];
    }
}

return cost;
```

}

int choose_traveler_from_queue(
NodeQueue* queue,
SchedulerType scheduler,
SchedulerTravelerView travelers[],
Graph* g
) {
if (queue_is_empty(queue)) {
return -1;
}

```
if (scheduler == SCHD_FCFS) {
    return queue_pop_at(queue, 0);
}

int best_index = 0;
int best_cost = remaining_path_cost(g, &travelers[queue->items[0]]);

for (int i = 1; i < queue->count; i++) {
    int traveler_id = queue->items[i];
    int cost = remaining_path_cost(g, &travelers[traveler_id]);

    if (cost < best_cost) {
        best_cost = cost;
        best_index = i;
    }
}

return queue_pop_at(queue, best_index);
```

}
