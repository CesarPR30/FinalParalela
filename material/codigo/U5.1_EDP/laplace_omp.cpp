#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <omp.h>

int main() {
    const int N = 100;           // Número total de puntos
    const double L = 1.0;        // Longitud del dominio
    const double dx = L / (N - 1);
    const double tol = 1e-6;     // Tolerancia para convergencia
    const int max_iter = 10000;

    std::vector<double> u(N, 0.0);
    std::vector<double> u_old(N, 0.0);

    // Condiciones de frontera
    u[0] = 0.0;
    u[N-1] = 100.0;

    int iter = 0;
    double error = 1.0;

    double start_time = omp_get_wtime();

    while (error > tol && iter < max_iter) {
        u_old = u;
        error = 0.0;

        #pragma omp parallel for reduction(max:error)
        for (int i = 1; i < N - 1; ++i) {
            u[i] = 0.5 * (u_old[i - 1] + u_old[i + 1]);
            double local_error = std::abs(u[i] - u_old[i]);
            if (local_error > error) error = local_error;
        }

        iter++;
    }

    double end_time = omp_get_wtime();

//    std::cout << "Convergió en " << iter << " iteraciones, tiempo: "
  //            << (end_time - start_time) << " segundos\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout<<"#"<<iter<<"\t"<<std::setprecision(10)<<error<<std::endl;
//    std::cout << "Solución después de " << iter << " iteraciones:\n";
    std::cout << std::setprecision(4);

    for (int i = 0; i < N; ++i) {
        std::cout << i * dx <<"\t"<<  u[i] << '\n';
        //std::cout << "x = " << i * dx << ", u = " << u[i] << '\n';
    }

    return 0;
}

