# Bloque 1: GPU, CUDA y métricas de rendimiento

Curso: CS4052 Computación Paralela y Distribuida (UTEC), ciclo 2026-II.
Objetivo: resolver la pregunta 1 del examen final, que siempre es un problema de GPU y vale 6 puntos.

> Instrucción sugerida para el video en NotebookLM: "Explica este material como un profesor que prepara a un estudiante para un examen al día siguiente. Primero la intuición, luego las fórmulas y al final resuelve paso a paso el ejemplo de detección de incendios y el de Value at Risk. Habla en español."

---

## 1. Qué tiene que saber hacer el estudiante

En los cuatro exámenes finales anteriores, la pregunta 1 fue un problema en una tarjeta gráfica NVIDIA:

- 2025-I: filtro convolucional de 3 por 3 sobre una imagen.
- 2025-II sección 1: modelo financiero Value at Risk.
- 2025-II sección 2: K vecinos más cercanos (KNN).
- 2026-I: detección de incendios forestales en imágenes satelitales.

Siempre se da el mismo hardware: 2 elevado a 10 hilos por bloque (1024 hilos) y hasta 2 elevado a 31 bloques. También se da un kernel corto y un tiempo por operación, por ejemplo 10 elevado a menos 8 segundos.

Las preguntas se repiten: cuántos modelos caben en la GPU, cómo configurar bloques e hilos, cuántas operaciones de punto flotante hace un hilo, cuánto tarda en paralelo y en secuencial, cuál es el speedup, la eficiencia y la velocidad en FLOP por segundo, y a veces cómo aplicar el método de Foster o si alcanza la memoria compartida.

La mitad del puntaje de cada subpregunta es la justificación. No basta con el número: hay que explicar de dónde sale y qué significa.

## 2. Por qué existen las GPU

Durante décadas los programas se volvían más rápidos solo porque los procesadores subían de frecuencia. Eso se acabó por dos razones.

La primera es el muro de energía. La potencia dinámica de un procesador crece con el cubo de la frecuencia. Duplicar los gigahercios multiplicaría por ocho el consumo y el calor. Por eso los fabricantes dejaron de subir la frecuencia y empezaron a poner más núcleos.

La segunda es el muro de memoria. Calcular es mucho más rápido que traer datos de la memoria principal. Para compensarlo se agregaron cachés, pero el acceso a memoria sigue siendo el cuello de botella.

Seymour Cray, el padre de la supercomputación, lo planteó así: para arar un campo, ¿prefieres dos bueyes fuertes o 1024 gallinas? La CPU es el buey: pocos núcleos muy potentes, optimizados para responder rápido a una tarea. La GPU es el conjunto de gallinas: miles de núcleos simples que, juntos, procesan una enorme cantidad de datos. A esto se le llama priorizar el throughput (rendimiento total) sobre la latencia.

En la taxonomía de Flynn, la GPU es SIMD: una sola instrucción se aplica a muchos datos a la vez. Por eso funciona tan bien en problemas como filtros de imagen o suma de vectores, donde cada píxel o cada elemento recibe el mismo tratamiento.

## 3. Cómo está organizada una GPU

El núcleo básico se llama Streaming Processor. Los núcleos se agrupan en Streaming Multiprocessors, abreviado SM. Todos los hilos de un mismo bloque se ejecutan en el mismo SM.

Los hilos se ejecutan en grupos de 32 llamados warps. Los 32 hilos de un warp ejecutan la misma instrucción al mismo tiempo. Por eso se recomienda que el número de hilos por bloque sea múltiplo de 32, por ejemplo 256.

La memoria tiene niveles:

- Los registros son privados de cada hilo y son lo más rápido.
- La memoria compartida (shared memory) la comparten todos los hilos de un bloque. Está dentro del chip y es rápida, pero un bloque no puede ver la memoria compartida de otro bloque. Sirve para que los hilos colaboren, y necesita sincronización.
- La memoria global es la memoria grande de la tarjeta. La ven todos los hilos y también la CPU, pero es lenta.

Un concepto importante es la coalescencia: la GPU lee la memoria global de forma eficiente cuando los 32 hilos de un warp acceden a direcciones contiguas. Si cada hilo lee en un lugar disperso, el acceso se vuelve lento.

## 4. El modelo de programación CUDA

CUDA es la interfaz de NVIDIA para programar la GPU. La CPU se llama host y la GPU se llama device. Una función que corre en la GPU y se llama desde la CPU se marca como global y se llama kernel.

El flujo de un programa CUDA tiene cinco pasos: reservar memoria en la GPU, copiar los datos de la CPU a la GPU, lanzar el kernel, copiar los resultados de vuelta a la CPU y liberar la memoria.

Al lanzar un kernel se indica cuántos bloques y cuántos hilos por bloque se quieren. Cada hilo calcula su índice global con la fórmula: índice del hilo dentro del bloque más índice del bloque por el tamaño del bloque. Por ejemplo, el hilo 2 del bloque 2, con bloques de 4 hilos, tiene índice 2 más 2 por 4, que es 10.

Casi todos los kernels tienen una condición del tipo "si el índice es menor que N, trabaja". Esa condición existe porque el número de datos no siempre es múltiplo del tamaño de bloque, y los hilos sobrantes del último bloque no deben leer fuera del arreglo. Esos hilos quedan inactivos.

## 5. La receta para resolver la pregunta 1

Estos son los pasos que sirven para cualquier problema GPU del examen.

**Paso 1, capacidad.** Hilos totales igual a 2 elevado a 31 bloques por 2 elevado a 10 hilos, que es 2 elevado a 41, unos 2,2 billones de hilos. Si cada hilo resuelve un escenario y un modelo tiene N escenarios, caben 2 elevado a 41 dividido entre N modelos. En el examen de Value at Risk, N era 2 elevado a 8, así que cabían 2 elevado a 33 modelos.

**Paso 2, configuración.** Se elige un número de hilos por bloque, por ejemplo 256. El número de bloques es el total de datos dividido entre 256, redondeado hacia arriba. Los hilos inactivos son bloques por hilos por bloque menos el total de datos.

**Paso 3, operaciones por hilo.** Se recorre el kernel de adentro hacia afuera. Se cuentan las sumas, restas, multiplicaciones y divisiones del bucle interno, se multiplican por las iteraciones de ese bucle, se suman las operaciones del bucle externo y se multiplica todo por las iteraciones externas. Las comparaciones y los accesos a memoria se ignoran salvo que el enunciado diga otra cosa.

**Paso 4, tiempo paralelo.** Si todos los hilos caben en la GPU al mismo tiempo, el tiempo paralelo es el tiempo de un solo hilo: operaciones por hilo por tiempo por operación.

**Paso 5, tiempo secuencial.** Es el tiempo paralelo multiplicado por el número de hilos que trabajan, porque en secuencial se harían uno tras otro. Conviene pasarlo a días dividiendo entre 86 400 segundos.

**Paso 6, métricas.** El speedup es tiempo secuencial entre tiempo paralelo, y sale aproximadamente igual al número de hilos que trabajan. La eficiencia es el speedup entre el número de hilos usados; si no hay hilos ociosos ni comunicación, sale cerca de 1. La velocidad es el total de operaciones entre el tiempo paralelo. Si dan una velocidad pico, la eficiencia de la GPU es la velocidad alcanzada entre la velocidad pico.

**Paso 7, comentario.** ¿Hay hilos ociosos? ¿Un hilo tiene demasiado trabajo y conviene repartirlo? ¿Sobran bloques? Este comentario vale la mitad del puntaje.

Una pregunta frecuente es por qué el speedup es proporcional a la velocidad en FLOP por segundo. La respuesta: el tiempo secuencial es igual al total de operaciones por el tiempo de una operación. Entonces speedup y velocidad tienen numeradores proporcionales y el mismo denominador, el tiempo paralelo.

## 6. Ejemplo resuelto: detección de incendios (final 2026-I)

Una imagen satelital de 8192 por 8192 píxeles, es decir 2 elevado a 26, unos 67 millones de píxeles. Cada hilo procesa un píxel. Para cada píxel hay 500 escenarios. En cada escenario se calcula un valor inicial con 4 operaciones y luego un bucle de 100 iteraciones. Cada iteración hace dos multiplicaciones, una suma y un seno que cuenta como 40 operaciones: 43 operaciones. Al final de cada escenario se acumula el riesgo con 1 operación.

Operaciones por píxel: 500 por la cantidad 4 más 100 por 43 más 1, que da 2 152 500, unos 2 millones de operaciones por hilo.

Configuración: con 256 hilos por bloque se necesitan 2 elevado a 26 entre 2 elevado a 8, es decir 262 144 bloques. Como la división es exacta, no hay hilos inactivos.

Tiempo paralelo: 2 152 500 por 10 elevado a menos 8, unos 0,02 segundos. Tiempo secuencial: eso por 67 millones de píxeles, aproximadamente 1,44 millones de segundos, unos 17 días. El speedup es aproximadamente 67 millones, el número de píxeles. La velocidad por hilo es 2 millones de operaciones en 0,02 segundos, 0,1 gigaFLOP por segundo; si el pico es 1 gigaFLOP por segundo, la eficiencia de la GPU es 0,1.

Comentario para el examen: 2 millones de operaciones por hilo es mucho trabajo; se podría repartir el bucle de escenarios entre varios hilos de un mismo bloque y combinar resultados en memoria compartida. Eso agrega sincronización, pero el costo es pequeño frente a 2 millones de operaciones.

## 7. Ejemplo resuelto: Value at Risk y la trampa del bloque (final 2025-II)

Cada modelo tiene 256 escenarios y cada escenario recorre 256 activos con 2 operaciones por activo. Un hilo hace 512 operaciones; con 10 elevado a menos 8 segundos por operación tarda 5,12 microsegundos. Si cada hilo resuelve un escenario caben 2 elevado a 33 modelos, se usan todos los hilos y la eficiencia es 1.

La variante: los escenarios de un modelo deben compartir memoria, así que un modelo entero tiene que vivir en un bloque. Un bloque tiene 1024 hilos pero el modelo solo usa 256. Caben tantos modelos como bloques, 2 elevado a 31, y tres de cada cuatro hilos quedan ociosos. La eficiencia frente a la capacidad cae a un cuarto. La solución sería empaquetar cuatro modelos en cada bloque.

## 8. Memoria compartida y la forma del bloque (final 2025-I)

En un filtro de 3 por 3, cada píxel necesita sus ocho vecinos. Un bloque carga en memoria compartida su porción de imagen más un borde de un píxel alrededor, llamado halo. Para un bloque cuadrado de lado B, la memoria es B al cuadrado más aproximadamente 4 por B. Para 16 por 16 son 320 bytes; para 256 por 256 son 66 560 bytes, que todavía caben en 100 kilobytes; 512 por 512 ya no cabe.

¿Por qué un bloque cuadrado es mejor que uno rectangular? Para el mismo número de píxeles, el cuadrado tiene el menor perímetro, y el halo es proporcional al perímetro. Un bloque de 16 por 16 tiene 64 píxeles de borde; uno de 32 por 8 tiene 80; uno de 64 por 1 tiene 130. Menos halo es menos memoria y menos lecturas repetidas.

Un detalle que suma puntos: un bloque de 256 por 256 píxeles cabe en memoria, pero son 65 536 píxeles y un bloque solo puede tener 1024 hilos. Cada hilo tendría que procesar 64 píxeles.

## 9. El método de Foster aplicado a la GPU

Foster propone cuatro pasos para diseñar un algoritmo paralelo: particionar, comunicar, aglomerar y mapear.

En el problema de incendios: la partición es un píxel por tarea, 67 millones de tareas primitivas. No hay comunicación entre tareas, porque cada hilo solo lee los datos de su propio píxel; la única comunicación es con la memoria global y entre CPU y GPU. La aglomeración es agrupar los hilos en bloques de 256, que es múltiplo del warp, da buena ocupación y permite accesos coalescentes. El mapeo lo hace la propia GPU: asigna los bloques a los multiprocesadores y los ejecuta por oleadas.

Las reglas generales de Foster: tener al menos diez veces más tareas que procesadores, tareas de tamaño parecido, que el número de tareas crezca con el problema, comunicación balanceada y con pocos vecinos.

## 10. Métricas generales que también entran

- Speedup: tiempo secuencial entre tiempo paralelo. Normalmente es menor o igual al número de procesadores.
- Eficiencia: speedup entre procesadores. Si es 1, cada procesador trabaja todo el tiempo.
- Costo: procesadores por tiempo paralelo. Un algoritmo es de costo óptimo si su costo es del mismo orden que el tiempo secuencial.
- Ley de Amdahl, o escalamiento fuerte: con un problema de tamaño fijo, el speedup nunca supera 1 entre la fracción secuencial. Si el 10 % del código es secuencial, el speedup máximo es 10, aunque haya mil procesadores.
- Ley de Gustafson, o escalamiento débil: si el problema crece con el número de procesadores, el speedup crece casi linealmente.
- Más MIPS no significa menos tiempo: en el ejemplo de clase, un compilador con 1600 MIPS tardaba 7,5 segundos y otro con 1400 MIPS tardaba 5 segundos, porque generaba menos instrucciones.

## 11. Errores comunes

- Confundir píxeles por bloque con hilos por bloque.
- Olvidar multiplicar por los dos bucles al contar operaciones.
- Calcular el tiempo secuencial con todos los hilos de la GPU en lugar de los hilos que realmente trabajan.
- Dar el número sin comentar si hay hilos ociosos o si el trabajo por hilo es excesivo.
- No trabajar en potencias de 2: dividir 2 elevado a 26 entre 2 elevado a 8 es restar exponentes, 2 elevado a 18.

## 12. Resumen en cinco frases

1. La GPU tiene 2 elevado a 41 hilos disponibles en el problema típico del examen.
2. El tiempo paralelo es el tiempo de un hilo; el secuencial es ese tiempo por los hilos que trabajan.
3. El speedup sale igual al número de hilos que trabajan y la eficiencia cerca de 1, salvo que haya hilos ociosos.
4. La memoria compartida es por bloque; los bloques cuadrados minimizan el halo.
5. Siempre cierra con un comentario sobre carga por hilo, hilos ociosos y uso de la GPU.
