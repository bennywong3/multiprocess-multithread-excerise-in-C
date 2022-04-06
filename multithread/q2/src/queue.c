#include "../include/queue.h"

Queue queueCreate(unsigned capacity){
  Queue q = malloc(sizeof(queue_t)); 
  q->capacity = capacity;
  q->front = q->size = 0;
  q->rear = capacity - 1;
  q->array = malloc(q->capacity * sizeof(*q->array));
  sem_init(&sem01, 0, 1);
  return q;
}

void queueDestroy(Queue q){
  free(q->array);
  free(q);
  sem_destroy(&sem01);
}

int queueIsFull(Queue q){
  sem_wait(&sem01);
  int b= (q->size == q->capacity); 
  sem_post(&sem01);
  return b;
}

int queueIsEmpty(Queue q){
  sem_wait(&sem01);
  int b = (q->size == 0);
  sem_post(&sem01);
  return b;
}

int queueEnqueue(Queue q, int* value){ 
  if (queueIsFull(q)) return -1;
  sem_wait(&sem01);
  q->rear = (q->rear + 1) % q->capacity;
  q->array[q->rear] = *value;
  q->size = q->size + 1;
  sem_post(&sem01);
  return 0;
}

int queueDequeueFront(Queue queue, int* container){ 
  if (queueIsEmpty(queue)) return -1; 
  sem_wait(&sem01);
  *container = queue->array[queue->front];
  queue->front = (queue->front + 1) % queue->capacity;
  queue->size = queue->size - 1;
  sem_post(&sem01);
  return 0;
}

int queueDequeueRear(Queue queue, int* container){
  if (queueIsEmpty(queue)) return -1; 
  sem_wait(&sem01);
  *container = queue->array[queue->rear];
  queue->rear = (queue->capacity + queue->rear - 1) % queue->capacity;
  queue->size = queue->size - 1; 
  sem_post(&sem01);
  return 0;
}

int* queueFront(Queue queue){
  if (queueIsEmpty(queue)) return NULL;
  sem_wait(&sem01);
  int *i= &queue->array[queue->front]; 
  sem_post(&sem01);
  return i;
}

int* queueRear(Queue queue){ 
  if (queueIsEmpty(queue)) return NULL;
  sem_wait(&sem01);
  int *i=  &queue->array[queue->rear];
  sem_post(&sem01);
  return i;
}

void queuePrint(Queue q){
  sem_wait(&sem01);
  printf("Queue(^: front, *: rear): [");
  int i;
  for(i = 0; i < q->capacity; i++){
    printf("%d", q->array[i]);
    if(i == q->front) printf("^");
    if(i == q->rear) printf("*");
    i == q->capacity - 1 ? printf("] ") : printf(", ");
  }
  printf("size=%d, capacity=%d\n", q->size, q->capacity);
  fflush(stdout);
  sem_post(&sem01);
} 
