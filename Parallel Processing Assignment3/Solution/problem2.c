#include <stdio.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char** argv)
{
    int process_id, c_size;
    int no_of_steps = 1000000;
    double step;
    double sum,p_sum = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &c_size);


    //Divide the number of steps into ranges for each slave process
    int process_range = no_of_steps / c_size;
    int start = process_id * process_range;
    int end = start + process_range;
    if (process_id == c_size - 1)
    {
        end = no_of_steps - 1;
    }


    if (process_id == 0)//master process
    {
        step = 1.0 / (double) no_of_steps; //calculate the step for each slave process
    }
    MPI_Bcast(&step, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); //Broadcast the step value to all slaves


    //To calculate the partial sum
    for (int i = start; i < end; i++)
    {
        double p = ((double)(i+0.5))*step;
        p_sum = p_sum + 4.0 / (1.0 + p*p);
    }

    MPI_Reduce(&p_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); //perform reduce to output the value of PI


    //To calculate the value of pi using reduced sum
    if (process_id == 0)
    {
        double pi = step * sum;
        printf("pi = %.20f\n", pi);
    }


    MPI_Finalize();

    return 0;
}
