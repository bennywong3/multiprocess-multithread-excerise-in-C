#include "../include/production.h"
#include "../include/utils.h"
#include "../include/scheduler.h"
#include "../include/robot.h"

static int num_cars;
static int num_spaces;
static int num_typeA;
static int num_typeB;
static int num_typeC;
static int total_num_robots;

void startProduction(){
  /* Init robots */
  Robot* robotsA = calloc(num_typeA, sizeof(Robot_t));
  Robot* robotsB = calloc(num_typeB, sizeof(Robot_t));
  Robot* robotsC = calloc(num_typeC, sizeof(Robot_t));
  /* Init robots end*/

  /* Prepare task */
  int ideal=0;
  if (num_spaces>=16 && num_typeA>=9 && num_typeB>= 1 && num_typeC>=7){
    ideal=1;
  }
  Task task = calloc(1, sizeof(Task_t));
  sem_init(&(task->sembody), 0, 1);
  sem_init(&(task->semcar), 0, 0);
  sem_init(&(task->semskeleton), 0, 0);
  sem_init(&(task->semengine), 0, 0);
  sem_init(&(task->semchassis), 0, 0);
  sem_init(&(task->sembattery), 0, 0);
  sem_init(&(task->semwindow), 0, 0);
  sem_init(&(task->semtire), 0, 0);
  task->jobQ = queueCreate(num_cars * 17);
  task->jobQA = queueCreate(num_cars * 9);
  task->jobQB = queueCreate(num_cars * 1);
  task->jobQC = queueCreate(num_cars * 7);
  for (int k = 0; k < num_cars; k++){
    int b=6;
    int c=7;
    queueEnqueue(task->jobQ, &b);
    for(int i = 0; i < 3; i++) { 
      queueEnqueue(task->jobQ, &i);
    }
    queueEnqueue(task->jobQ, &c);
    for(int i = 3; i < 6; i++) { 
      if(i == WINDOW) { 
        for(int j = 0; j < 7; j++) queueEnqueue(task->jobQ, &i);
      } else if(i == TIRE) { 
        for(int j = 0; j < 4; j++) queueEnqueue(task->jobQ, &i);
      } else queueEnqueue(task->jobQ, &i);
    }
  }
  if (ideal==1){
    for (int k = 0; k < num_cars; k++){
      for(int i = 0; i < 8; i++) { 
        if(i == WINDOW) { 
          for(int j = 0; j < 7; j++) queueEnqueue(task->jobQA, &i);
        } else if(i == TIRE) { 
          for(int j = 0; j < 4; j++) queueEnqueue(task->jobQC, &i);
        } else if(i == CHASSIS || i == BATTERY){
          queueEnqueue(task->jobQA, &i);
        } else if(i == BODY){
          queueEnqueue(task->jobQB, &i);
        } else {queueEnqueue(task->jobQC, &i);}
      }
    }
  }
  /* Prepare task end*/

  /* Production start */
  // Create robot, assign task, and start to work
  for (int i = 0; i < num_typeA; ++i) {
    robotsA[i] = createRobot(TypeA);
    robotsA[i]->task = task;
    if (ideal==1){
      pthread_create(&robotsA[i]->pthread, NULL, notsimpleRobotRoutine, robotsA[i]);
    } else{pthread_create(&robotsA[i]->pthread, NULL, simpleRobotRoutine, robotsA[i]);}
  }
  // TODO: create typeB and typeC robots
  for (int i = 0; i < num_typeB; ++i) {
    robotsB[i] = createRobot(TypeB);
    robotsB[i]->task = task;
    if (ideal==1){
      pthread_create(&robotsB[i]->pthread, NULL, notsimpleRobotRoutine, robotsB[i]);
    } else{pthread_create(&robotsB[i]->pthread, NULL, simpleRobotRoutine, robotsB[i]);}
  }
  for (int i = 0; i < num_typeC; ++i) {
    robotsC[i] = createRobot(TypeC);
    robotsC[i]->task = task;
    if (ideal==1){
      pthread_create(&robotsC[i]->pthread, NULL, notsimpleRobotRoutine, robotsC[i]);
    } else{pthread_create(&robotsC[i]->pthread, NULL, simpleRobotRoutine, robotsC[i]);}
  }
  // wait until work done
  for (int i = 0; i < num_typeA; ++i) {
    pthread_join(robotsA[i]->pthread, NULL);
  }
  // TODO: join typeB and typeC robot threads
  for (int i = 0; i < num_typeB; ++i) {
    pthread_join(robotsB[i]->pthread, NULL);
  }
  for (int i = 0; i < num_typeC; ++i) {
    pthread_join(robotsC[i]->pthread, NULL);
  }
  /* Production end */

  /* Release memory */
  for (int i = 0; i < num_typeA; ++i) releaseRobot(robotsA[i]); 
  queueDestroy(task->jobQ);
  free(task); 
  free(robotsA);
  free(robotsB);
  free(robotsC);
}

int main(int argc, char *argv[]) {
  if (argc < 6) {
    printf("Usage: %s <number of cars> <number of spaces> <number of typeA robot> <number of typeB robot> <number of typeC robot>\n", argv[0]);
    return EXIT_SUCCESS;
  }
  // Change to your name and university ID
  printf("Name: Wong Ka Ngai\t UID: 3035568881\n"); 
  /* Initialization */
  num_cars   = atoi(argv[1]);
  num_spaces = atoi(argv[2]);
  num_typeA  = atoi(argv[3]);
  num_typeB  = atoi(argv[4]);
  num_typeC  = atoi(argv[5]);
  total_num_robots = num_typeA + num_typeB + num_typeC;
  printf("Production goal: %d, num space: %d, num typeA: %d, num typeB: %d, num typeC: %d\n",
      num_cars, num_spaces, num_typeA, num_typeB, num_typeC);
  initProduction(num_cars, num_spaces);
  initNumRobot(num_typeA, num_typeB, num_typeC); 
  /* Initialization end */

  double timer = getCurrentTime();
  startProduction();
  timer = getCurrentTime() - timer;
  
  printf("====Final Report====\n");
  printProductionProgress(); 
  printf("Production time: %lf\n", timer);
  return 0;
}
