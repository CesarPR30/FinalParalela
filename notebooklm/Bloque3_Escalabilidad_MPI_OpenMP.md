# Bloque 3: escalabilidad, MPI, OpenMP y programación híbrida

Curso: CS4052 Computación Paralela y Distribuida (UTEC), ciclo 2026-II.
Objetivo: resolver la pregunta 3 del examen final, que compara la escalabilidad de dos o tres modelos paralelos. Vale 7 puntos.

> Instrucción sugerida para el video en NotebookLM: "Enseña este material como un profesor. Explica primero qué es el overhead y la isoeficiencia con intuición, luego la receta en cinco pasos, y resuelve paso a paso la comparación filas contra bloques y la de memoria compartida, distribuida e híbrida. Termina con un repaso de MPI y OpenMP. Habla en español."

---

## 1. Qué tiene que saber hacer el estudiante

Las preguntas 3 de los finales anteriores fueron:

- 2025-I: Quicksort con dos topologías de red, malla contra hipercubo.
- 2025-II sección 1: algoritmo de Floyd con partición por filas contra partición por bloques.
- 2025-II sección 2: multiplicación de matrices, filas contra bloques.
- 2026-I: el mismo problema en memoria compartida, distribuida e híbrida.

El enunciado da fórmulas de tiempo paralelo con un término de cómputo y términos de comunicación: alfa, la latencia (el costo fijo de mandar un mensaje), y beta, el costo por cada dato enviado. Se pide calcular el overhead, la condición de isoeficiencia, decir cuál escala mejor, a veces armar una tabla de eficiencias con números, y comentar qué pasa si crece el problema o si hay un límite de memoria.

## 2. La intuición del overhead

Cuando cuatro personas suman una lista de números, no tardan exactamente la cuarta parte: también tienen que juntar sus resultados parciales. Ese tiempo extra, que no existiría si una sola persona hiciera todo, es el overhead.

Formalmente, el overhead T cero es p por el tiempo paralelo, menos el trabajo W. Si el tiempo paralelo fuera exactamente W entre p, el overhead sería cero. Todo lo que sobra es comunicación, sincronización, desbalance o cómputo repetido.

La eficiencia se puede escribir en función del overhead: E igual a 1 entre 1 más T cero sobre W. Si el overhead es pequeño frente al trabajo, la eficiencia se acerca a 1.

## 3. Qué es la isoeficiencia

Si se agregan procesadores sin agrandar el problema, la eficiencia baja, porque el overhead crece con p y el trabajo no. La pregunta de la isoeficiencia es: ¿cuánto tiene que crecer el problema para mantener la misma eficiencia cuando aumento los procesadores?

Se despeja de la fórmula de eficiencia: W igual a K por T cero, donde K es E entre 1 menos E. Para eficiencia 0,5, K vale 1; para 0,8, K vale 4; para 0,9, K vale 9.

Un algoritmo cuya isoeficiencia es p log p necesita que el problema crezca solo un poco más rápido que los procesadores: es muy escalable. Uno cuya isoeficiencia es p al cubo necesita multiplicar el trabajo por 8 cada vez que duplica los procesadores: escala mal.

## 4. La receta en cinco pasos

1. Escribir el tiempo paralelo e identificar el trabajo W, que es el término de cómputo multiplicado por p.
2. Calcular el overhead: p por el tiempo paralelo menos W. El cómputo se cancela y quedan los términos de comunicación multiplicados por p.
3. Igualar W a K por cada término del overhead, por separado, y despejar n en función de p.
4. Quedarse con la condición más restrictiva, la que obliga a n a crecer más rápido, porque hay que cumplirlas todas a la vez.
5. Comparar los modelos: el que necesita menos crecimiento del problema es el más escalable.

Interpretaciones que el profesor pide con frecuencia:

- ¿Se cumple que W es igual al costo? No, porque el costo es W más el overhead, y el overhead es positivo.
- ¿Qué significa W proporcional a p al cubo cuando W es n al cubo? Significa n proporcional a p: el tamaño del problema crece linealmente con los procesadores.
- ¿Qué pasa si crece n? El cómputo crece más rápido que la comunicación, así que la eficiencia de ambos modelos se acerca a 1, aunque el mejor sigue siendo mejor.
- La granularidad es cómputo entre comunicación. Mayor granularidad significa más trabajo útil por cada mensaje y mejor escalabilidad. Además, la eficiencia es G entre 1 más G.

## 5. Ejemplo resuelto: filas contra bloques (finales 2025-II)

En el algoritmo de Floyd con partición por filas, cada proceso tiene filas completas. En cada iteración necesita una fila entera, de n elementos, y hay n iteraciones: la comunicación por ancho de banda es beta por n al cuadrado. Con partición por bloques en una malla de raíz de p por raíz de p, cada proceso solo necesita un trozo de n entre raíz de p de la fila y de la columna: la comunicación baja a beta por n al cuadrado entre raíz de p.

Overhead de filas: alfa por p por n por log p, más beta por p por n al cuadrado. Overhead de bloques: alfa por p por n por log de raíz de p, más beta por raíz de p por n al cuadrado.

Isoeficiencia con el término de ancho de banda. Filas: n al cubo igual a K por p por n al cuadrado, así que n crece como p. Bloques: n al cubo igual a K por raíz de p por n al cuadrado, así que n crece como raíz de p. Bloques escala mejor.

Tabla numérica con n igual a 10 000 y todas las constantes en 1: con 16 384 procesadores, el overhead de filas es unos 1,6 billones frente a un trabajo de 1 billón, eficiencia de aproximadamente 0,38. El de bloques es unos 13 mil millones, eficiencia de aproximadamente 0,99. Con pocos procesadores ambos están cerca de 1; la diferencia aparece cuando p es grande.

Con la multiplicación de matrices pasa lo mismo: filas da n proporcional a p y bloques n proporcional a raíz de p. Los métodos clásicos de la clase son Fox (broadcast por filas y desplazamiento por columnas), Cannon (alineación inicial y solo desplazamientos circulares, isoeficiencia p a la tres medios) y DNS (malla tridimensional, isoeficiencia p por log al cubo de p).

## 6. Ejemplo resuelto: compartida, distribuida e híbrida (final 2026-I)

Problema iterativo sobre una malla de n por n durante k iteraciones. Trabajo: k por n al cuadrado.

Memoria compartida, un nodo con P núcleos: el overhead es solo sincronización, k por gamma por log P en cada iteración. Overhead total: k por P por gamma por log P. Isoeficiencia: n al cuadrado proporcional a P log P, trabajo del orden de k P log P.

Memoria distribuida, P procesos MPI: además de latencia aparece un término de ancho de banda que depende de n, k por beta por n por log P. Al multiplicar por P y despejar, n al cuadrado proporcional a P por n por log P, es decir, n proporcional a P log P, y el trabajo crece como k por P al cuadrado por log al cuadrado de P. Es la peor isoeficiencia.

Modelo híbrido, q nodos con m hilos cada uno: la sincronización ocurre dentro del nodo, log m, y la comunicación entre nodos, log q, donde q es P entre m. La isoeficiencia es k por P al cuadrado por log al cuadrado de P entre m. Queda entre las dos anteriores.

Ahora la restricción de memoria. En memoria compartida todo el problema debe caber en una máquina: b por n al cuadrado menor o igual a la memoria del nodo. En distribuida se suma la memoria de P nodos. En híbrida, la de P entre m nodos.

La decisión práctica: la memoria compartida escala mejor en eficiencia pero tiene un techo fijo de capacidad; la distribuida puede resolver problemas enormes pero pierde eficiencia; la híbrida busca el equilibrio. Es lo que usan los supercomputadores.

## 7. Ejemplo resuelto: malla contra hipercubo (final 2025-I)

Quicksort en p procesos. En malla la comunicación es alfa por raíz de p más beta por n entre raíz de p. En hipercubo es alfa por log p más beta por n entre p log p.

El overhead dominante de la malla es alfa por p elevado a tres medios; el del hipercubo, alfa por p log p. La condición de escalabilidad: n log n proporcional a p a la tres medios en malla y a p log p en hipercubo. El hipercubo es más escalable.

Si el arreglo crece 10 veces partiendo de un millón de elementos y 64 procesos, la razón n log n pasa a ser 11,67 veces mayor. En malla, p crece como esa razón elevada a dos tercios: unos 329 procesos. En hipercubo, p log p crece en esa razón: p log p igual a 4480, lo que da unos 500 procesos. El hipercubo admite más procesos para el mismo crecimiento.

## 8. Costos de comunicación

El costo de mandar un mensaje de m datos es alfa más m por beta.

- Broadcast lineal: el maestro envía a cada proceso uno por uno, p menos 1 por alfa más m beta.
- Broadcast en árbol binomial, el que usa MPI: log p por alfa más m beta.
- Scatter y gather: lineal, del orden de alfa por p más beta por m; en árbol, alfa por log p más beta por m.
- Reducción en árbol: log p por alfa más beta.
- Para mensajes muy grandes conviene el pipeline: m beta más p alfa más un término de raíz.
- Topologías: en un anillo el broadcast tarda p medios pasos; en una malla de dimensión d, p elevado a 1 entre d; en un hipercubo, log p.

## 9. MPI en lo esencial

MPI es la interfaz de paso de mensajes para memoria distribuida. Cada proceso tiene su propia memoria y se identifica con un número llamado rank.

- Inicio y fin: Init, Comm rank, Comm size, Finalize. Se compila con mpicc y se ejecuta con mpirun indicando el número de procesos.
- Punto a punto bloqueante: Send y Recv. El envío síncrono termina cuando el receptor recibió; el estándar usa un buffer si el mensaje es pequeño y se comporta como síncrono si es grande.
- Deadlock: si en un anillo todos envían de forma síncrona antes de recibir, nadie avanza. Se evita alternando el orden según rank par o impar, con Sendrecv o con comunicación no bloqueante.
- No bloqueante: Isend e Irecv retornan de inmediato y la operación se completa con Wait. Isend más Wait equivale a Send. Permite solapar comunicación y cómputo, por ejemplo calcular el interior de un subdominio mientras llegan los valores del borde. No se debe tocar el buffer antes del Wait.
- Colectivas: Barrier, Bcast, Scatter, Gather, Allgather, Reduce y Allreduce. Todos los procesos del comunicador participan.
- Tipos derivados: contiguous, vector, indexed y struct. Se construyen, se confirman con commit, se usan y se liberan. Evitan mandar muchos mensajes pequeños.

## 10. OpenMP en lo esencial

OpenMP es la interfaz para memoria compartida. Sigue el modelo fork-join: el hilo maestro crea un equipo de hilos al entrar en una región paralela y los une al final.

- Variables: shared las comparten todos; private da una copia sin inicializar a cada hilo; firstprivate la inicializa con el valor original; lastprivate devuelve el valor de la última iteración; reduction crea copias privadas y las combina al final.
- Reparto de bucles con schedule: static reparte bloques fijos, ideal si todas las iteraciones cuestan lo mismo; dynamic los entrega a demanda, ideal para cargas irregulares como el fractal de Mandelbrot; guided usa bloques cada vez más pequeños.
- Sincronización: critical deja pasar un hilo a la vez a un bloque; atomic protege una sola operación simple sobre una variable; barrier espera a todos; ordered respeta el orden de las iteraciones; los locks permiten proteger estructuras específicas.
- Trabajo: single lo ejecuta un hilo (con barrera al final); master solo el maestro (sin barrera); sections asigna bloques distintos a hilos distintos; task y taskwait sirven para recursión, como en búsqueda en árboles.
- Condición de carrera: dos hilos acceden a la misma posición, al menos uno escribe y no hay sincronización. El compilador y el procesador pueden reordenar instrucciones; OpenMP garantiza consistencia con barrier, critical, atomic, reduction y flush.

La regla práctica: para sumas, productos, máximos y mínimos, reduction es lo más eficiente; atomic sirve para actualizaciones simples; critical para bloques completos, sabiendo que serializa.

## 11. Programación híbrida

En un clúster cada nodo es una máquina con memoria compartida, y los nodos se conectan por red. La programación híbrida usa MPI entre nodos y OpenMP dentro de cada nodo. Con 8 núcleos por nodo se puede elegir 1 proceso con 8 hilos, 2 con 4 o 4 con 2.

Ventajas: mayor escalabilidad y granularidad, mejor balance de carga y menos memoria, porque hay menos regiones de halo replicadas. Desventajas: código más complejo, efectos de memoria no uniforme (NUMA), y que durante la comunicación MPI los demás hilos quedan esperando.

Sobre el halo: al pasar de P a 4P procesos en una malla 2D, cada proceso tiene un cuarto de las celdas pero la mitad del halo. El halo baja más lento que el cómputo, así que la comunicación relativa crece.

## 12. Ecuaciones diferenciales

La ecuación de Laplace se resuelve con diferencias finitas: cada punto es el promedio de sus cuatro vecinos. El método de Jacobi usa solo valores de la iteración anterior y se paraleliza directamente. Gauss-Seidel usa los valores nuevos en la misma iteración, converge más rápido pero tiene dependencias; se paraleliza con el orden rojo-negro. En paralelo se divide la malla en franjas o bloques y en cada iteración se intercambian las filas de borde con los vecinos.

## 13. Errores comunes

- Olvidar multiplicar por p los términos de comunicación al calcular el overhead.
- Quedarse con la condición menos restrictiva en lugar de la más restrictiva.
- No notar que un término de comunicación que contiene n cambia la forma de la isoeficiencia.
- Confundir critical con atomic.
- Dar la tabla de eficiencias sin explicar la tendencia.

## 14. Resumen en cinco frases

1. El overhead es p por el tiempo paralelo menos el trabajo.
2. La isoeficiencia es W igual a K por el overhead, con K igual a E entre 1 menos E.
3. Partir por bloques escala mejor que por filas porque cada proceso comunica n entre raíz de p datos en lugar de n.
4. La memoria compartida escala mejor en eficiencia, la distribuida en capacidad, y la híbrida equilibra ambas.
5. En OpenMP, reduction para acumular, atomic para operaciones simples y critical para bloques completos.
