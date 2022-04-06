#include "../include/scheduler.h"

void *simpleRobotRoutine(void *arg) {
  Robot robot = (Robot)arg;
  Task task = robot->task;
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] starts...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
#endif 
  int jobID;
  while (!queueIsEmpty(task->jobQ)) {
    queueDequeueFront(task->jobQ, &jobID);
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
        RobotTypeToChar(robot->robotType), robot->id, jobID);
#endif
    simpleWork(jobID, robot, task);
  }

  pthread_exit(NULL);
}

void *notsimpleRobotRoutine(void *arg) {
  Robot robot = (Robot)arg;
  Task task = robot->task;
  char type = RobotTypeToChar(robot->robotType);
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] starts...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
#endif 
  int jobID;
  if (type=='A'){
    while (!queueIsEmpty(task->jobQA)) {
      queueDequeueFront(task->jobQA, &jobID);
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
        RobotTypeToChar(robot->robotType), robot->id, jobID);
#endif
      notsimpleWork(jobID, robot, task);
    }
  } else if (type=='B'){
    while (!queueIsEmpty(task->jobQB)) {
      queueDequeueFront(task->jobQB, &jobID);
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
        RobotTypeToChar(robot->robotType), robot->id, jobID);
#endif
      notsimpleWork(jobID, robot, task);
    } 
  } else if (type=='C'){
    while (!queueIsEmpty(task->jobQC)) {
      queueDequeueFront(task->jobQC, &jobID);
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
        RobotTypeToChar(robot->robotType), robot->id, jobID);
#endif
      notsimpleWork(jobID, robot, task);
    }
  }

  pthread_exit(NULL);
}

void simpleWork(int jobID, Robot robot, Task task) {
  double timer = getCurrentTime();
  switch (jobID) {
  case SKELETON:

    sem_wait(&(task->semskeleton));
    //sleep(1);
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] making skeleton...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
    fflush(stdout);
#endif
    makeSkeleton(robot);
    sem_post(&(task->semengine));
    break;


  case ENGINE:

    sem_wait(&(task->semengine));
    //sleep(1);
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] making engine...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
    fflush(stdout);
#endif  
    makeEngine(robot);
    sem_post(&(task->semchassis));
    break;


  case CHASSIS:

    sem_wait(&(task->semchassis));
    //sleep(1);
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] making chassis...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
    fflush(stdout);
#endif 
    makeChassis(robot);
    break;


  case BODY:

    sem_wait(&(task->sembody));
    sem_post(&(task->semskeleton));
    // sem_post(&(task->semengine));
    // sem_post(&(task->semchassis));
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] making body...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
    fflush(stdout);
#endif 
    makeBody(robot);
      
    
    break;


  case WINDOW:
    sem_wait(&(task->semwindow));
    makeWindow(robot);
    break;
  case TIRE:
    sem_wait(&(task->semtire));
    makeTire(robot);
    break;
  case BATTERY:
    sem_wait(&(task->sembattery));
    makeBattery(robot);
    break;
  case CAR:
    sem_wait(&(task->semcar));
      sem_post(&(task->sembattery));
      for(int j = 0; j < 7; j++) {
        sem_post(&(task->semwindow));
      }
      for(int j = 0; j < 4; j++) {
        sem_post(&(task->semtire));
      }
      
    makeCar(robot);

    break;
  default:
    err_printf(__func__, __LINE__, "Error!! Robot%c[%d] gets invalid jobID %d\n", 
        RobotTypeToChar(robot->robotType), robot->id, jobID);
    break;
  }
  timer = getCurrentTime() - timer;
#ifdef DEBUG
  debug_printf(__func__, "Robot%c[%d] job %d done! Time: %lf\n", 
      RobotTypeToChar(robot->robotType), robot->id, jobID, timer);
#endif
  if (jobID==6){
    sem_post(&(task->semcar));
  }
  if (jobID==7){
    sem_post(&(task->sembody));
  }
}

void notsimpleWork(int jobID, Robot robot, Task task) {
  double timer = getCurrentTime();
  switch (jobID) {
  case SKELETON:

    sem_wait(&(task->semskeleton));
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] making skeleton...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
    fflush(stdout);
#endif
    makeSkeleton(robot);
    break;

  case ENGINE:
    sem_wait(&(task->semengine));
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] making engine...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
    fflush(stdout);
#endif  
    makeEngine(robot);
    break;

  case CHASSIS:
    sem_wait(&(task->semchassis));
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] making chassis...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
    fflush(stdout);
#endif 
    makeChassis(robot);
    break;

  case BODY:
    sem_wait(&(task->sembody));
    sem_post(&(task->semskeleton));
    sem_post(&(task->semengine));
    sem_post(&(task->semchassis));
    sem_post(&(task->semcar));
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] making body...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
    fflush(stdout);
#endif 
    makeBody(robot);
    break;

  case WINDOW:
    sem_wait(&(task->semwindow));
    makeWindow(robot);
    break;
  case TIRE:
    sem_wait(&(task->semtire));
    makeTire(robot);
    break;
  case BATTERY:
    sem_wait(&(task->sembattery));
    makeBattery(robot);
    break;
  case CAR:
    sem_wait(&(task->semcar));
      sem_post(&(task->sembattery));
      for(int j = 0; j < 7; j++) {
        sem_post(&(task->semwindow));
      }
      for(int j = 0; j < 4; j++) {
        sem_post(&(task->semtire));
      }
      sem_post(&(task->sembody));
    makeCar(robot);
    break;
  default:
    err_printf(__func__, __LINE__, "Error!! Robot%c[%d] gets invalid jobID %d\n", 
        RobotTypeToChar(robot->robotType), robot->id, jobID);
    break;
  }
  timer = getCurrentTime() - timer;
#ifdef DEBUG
  debug_printf(__func__, "Robot%c[%d] job %d done! Time: %lf\n", 
      RobotTypeToChar(robot->robotType), robot->id, jobID, timer);
#endif
  // if (jobID==7){
  //   sem_post(&(task->sembody));
  // }
}