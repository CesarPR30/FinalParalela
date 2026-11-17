 /*************************************************
* Este archivo fue escrito como ejemplo en el curso Computacion Paralela y Distribuida,
* de la Universidad de Ingeniería y Tecnología (UTEC)
* Material es de libre uso, entendiendo que debe contener este encabezado
* UTEC no se responsabiliza del uso particular del código
*
* Autor:       Jose Fiestas (UTEC)
* contacto:    jfiestas@utec.edu.pe
* objetivo:    Paralelismo hibrido - MPI+OMP
*              N debe ser multiplo de p
* ejecucion:   export OMP_NUM_THREADS=8
*              export OMP_PROC_BIND=close OMP_PLACES=cores
*              mpirun -np 4 ./matvec_hibrido 4096 50
*              (argumentos: N (matriz NxN), repeticiones del producto)
* contenido:   codigo fuente en MPI-OMP-C++
 *************************************************/
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
    int provided, rank, size;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long N    = (argc > 1) ? atol(argv[1]) : 4096;
    int  reps = (argc > 2) ? atoi(argv[2]) : 50;

    if (N % size != 0) {
        if (rank == 0) fprintf(stderr, "N debe ser multiplo del numero de procesos\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    long filas = N / size;   /* filas por proceso */

    double *A = NULL, *y = NULL;
    double *x     = (double *)malloc(N * sizeof(double));
    double *A_loc = (double *)malloc(filas * N * sizeof(double));
    double *y_loc = (double *)malloc(filas * sizeof(double));

    /* Cada hilo toca las paginas de las filas que luego calculara,
       ANTES de que MPI_Scatter escriba los datos (asi no quedan todas en el
       dominio NUMA del hilo principal). */
    #pragma omp parallel for schedule(static)
    for (long i = 0; i < filas; i++) {
        for (long j = 0; j < N; j++) A_loc[i * N + j] = 0.0;
        y_loc[i] = 0.0;
    }

    /* Solo el proceso 0 genera A y x:  A[i][j] = i+1,  x[j] = j
       => y[i] = (i+1) * N(N-1)/2   (resultado conocido para verificar) */
    if (rank == 0) {
        A = (double *)malloc(N * N * sizeof(double));
        y = (double *)malloc(N * sizeof(double));
        #pragma omp parallel for schedule(static)
        for (long i = 0; i < N; i++)
            for (long j = 0; j < N; j++)
                A[i * N + j] = (double)(i + 1);
        for (long j = 0; j < N; j++) x[j] = (double)j;
    }

    //MPI_Barrier(MPI_COMM_WORLD);

    double t0 = MPI_Wtime();
    MPI_Scatter(A, filas * N, MPI_DOUBLE, A_loc, filas * N, MPI_DOUBLE,0, MPI_COMM_WORLD);
    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    double t_in = MPI_Wtime() - t0;

    //MPI_Barrier(MPI_COMM_WORLD);
    t0 = MPI_Wtime();
    for (int r = 0; r < reps; r++) {
        #pragma omp parallel for schedule(static)
        for (long i = 0; i < filas; i++) {
            double s = 0.0;
            for (long j = 0; j < N; j++)
                s += A_loc[i * N + j] * x[j];
            y_loc[i] = s;
        }
    }
    double t_comp = (MPI_Wtime() - t0) / reps;

    /* ---- Comunicacion de salida ---- */
    t0 = MPI_Wtime();
    MPI_Gather(y_loc, filas, MPI_DOUBLE, y, filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    double t_out = MPI_Wtime() - t0;

    /* ---- Tiempos ---- */
    double t_comm = t_in + t_out;

    if (rank == 0) {
        double esperado_base = (double)N * (double)(N - 1) / 2.0;
        double err_max = 0.0;
        for (long i = 0; i < N; i++) {
            double esperado = (double)(i + 1) * esperado_base;
            double err = fabs(y[i] - esperado) / esperado;
            if (err > err_max) err_max = err;
        }
        double gflops = 2.0 * N * N / t_comp / 1e9;

        printf("N=%ld  procesos=%d  hilos/proceso=%d\n",
               N, size, omp_get_max_threads());
        printf("Error relativo maximo = %.3e\n", err_max);
        printf("Computo (por producto)   = %.6f s  (%.2f GFLOP/s)\n", t_comp, gflops);
        printf("Comunicacion (una vez)   = %.6f s\n", t_comm);
    }

    free(x); free(A_loc); free(y_loc);
    if (rank == 0) { free(A); free(y); }
    MPI_Finalize();
    return 0;
}
