#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> 
#include<sys/shm.h>

sem_t coronaPatient;
sem_t fluPatient;
sem_t increment;


int testing(){
	int a = rand() & 1;
	return a;
}

void * Admit_Patient(void * param)
{

	int key=shmget(12329, 1024,IPC_CREAT | IPC_EXCL | 0666);
	int potentialCPatients = (int) shmat(key, NULL, 0);

	sem_wait(&increment); 
	potentialCPatients++;
	sem_post(&increment); 
	
	int testResult = testing();



   if(testResult == 1) //Is a corona patient
   {	
	sem_wait(&increment); 
	potentialCPatients--;
	sem_post(&increment); 


	printf("\nCornona partient confirmed\n");
	sem_post(&coronaPatient); //cornona patient confirmed so the semaphore will be incremented

	while(1)
	{
		if( testing() == 0 && testing() == 0 )
		{
		 sem_wait(&coronaPatient); 
		 printf("\nPatient successfully recovered\n");
		 break;
		}
	}

    }
   else //If not a corona patient, then the person is a flu patient
	{
	sem_wait(&increment); 
	potentialCPatients--;
	sem_post(&increment); 

 	printf("\nFlu patient\n");	
	sem_post(&fluPatient); 
	}
	
}


int main()
{
srand(time(0)); 

	int N; int i;
	printf("Enter the potential number of patients:"); 
	scanf("%d", &N);  

	pthread_t * id =  (pthread_t*) malloc(N* sizeof(pthread_t));

sem_init(&coronaPatient, 0, 1); 
sem_init(&fluPatient, 0, 1); 
sem_init(&increment, 0, 1); 

int key=shmget(12329, 1024,0);
int ptr= (int) shmat(key, NULL, 0);

	for( i=0 ; i < N; i++) //Creating N threads
	{
		if (pthread_create( &id[i] , NULL,  &Admit_Patient , NULL)== -1) 
		{
			printf("Thread Creation Failed!");
			return 1;
		}
	}

	for( i=0 ; i < N; i++)
	{
		pthread_join( id[i] , NULL  );
	}

sem_destroy(&coronaPatient); 
sem_destroy(&fluPatient); 
shmdt(ptr);
shmctl(key, IPC_RMID, NULL);
return 0;

}