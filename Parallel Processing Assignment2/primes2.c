#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "mpi.h"

bool isPrime(int n)
{
    int i;

    if (n == 0 || n == 1)
        return false;

    for (i = 2; i <= n / 2; ++i)
        if (n % i == 0)
            return 0;

    return 1;
}

int main(int argc, char *argv[])
{
    int pid;
    int x, y, r, i;
    int noOfProc;
    int cnt;
    int rem;
    char msg[100];

    clock_t duration = clock();

    MPI_Status status;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &noOfProc);

    if (pid == 0)
    {
        printf("Enter range: ");
        scanf("%d %d", &x, &y);

        if (x > y)
            return 1;
        r = (y - x) / (noOfProc - 1);
        rem = (y - x) % (noOfProc - 1);

        MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&rem, 1, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Reduce(&pid, &cnt, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        printf("After reduction, P0 will have Count = %d\n", cnt);

        duration = clock() - duration;
        printf("Time taken to execute in seconds: %0.2f\n", (double)duration / CLOCKS_PER_SEC);
    }
    else
    {
        int a, b, i, res, rem;
        MPI_Bcast(&a, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&b, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&rem, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int c = b;

        int tmpCnt;
        if (pid == 1)
        {
            tmpCnt = 0;
            for (i = a; i <= b; ++i)
            {
                if (isPrime(i))
                    tmpCnt++;
            }
            printf("p%d: calculate partial count of prime numbers from %d to %d->Count = %d\n", pid, a, b, tmpCnt);
            MPI_Reduce(&tmpCnt, &res, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        }
        else if (pid == noOfProc - 1)
        {
            if (rem == 0)
            {
                tmpCnt = 0;
                a = (pid * c) - (c - 1);
                b = pid * c;
                for (i = a; i <= b; ++i)
                {
                    if (isPrime(i))
                        tmpCnt++;
                }
                printf("p%d: calculate partial count of prime numbers from %d to %d->Count = %d\n", pid, a, b, tmpCnt);
                MPI_Reduce(&tmpCnt, &res, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
            }
            else
            {
                tmpCnt = 0;
                a = (pid * c) - (c - 1);
                b = (pid * c) + rem;
                for (i = a; i <= b; ++i)
                {
                    if (isPrime(i))
                        tmpCnt++;
                }
                printf("p%d: calculate partial count of prime numbers from %d to %d->Count = %d\n", pid, a, b, tmpCnt);
                MPI_Reduce(&tmpCnt, &res, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
            }
        }
        else
        {
            tmpCnt = 0;
            a = (pid * c) - (c - 1);
            b = pid * c;
            for (i = a; i <= b; ++i)
            {
                if (isPrime(i))
                    tmpCnt++;
            }
            printf("p%d: calculate partial count of prime numbers from %d to %d->Count = %d\n", pid, a, b, tmpCnt);
            MPI_Reduce(&tmpCnt, &res, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}