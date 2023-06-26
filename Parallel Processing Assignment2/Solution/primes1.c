#include <stdio.h>
#include <stdbool.h> 
#include <string.h>
#include <time.h>
#include "mpi.h"

bool isPrime(int n){
    int i;

    if(n == 0 || n == 1)
        return false;

    for(i = 2; i <= n/2; ++i)
        if(n % i == 0)
            return 0;

    return 1;         
}

int main(int argc, char* argv[]){
    int pid;
    int x, y, r;
    int noOfProc;
    char msg[100];

    clock_t duration = clock();

    MPI_Status status;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &noOfProc);

    if(pid == 0){
        int i;
        printf("Enter range: ");
        scanf("%d %d", &x, &y);
        if(x > y)
            return 1;
        r = (y - x) / (noOfProc - 1);
        int r2 = r;

        for(i = 1; i < noOfProc-1; ++i){
            MPI_Send(&x,
                    1, MPI_INT, i, 0,
                    MPI_COMM_WORLD);   
            MPI_Send(&r2,
                    1, MPI_INT, i, 0,
                    MPI_COMM_WORLD);
            x += r;
            r2 += r;        
        }

        r2 = y-1;
        MPI_Send(&x,
                1, MPI_INT, i, 0,
                MPI_COMM_WORLD);
        MPI_Send(&r2,
                1, MPI_INT, i, 0,
                MPI_COMM_WORLD);

        int cnt = 0, tmpCnt;
        for(i = 1; i < noOfProc; ++i){
            MPI_Recv(msg, 100, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
            printf("%s\n", msg);
            MPI_Recv(&tmpCnt,
                1, MPI_INT, i, 0,
                MPI_COMM_WORLD, &status);
            cnt += tmpCnt;    
        }

        printf("After reduction, P0 will have Count = %d \n", cnt);

        duration = clock() - duration;
        printf("Time taken to execute in seconds: %0.2f\n", (double)duration / CLOCKS_PER_SEC);

    }else{
        int r1, r2, i, j;
        MPI_Recv(&r1,
                1, MPI_INT, 0, 0,
                MPI_COMM_WORLD, &status);
        MPI_Recv(&r2,
                1, MPI_INT, 0, 0,
                MPI_COMM_WORLD, &status);
        int tmpCnt = 0;
        for(i = r1; i <= r2; ++i)
            if(isPrime(i))
                tmpCnt++;
        
        sprintf(msg, "p%d: calculate partial count of prime numbers from %d to %d -> Count = %d", pid, r1, r2, tmpCnt);
        MPI_Send(msg, strlen(msg)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&tmpCnt,
                1, MPI_INT, 0, 0,
                MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}