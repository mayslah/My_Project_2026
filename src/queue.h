#ifndef QUEUE_H
#define QUEUE_H

#define MAX_QUEUE 20

typedef struct {
int items[MAX_QUEUE];
int count;
} NodeQueue;

void queue_init(NodeQueue* q);
int queue_is_empty(NodeQueue* q);
void queue_push(NodeQueue* q, int traveler_id);
int queue_pop_at(NodeQueue* q, int index);

#endif
