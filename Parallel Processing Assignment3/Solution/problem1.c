#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char* argv[]){
    int pid;
    int noOfProc;
    char inputfile[20];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &noOfProc);

    char msg[100000];
    int key, len, cnt;

    if(pid == 0){
        printf("Enter file name: ");
        scanf("%s", inputfile);
        FILE* file = fopen(inputfile, "r");
        if (file == NULL) {
            printf("Error opening file %s\n", output_file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fread(msg, sizeof(char), 100000, file);
        fclose(file);

        len = strlen(msg);
        int rem = len % noOfProc;
        cnt = (len / noOfProc) + rem;
 
        printf("Enter key value: ");
        scanf("%d", &key);
    }

    MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cnt, 1, MPI_INT, 0, MPI_COMM_WORLD);

    char local_msg[cnt];
    MPI_Scatter(msg, cnt, MPI_CHAR, local_msg, cnt, MPI_CHAR, 0,
                MPI_COMM_WORLD);

    for(int i = 0; i < strlen(local_msg); ++i){
        if(local_msg[i] >= 'A' && local_msg[i] <= 'Z')
            local_msg[i] = (local_msg[i] - 'A' + key) % 26 + 'A'; 
        else if(local_msg[i] >= 'a' && local_msg[i] <= 'z')
            local_msg[i] = (local_msg[i] - 'a' + key) % 26 + 'a';
        else
            local_msg[i] = local_msg[i];        
    }

    MPI_Gather(local_msg, cnt, MPI_CHAR, msg, cnt, MPI_CHAR, 0, MPI_COMM_WORLD);

    if(pid == 0){
        FILE* file = fopen("output.txt", "w");
        fwrite(msg, sizeof(char), len, file);
        fclose(file);
    }

    MPI_Finalize();
    return 0;
}