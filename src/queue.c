#include "queue.h"

void queue_init(NodeQueue* q) {
q->count = 0;
}

int queue_is_empty(NodeQueue* q) {
return q->count == 0;
}

void queue_push(NodeQueue* q, int traveler_id) {
if (q->count < MAX_QUEUE) {
q->items[q->count] = traveler_id;
q->count++;
}
}

int queue_pop_at(NodeQueue* q, int index) {
if (index < 0 || index >= q->count) {
return -1;
}


int traveler_id = q->items[index];

for (int i = index; i < q->count - 1; i++) {
    q->items[i] = q->items[i + 1];
}

q->count--;
return traveler_id;


}
