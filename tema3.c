#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

//structura reprezentativa pentru clustere
typedef struct cluster {
    int process;
    int* worker;
}cluster;

//citire date din fisier si trimiterea catre celelalte procese
int read_and_send(int rk, int rk_next, cluster* clusters) {
    FILE* file;
    if (rk == 0) {
        file = fopen("cluster0.txt", "r");
        if(file == NULL)
        {
            printf("Error!");   
            exit(1);             
        } 
    } else if (rk == 1) {
        file = fopen("cluster1.txt", "r");
        if(file == NULL)
        {
            printf("Error!");   
            exit(1);             
        } 
    } else if (rk == 2) {
        file = fopen("cluster2.txt", "r");
        if(file == NULL)
        {
            printf("Error!");   
            exit(1);             
        } 
    }

    //citirea numarului workerilor si alocarea vectorului de workeri 
    int line;
    fscanf(file, "%d", &line);
    clusters[rk].worker = (int*) malloc(line * sizeof(int));
    //trimiterea dimensiunii catre celelalte procese
    printf("M(%d,%d)\n", rk, rk_next);
    MPI_Send(&line, 1, MPI_INT, rk_next, 0, MPI_COMM_WORLD); 
    //citirea workerilor si trimiterea acestora si celorlalte procese
    for (int i = 0; i < line; i++) {
        int val;
        fscanf(file, "%d", &val);
        clusters[rk].worker[i] = val;
        printf("M(%d,%d)\n", rk, rk_next);
        MPI_Send(&val, 1, MPI_INT, rk_next, 0, MPI_COMM_WORLD);

    }
    return line;

}
//printarea mesajului corespunzator topologiei
void printf_topology(int rk, int dim0, int dim1, int dim2, cluster *clusters) {
    //printare rang proces
    printf("%d -> ", rk);
    //procesul 0 si workerii corespunzatori
    printf("0:");
    for (int i = 0; i < dim0; i++) {
        if (i < dim0 - 1) {
            printf("%d,", clusters[0].worker[i]);
        }else {
            printf("%d", clusters[0].worker[i]);             
        }
    }
    //procesul 1 si workerii corespunzatori
    printf(" 1:");
    for (int i = 0; i < dim1; i++) {
        if (i < dim1 - 1) {
            printf("%d,", clusters[1].worker[i]);
        }else {
            printf("%d", clusters[1].worker[i]);             
        }
    }  
    //procesul 2 si workerii corespunzatori
    printf(" 2:");
    for (int i = 0; i < dim2; i++) {
        if (i < dim2 - 1) {
            printf("%d,", clusters[2].worker[i]);
        }else {
            printf("%d", clusters[2].worker[i]);
        }
    }  
    printf("\n");
}
//functie pentru primirea informatiilor
int recv_data(int rk_send, cluster* clusters) {
        MPI_Status status;
        int dim_rank;
        //primire numarul de workeri
        MPI_Recv(&dim_rank, 1, MPI_INT, rk_send, 0, MPI_COMM_WORLD, &status);  
        //alocare memorie pentru workeri     
        clusters[rk_send].worker = (int*) malloc(dim_rank * sizeof(int));
        //primire workeri
        for (int i = 0; i < dim_rank; i++) {
            int x;
            MPI_Recv(&x, 1, MPI_INT, rk_send, 0, MPI_COMM_WORLD, &status);
            clusters[rk_send].worker[i] = x;
        }
        return dim_rank;
}
void send_workers(int rk, int rk0, int rk1, int rk2, int dim_rank0, int dim_rank1, int dim_rank2,int line, cluster* clusters) {
        for (int i = 0; i < line; i++) {
            //trimitere rangul procesului
            printf("M(%d,%d)\n", rk, clusters[rk].worker[i]);
            MPI_Send(&rk, 1, MPI_INT, clusters[rk].worker[i], 0, MPI_COMM_WORLD);
            //trimitere dimensiune 0:
            printf("M(%d,%d)\n", rk, clusters[rk].worker[i]);
            MPI_Send(&dim_rank0, 1, MPI_INT, clusters[rk].worker[i], 0, MPI_COMM_WORLD);
            //trimitere dimensiune 1: 
            printf("M(%d,%d)\n", rk, clusters[rk].worker[i]);
            MPI_Send(&dim_rank1, 1, MPI_INT, clusters[rk].worker[i], 0, MPI_COMM_WORLD);
            //trimitere dimensiune 2: 
            printf("M(%d,%d)\n", rk, clusters[rk].worker[i]);
            MPI_Send(&dim_rank2, 1, MPI_INT, clusters[rk].worker[i], 0, MPI_COMM_WORLD);
        }
        //trimitere topologie
        //rang 0
        for (int i = 0; i < line; i++) {
            for(int j = 0; j < dim_rank0; j++) {
                int aux = clusters[rk0].worker[j];
                printf("M(%d,%d)\n", rk, clusters[rk].worker[i]);
                MPI_Send(&aux, 1, MPI_INT, clusters[rk].worker[i], 0, MPI_COMM_WORLD);
            }
        }
        //rang 1
        for (int i = 0; i < line; i++) {
            for (int j = 0; j < dim_rank1; j++) {
                int aux = clusters[rk1].worker[j];
                printf("M(%d,%d)\n", rk, clusters[rk].worker[i]);
                MPI_Send(&aux, 1, MPI_INT, clusters[rk].worker[i], 0, MPI_COMM_WORLD);
            }
        }
        //rang 2
        for (int i = 0; i < line; i++) {
            for (int j = 0; j < dim_rank2; j++) {
                int aux = clusters[rk2].worker[j];
                printf("M(%d,%d)\n", rk, clusters[rk].worker[i]);
                MPI_Send(&aux, 1, MPI_INT, clusters[rk].worker[i], 0, MPI_COMM_WORLD);
            }
        }
}
int main (int argc, char *argv[])
{
    int  numtasks, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    cluster *clusters = (cluster*) malloc(3 * sizeof(cluster));
    int N = atoi(argv[1]);
    int* v = (int*) malloc (N * sizeof(int));

    if (rank == 0) {
        MPI_Status status;
        //citire date si trimitere informatii
        int line = read_and_send(rank, 2, clusters);
        //primire date despre procesul 2
        int dim_rank2 = recv_data(2, clusters);
        int dim_rank1;
        MPI_Recv(&dim_rank1 , 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
        //alocare memorie pentru workeri     
        clusters[1].worker = (int*) malloc(dim_rank1 * sizeof(int));
        for (int i = 0; i < dim_rank1; i++) {
            int val;
            MPI_Recv(&val, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
            clusters[1].worker[i] = val;
        }
        //afisare topologie
        printf_topology(rank, line, dim_rank1, dim_rank2, clusters);
        //trimitere catre workeri
        send_workers(0, 0, 1, 2, line, dim_rank1, dim_rank2, line, clusters);
        //creare vector
        for (int i = 0; i < N; i++) {
            v[i] = i;
        }
        //numar total de workeri
        int numar_workeri = line + dim_rank1 + dim_rank2;
                //calculare rest
        int rest = N - (N / numar_workeri) * numar_workeri;
         //fragmentare vector si trimitere catre workerii procesului 0
        int k = 0;
        int nr_el = N / numar_workeri;
        for (int i = 0; i < line; i++) {
            printf("M(%d,%d)\n", 0, clusters[0].worker[i]);
            MPI_Send(&nr_el, 1, MPI_INT, clusters[0].worker[i], 0, MPI_COMM_WORLD);
            for (int j = k; j < k + N / numar_workeri; j++) {
                int val = v[j];
                printf("M(%d,%d)\n", 0, clusters[0].worker[i]);
                MPI_Send(&val, 1, MPI_INT, clusters[0].worker[i], 0, MPI_COMM_WORLD);
                int element;
                //primire elemente duplicate de la workeri
                MPI_Recv(&element, 1, MPI_INT, clusters[0].worker[i], 0, MPI_COMM_WORLD, &status);
                v[j] = element;
            }
            k += N / numar_workeri;
        }

        //trimitere pentru procesul 1
        int nr_el1 = (N / numar_workeri) * dim_rank1;
        printf("M(%d,%d)\n", 0, 2);
        MPI_Send(&nr_el1, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        for (int j = 0; j < dim_rank1; j++) {
            for (int i = k; i < k + N / numar_workeri; i++) {
                int val = v[i];
                printf("M(%d,%d)\n", 0, 2);
                MPI_Send(&val, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                
            }
            k += N / numar_workeri;
        }
        //primire elementele duplicate de la procesul 1
        for (int i = 0; i < nr_el1; i++) {
            int element;
            MPI_Recv(&element, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
            v[i + k - nr_el1] = element;
        }

        //trimitere catre procesul 2
        int nr_el2 = N - k;
        //trimitere cate elemente are de prelucrat
        printf("M(%d,%d)\n", 0, 2);
        MPI_Send(&nr_el2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        for (int i = k; i < N; i++) {
            int val = v[i];
            printf("M(%d,%d)\n", 0, 2);
            MPI_Send(&val, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        }

        //primire elemente duplicate de la 2
        for (int i = k; i < N; i++) {
            int element;
            MPI_Recv(&element, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
            v[i] = element;
        }
        //printare vectorul dupa duplicarea elementelor de catre workeri
        printf("Rezultat: ");
        for (int i = 0; i < N; i++) {
            printf("%d ", v[i]);
        }
        printf("\n");

    } else if (rank == 1) {
        MPI_Status status;
        int line = read_and_send(rank, 2, clusters);
        //primire date despre procesul 2
        int dim_rank2 = recv_data(2, clusters);
        int dim_rank0;
        MPI_Recv(&dim_rank0 , 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
        //alocare memorie pentru workeri     
        clusters[0].worker = (int*) malloc(dim_rank0 * sizeof(int));
        for (int i = 0; i < dim_rank0; i++) {
            int val;
            MPI_Recv(&val, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
            clusters[0].worker[i] = val;
            
        }
        //printare topologie
        printf_topology(rank, dim_rank0, line, dim_rank2, clusters);
        //trimitere catre workeri
        send_workers(1, 0, 1, 2, dim_rank0, line, dim_rank2, line, clusters);
        int nr_el;
        MPI_Recv(&nr_el , 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
        int* q = (int *)malloc(nr_el * sizeof(int));
        for (int i = 0; i < nr_el; i++) {
            int val;
            MPI_Recv(&val, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
            //adaugarea elementelor in vector
            q[i] = val;
        }
        int count = nr_el / line;

        int k = 0;
        for (int i = 0; i < line; i++) {
            //trimitere cate numere va prelucra fiecare worker
            printf("M(%d,%d)\n", rank, clusters[1].worker[i]);
            MPI_Send(&count, 1, MPI_INT, clusters[1].worker[i], 0, MPI_COMM_WORLD);

            for (int j = k; j < k + nr_el / line; j++){
                //trimite elemente din vector
                int val = q[j];
                printf("M(%d,%d)\n", rank, clusters[1].worker[i]);
                MPI_Send(&val, 1, MPI_INT, clusters[1].worker[i], 0, MPI_COMM_WORLD);
                //primire elementul prelucrat
                int element;
                MPI_Recv(&element, 1, MPI_INT, clusters[1].worker[i], 0, MPI_COMM_WORLD, &status);
                q[j] = element;
                
            }
            k += nr_el / line;
        }

        //trimitere elementul prelucrat catre procesul 2
        for (int i = 0; i < nr_el; i++) {
            int val = q[i];
            printf("M(%d,%d)\n", rank, 2);
            MPI_Send(&val, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
            
        }

    } else if (rank == 2) {
        MPI_Status status;
        //citire date si trimitere informatii
        //catre procesul 0
        int line = read_and_send(rank, 0, clusters);
        //catre procesul 1
        int l = read_and_send(rank, 1, clusters);
        //primire date de la procesul 0
        int dim_rank0 = recv_data(0, clusters);
        //primire date de la procesul 1
        int dim_rank1 = recv_data(1, clusters);
        //trimitere informatii catre 0 de la 1
        printf("M(%d,%d)\n", 2, 0);
        MPI_Send(&dim_rank1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        for (int i = 0; i < dim_rank1; i++) {
            int val = clusters[1].worker[i];
            printf("M(%d,%d)\n", 2, 0);

            MPI_Send(&val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        //trimitere informatii catre 1 de la 0
        printf("M(%d,%d)\n", 2, 1);

        MPI_Send(&dim_rank0, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        for (int i = 0; i < dim_rank0; i++) {
            int val = clusters[0].worker[i];
            printf("M(%d,%d)\n", 2, 1);

            MPI_Send(&val, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
        //afisare topologie
        printf_topology(rank, dim_rank0, dim_rank1, line, clusters);
        //trimitere topologie
        send_workers(2, 0, 1, 2, dim_rank0, dim_rank1, line, line, clusters);


        //numarul total de elemente primite din vector
        int nr_el1;
        MPI_Recv(&nr_el1 , 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        printf("M(%d,%d)\n", 2, 1);
        MPI_Send(&nr_el1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        for (int i = 0; i < nr_el1; i++) {
            int val;
            MPI_Recv(&val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            printf("M(%d,%d)\n", 2, 1);

            MPI_Send(&val, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        }

        for (int i = 0; i < nr_el1; i++) {
            int element;
            MPI_Recv(&element, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
            //printf("%d\n", element);
            printf("M(%d,%d)\n", 2, 0);
            MPI_Send(&element, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }

        int nr_el;
        MPI_Recv(&nr_el , 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        int* q = (int *)malloc(nr_el * sizeof(int));
        for (int i = 0; i < nr_el; i++) {
            int val;
            MPI_Recv(&val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            //adaugarea elementelor primite in vector
            q[i] = val;
        }

        for (int i = 0; i < line; i++) {
            //trimitere numarul de elemente de prelucrat
            printf("M(%d,%d)\n", rank, clusters[2].worker[i]);
            MPI_Send(&nr_el, 1, MPI_INT, clusters[2].worker[i], 0, MPI_COMM_WORLD);
            for (int j = 0; j < nr_el; j++) {
                //trimitere elemente catre workeri
                int val = q[j];
                printf("M(%d,%d)\n", rank, clusters[2].worker[i]);
                MPI_Send(&val, 1, MPI_INT, clusters[2].worker[i], 0, MPI_COMM_WORLD);
                //primire elemente prelucrate de la workeri
                int element;
                MPI_Recv(&element, 1, MPI_INT, clusters[2].worker[i], 0, MPI_COMM_WORLD, &status);
                q[j] = element;
                //trimitere catre procesul 0
                printf("M(%d,%d)\n", rank, 0);
                MPI_Send(&element, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
 
    } else {
         MPI_Status status;
            int rk;
            int dim0, dim1, dim2;
            //primire rankul procesului
            MPI_Recv(&rk, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            //primire numar workeri proces 0
            MPI_Recv(&dim0, 1, MPI_INT, rk, 0, MPI_COMM_WORLD, &status);
            //primire numar workeri proces 1
            MPI_Recv(&dim1, 1, MPI_INT, rk, 0, MPI_COMM_WORLD, &status);
            //primire numar workeri proces 2
            MPI_Recv(&dim2, 1, MPI_INT, rk, 0, MPI_COMM_WORLD, &status);
            //alocare memorie pentru clustere
            clusters[0].worker = (int*) malloc(dim0 * sizeof(int));
            clusters[1].worker = (int*) malloc(dim1 * sizeof(int));
            clusters[2].worker = (int*) malloc(dim2 * sizeof(int));
            //primire cluster 0 de la proces
            for (int i = 0; i < dim0; i++) {
                int val;
                MPI_Recv(&val, 1, MPI_INT, rk, 0, MPI_COMM_WORLD, &status);
                clusters[0].worker[i] = val;
            }
            //primire cluster 1 de la proces
            for (int i = 0; i < dim1; i++) {
                int val;
                MPI_Recv(&val, 1, MPI_INT, rk, 0, MPI_COMM_WORLD, &status);
                clusters[1].worker[i] = val;
            }
            //primire cluster 2 de la proces
            for (int i = 0; i < dim2; i++) {
                int val;
                MPI_Recv(&val, 1, MPI_INT, rk, 0, MPI_COMM_WORLD, &status);
                clusters[2].worker[i] = val;
            }
            //afisare topologie
            printf_topology(rank, dim0, dim1, dim2, clusters);
            
            int element;
            int nr;
            //primire numar de elemente pe care le va prelucra
            MPI_Recv(&nr, 1, MPI_INT, rk, 0, MPI_COMM_WORLD, &status);
            for (int i = 0; i < nr; i++) {
                //primire element pentru a fi prelucrat
                MPI_Recv(&element, 1, MPI_INT, rk, 0, MPI_COMM_WORLD, &status);
                element *= 2;
                //trimite rezultatul catre proces
                printf("M(%d,%d)\n", rank, rk);
                MPI_Send(&element, 1, MPI_INT, rk, 0, MPI_COMM_WORLD);
            }
            



    }

    MPI_Finalize();
}