#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MaxItems 5 // Maximum items a producer can produce or a consumer can consume
#define BufferSize 5 // Size of the buffer
#define StringSize 50 // Size of the string

sem_t readdata; 
sem_t analyz; 

pthread_mutex_t mutex;

int in = 0;
int out = 0;
char readstring[BufferSize][StringSize];

struct CPU_Usage{
    char ReadFirstStr[100];
    char ReadSecStr[100];
    long double Total;
    long double Idle;
    long double PrevTotal;
    long double PrevIdle;
    long double CPUPercentageUsage;
}CPU;

void *Read(void *arg)
{   
    sem_wait(&readdata);
    pthread_mutex_lock(&mutex);

    FILE* fp = fopen("/proc/stat","r");
    fgets(readstring[in],StringSize,fp);
    fclose(fp);
    printf("Producer: Insert Item %s at %d\n",readstring[in],in);
    in = (in+1)%BufferSize;

    pthread_mutex_unlock(&mutex);
    sem_post(&analyz);
}

void *Analyze(void *arg){  

    sem_wait(&analyz);
    pthread_mutex_lock(&mutex);

    char *item = readstring[out];
    const char d[2] = " ";
    char* token;
    int i = 0;

    token = strtok(item,d);
    CPU.Total = 0;  
    while(token!=NULL){      
        token = strtok(NULL,d);
        if(token!=NULL){
            CPU.Total += atoi(token);
            if(i==3)
                CPU.Idle = atoi(token);  
            i++;
        }
    }
            
    CPU.CPUPercentageUsage = 100 - (CPU.Idle-CPU.PrevIdle)*100.0/(CPU.Total-CPU.PrevTotal);
    printf("CPU Usage: %Lf %%\n",CPU.CPUPercentageUsage);            
    CPU.PrevIdle = CPU.Idle;
    CPU.PrevTotal = CPU.Total;

    printf("Consumer: Remove Item %s from %d\n",item, out);
    out = (out+1)%BufferSize;

    pthread_mutex_unlock(&mutex);
    sem_post(&readdata);
    
}



int main()
{   
    pthread_t Reader,Analyzer;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&readdata,0,BufferSize);
    sem_init(&analyz,0,0);
    
    while (1)
    {
        pthread_create(&Reader, NULL, Read, NULL); 
        pthread_create(&Analyzer, NULL, Analyze, NULL);
    
        pthread_join(Reader, NULL); 
        pthread_join(Analyzer, NULL);
        
        sleep(1);
    }
    
    pthread_mutex_destroy(&mutex);
    sem_destroy(&readdata);
    sem_destroy(&analyz);

    return 0;
}
