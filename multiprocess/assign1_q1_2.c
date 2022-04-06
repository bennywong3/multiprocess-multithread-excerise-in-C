#include <stdio.h>
#include <sys/wait.h>  
#include <unistd.h>    
#include <string.h>  

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>  // for using shared memory 
#include <stdlib.h>


void sigstop(); 
void sigcont(); 
int contin=0;

int main(int argc, char* argv[]) 
{
	printf("This is the BEGINNING of the program.\n\n");
	if(argc-1 != 3){
		printf("Error: The number of input integers now is %d. Please input 3 integers.\n",argc-1);
		return -1;
	}// Don't modify this Error Checking part
		
	{
		// Write your code in this brace
		int shmid;
		int *differ;
		pid_t pid_a;
		pid_t pid_b;
		int v1,v2,v3;
		int status;

		v1=atoi(argv[1]);
		v2=atoi(argv[2]);
		v3=atoi(argv[3]);
		
		int size_data = 3 * sizeof(int);
        shmid = shmget(IPC_PRIVATE, size_data, 0666 | IPC_CREAT);

		pid_b=fork();
		if (pid_b>0){
			pid_a=fork();
		}
		
        if (pid_a==0) {
			signal(SIGSTOP, sigstop);
			signal(SIGCONT, sigcont);
			while(contin!=1);
			printf("Child process A ID: %d.\n", getpid());
			differ=shmat(shmid,0,0);
			differ[2]=differ[0]+differ[1];
			printf("Sum of differences: %d.\n\n",differ[2]);
			shmdt(differ);
			printf("Send a SIGCONT to Process %d.\n\n",pid_b); 
			//fflush(stdout);
			kill(pid_b, SIGCONT);
			exit(0);

		} else if (pid_b==0){
			signal(SIGSTOP, sigstop);
			signal(SIGCONT, sigcont);
			while(contin!=1);
			printf("Child process B ID: %d.\n", getpid());
			differ=shmat(shmid,0,0);
			if (differ[2]>differ[0]){
				printf("The 3rd argument is larger than the 1st argument.\n");
			} else if (differ[2]<differ[0]){
				printf("The 3rd argument is smaller than the 1st argument.\n");
			} else{
				printf("The 3rd argument is equal to the 1st argument.\n");  
			}
			shmdt(differ);
			exit(0);

		} else{
			sleep(1); //sleep(1) is only allowed once at the beginning of the parent process after fork()
			int incre=0;
			differ=shmat(shmid,0,0);
			kill(pid_a, SIGSTOP);
			kill(pid_b, SIGSTOP);
			printf ("Apply %d bytes.\n",size_data);
			printf("Parent process ID: %d.\n", getpid());
			differ[0]=v2-v1;
            differ[1]=v3-v2;
			printf("Differences: %d, %d.\n",differ[0],differ[1]);
			printf ("Send a SIGCONT to process %d.\n\n", pid_a);
			kill(pid_a, SIGCONT);
			waitpid(pid_a,&status,0); 
			incre=incre+1;
			printf("Exited Process ID: %d; Count: %d.\n",pid_a,incre); 
			waitpid(pid_b,&status,0); 
			incre=incre+1;
			printf("Exited Process ID: %d; Count: %d.\n",pid_b,incre);

			shmdt (differ);
			shmctl(shmid, IPC_RMID, NULL);
			
		}	
	}
	printf("\nThis is the END of the program.\n");
	return 0;
} 

void sigstop(){ 
	//printf("Receive a SIGSTOP.\n"); 
	contin=0;
} 

void sigcont(){ 
	printf("Receive a SIGCONT.\n"); 
	contin=1;
}

// Plase use the following lines to print related information if needed.
/*
sleep(1);  // which you may need for registration of signal handlers in child process, 
               //only allowd at the beginning of the parent process after fork();
printf ("Apply %d bytes.\n",***);
printf("Child process A ID: %d.\n", getpid());
printf("Child process B ID: %d.\n", getpid());
printf("Sum of differences: %d.\n\n", ***);
printf("Send a SIGCONT to Process %d.\n\n", ***); 
printf("Parent process ID: %d.\n");
printf("Differences: %d, %d.\n", ***,***);
printf("Send a SIGCONT to process %d.\n\n", ***);
printf("Exited Process ID: %d.\n", **); 
printf("Receive a SIGCONT.\n"); 
printf("Receive a SIGSTOP.\n"); 


printf("Exited Process ID: %d; Count: %d.\n", **, ***); 
printf("The 3rd argument is larger than the 1st argument.\n");
printf("The 3rd argument is smaller than the 1st argument.\n");
printf("The 3rd argument is equal to the 1st argument.\n");   
*/ 



