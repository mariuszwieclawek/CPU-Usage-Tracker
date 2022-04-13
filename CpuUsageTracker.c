#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>


struct CPU_Usage{
    long double Total;
    long double Idle;
    long double PrevTotal;
    long double PrevIdle;
    long double CPUPercentageUsage;
};

void Calculate_CPU_Usage(struct CPU_Usage *cpu, int times, int lag){
	char buffer[100];
	const char d[2] = " ";
	char* token;
	int i = 0;
 	
    for(int j=1; j<=times; j++){
        FILE* fp = fopen("/proc/stat","r");
        i = 0;
        fgets(buffer,100,fp);
        fclose(fp);
        token = strtok(buffer,d);
        cpu->Total = 0;
        while(token!=NULL){         
            token = strtok(NULL,d);
            if(token!=NULL){
                cpu->Total += atoi(token);
                if(i==3)
                    cpu->Idle = atoi(token);  
                i++;
            }
        }

        if(j==times){
            cpu->CPUPercentageUsage = 100 - (cpu->Idle-cpu->PrevIdle)*100.0/(cpu->Total-cpu->PrevTotal);
            break;
        }
        cpu->PrevIdle = cpu->Idle;
        cpu->PrevTotal = cpu->Total;
    
        sleep(lag);
    }
}

 
int main(int argc,char* argv[]){
    struct CPU_Usage CPU;
    //Calculate_CPU_Usage(&CPU,2,1);

    /*printf("IDLE:%Lf\n",CPU.Idle);
    printf("PREVIDLE:%Lf\n",CPU.PrevIdle);
    printf("PREVTOTAL:%Lf\n",CPU.PrevTotal);
    printf("TOTAL:%Lf\n",CPU.PrevIdle);*/
    while(1){
        Calculate_CPU_Usage(&CPU,2,1);
        printf("Busy for : %Lf %% of the time.\n", CPU.CPUPercentageUsage);	
    }
	return 0;
}