set term png

set xrange[0:1]
set output 'laplace.png'
set key bottom 
set title 'Laplace 1D'
set xlabel 'x'
set ylabel 'u[x]'
plot 'laplace_sec.out' u 1:2 skip 1, 'laplace_mpi.out' u 1:2 skip 1, 'laplace_omp.out' u 1:2 skip 1
