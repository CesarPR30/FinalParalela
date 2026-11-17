#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 100;              // Número total de puntos
    const double L = 1.0;           // Longitud del dominio
    const double dx = L / (N - 1);
    const double tol = 1e-6;
    const int max_iter = 10000;

    int local_N = N / size;  // Suponemos que N es divisible entre size
    std::vector<double> local_u(local_N + 2, 0.0);   // +2 por los fantasmas
    std::vector<double> local_old(local_N + 2, 0.0);
    std::vector<double> global_u;

    if (rank == 0) {
        global_u.resize(N, 0.0);
        global_u[N-1] = 100.0; // Condición de frontera derecha
    }

    // Dispersar parte interna de la malla (sin extremos) a los procesos
    std::vector<double> sendbuf;
    if (rank == 0) {
        sendbuf.resize(N);
        sendbuf[0] = 0.0;
        sendbuf[N - 1] = 100.0;
        for (int i = 1; i < N - 1; ++i)
            sendbuf[i] = 0.0;
    }

    MPI_Scatter(sendbuf.data(), local_N, MPI_DOUBLE,
                &local_u[1], local_N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Aplicar condiciones de frontera locales
    if (rank == 0) local_u[1] = 0.0;
    if (rank == size - 1) local_u[local_N] = 100.0;

    int iter = 0;
    double global_error = 1.0;

    while (global_error > tol && iter < max_iter) {
        local_old = local_u;

        // Comunicación con vecino izquierdo
        if (rank > 0) {
            MPI_Send(&local_old[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&local_old[0], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Comunicación con vecino derecho
        if (rank < size - 1) {
            MPI_Send(&local_old[local_N], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
            MPI_Recv(&local_old[local_N + 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        double local_error = 0.0;
        for (int i = 1; i <= local_N; ++i) {
            // No actualizar extremos si somos primer o último proceso
            if ((rank == 0 && i == 1) || (rank == size - 1 && i == local_N)) continue;
            local_u[i] = 0.5 * (local_old[i - 1] + local_old[i + 1]);
            local_error = std::max(local_error, std::abs(local_u[i] - local_old[i]));
        }

        MPI_Allreduce(&local_error, &global_error, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        iter++;
    }

    // Recolectar resultados internos
    MPI_Gather(&local_u[1], local_N, MPI_DOUBLE,
               global_u.data(), local_N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
std::cout << std::fixed << std::setprecision(4);
std::cout<<"#"<<iter<<"\t"<<std::setprecision(10)<<global_error<<std::endl;
//    std::cout << "Solución después de " << iter << " iteraciones:\n";
    std::cout << std::setprecision(4);
     for (int i = 0; i < N; ++i) {
         std::cout << i * dx <<"\t"<<  global_u[i] << '\n';
         //std::cout << "x = " << i * dx << ", u = " << u[i] << '\n';
     }
    }

    MPI_Finalize();
    return 0;
}

