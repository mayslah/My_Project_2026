#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "graph.h"
#include "queue.h"

typedef enum {
SCHD_FCFS,
SCHD_SJF
} SchedulerType;

typedef struct {
int path[MAX_NODES];
int path_count;
int path_index;
} SchedulerTravelerView;

SchedulerType parse_scheduler_name(const char* name);
const char* scheduler_to_string(SchedulerType scheduler);

int choose_traveler_from_queue(
NodeQueue* queue,
SchedulerType scheduler,
SchedulerTravelerView travelers[],
Graph* g
);

#endif
