# Bloque 2: PRAM, estructuras de datos y algoritmos paralelos

Curso: CS4052 Computación Paralela y Distribuida (UTEC), ciclo 2026-II.
Objetivo: resolver la pregunta 2 del examen final, que pide diseñar un algoritmo paralelo para una estructura de datos o un problema de búsqueda y analizarlo. Vale entre 5 y 7 puntos.

> Instrucción sugerida para el video en NotebookLM: "Enseña este material como una clase. Empieza explicando trabajo y span con una analogía, luego el teorema de Brent, y resuelve paso a paso el parallel stack, el árbol k-ario con su k óptimo y la comparación DTS contra PWS. Habla en español."

---

## 1. Qué tiene que saber hacer el estudiante

Las preguntas 2 de los finales anteriores fueron:

- 2025-I: un árbol 2-3 (buscar, insertar un elemento, insertar una secuencia).
- 2025-II sección 1: un stack paralelo al que muchos hilos hacen push y pop.
- 2025-II sección 2: buscar el máximo en un árbol k-ario con memoria distribuida.
- 2026-I: comparar dos algoritmos de búsqueda, DTS y PWS.

El guion casi siempre es el mismo: escribir el pseudocódigo paralelo (llamado PRAM), calcular el span, calcular el trabajo, aplicar el teorema de Brent para obtener el tiempo con p procesadores, agregar la comunicación con MPI o la sincronización con OpenMP, y calcular overhead e isoeficiencia.

## 2. Trabajo y span: las dos medidas básicas

Imagina que hay que preparar un almuerzo. Hay tareas que se pueden hacer a la vez, como picar verduras y hervir agua, y otras que dependen de las anteriores, como servir cuando todo está cocinado.

Ese almuerzo se puede dibujar como un grafo dirigido sin ciclos, un DAG. Cada nodo es una tarea y cada flecha es una dependencia.

El trabajo, W, es la cantidad total de tareas. Es lo que tardaría una sola persona haciendo todo, es decir, el tiempo secuencial.

El span, T infinito, es la cadena más larga de tareas que dependen una de otra: el camino crítico. Es lo que tardaría el almuerzo con infinitas personas ayudando, porque la cadena de dependencias no se puede acortar.

De ahí salen dos límites: con p procesadores, el tiempo nunca baja de W entre p (no se puede repartir mejor que en partes iguales) ni de T infinito (no se puede romper la cadena crítica). El speedup máximo posible es W entre T infinito.

Ejemplo de clase: sumar 16 números con 8 procesadores tiene trabajo 15 sumas y span 4 niveles. Speedup 15 entre 4 y eficiencia 0,47. Con 5 procesadores el span sube a 5, el speedup baja a 3, pero la eficiencia sube a 0,6 porque se desperdician menos procesadores.

## 3. El modelo PRAM

PRAM significa Parallel Random Access Machine. Es un modelo teórico con p procesadores idénticos que ejecutan sincrónicamente y se comunican solo a través de una memoria compartida. Ignora cachés, latencias y costos de comunicación. Sirve para diseñar el algoritmo pensando primero en el paralelismo ideal.

El pseudocódigo usa la palabra pardo, que significa "hacer en paralelo". Por ejemplo, "para i de 1 a n pardo, B de i igual a A de i" significa que los n procesadores copian sus elementos al mismo tiempo.

Hay variantes según si se permite leer o escribir la misma posición de memoria a la vez:

- EREW: lectura y escritura exclusivas. Es el más restrictivo.
- CREW: lectura concurrente, escritura exclusiva. Por ejemplo, todos leen el mismo valor x para calcular B de i igual a A de i más x.
- CRCW: lectura y escritura concurrentes. Por ejemplo, el OR global, donde todos los que ven un 1 escriben 1 en el resultado al mismo tiempo.

Con escrituras concurrentes hay que decidir qué valor queda: el modelo común (todos escriben lo mismo), arbitrario, combinado (se acumulan) o prioritario.

## 4. El teorema de Brent

Brent responde la pregunta: si conozco el trabajo y el span de un algoritmo, ¿cuánto tarda con p procesadores reales?

La respuesta: el tiempo con p procesadores es como máximo W entre p más T infinito.

La idea de la demostración: en el paso i del algoritmo hay W sub i operaciones. Con p procesadores se hacen en W sub i entre p pasos, redondeado hacia arriba, lo que es como máximo W sub i entre p más 1. Sumando sobre todos los pasos, que son T infinito, sale W entre p más T infinito.

Ejemplo de clase: con trabajo 1000 y span 20, diez procesadores tardan unos 120 pasos; cien procesadores tardan unos 30. Ya no baja mucho más, porque el span de 20 es un piso.

El costo es procesadores por tiempo. Un algoritmo es de costo óptimo si su costo es del mismo orden que el tiempo secuencial. Con Brent, eso pasa cuando el número de procesadores es del orden de W entre T infinito.

## 5. Algoritmos base que hay que dominar

**OR global:** con n procesadores en CRCW se resuelve en tiempo constante; cada procesador que ve un 1 escribe 1.

**Máximo en tiempo constante:** con n al cuadrado procesadores, cada par compara A de i con A de j. El máximo es el elemento cuya fila de comparaciones es toda verdadera. Trabajo n al cuadrado, span constante, requiere CRCW.

**Suma por reducción:** en cada nivel se suman pares, B de i igual a B de 2i menos 1 más B de 2i. Hay log n niveles. Trabajo n, span log n. Con Brent, tiempo n entre p más log n.

**Suma de prefijos:** igual que la reducción pero con una fase de subida y otra de bajada para obtener todas las sumas parciales. Trabajo n, span log n.

**Pointer jumping:** cada nodo reemplaza su sucesor por el sucesor de su sucesor. En cada paso la distancia saltada se duplica, así que en log h pasos se llega a la raíz.

Recurrencias que aparecen siempre:

- T de n igual a T de n medios más una constante: log n.
- T de n igual a T de n medios más n: orden n.
- T de n igual a 2 T de n medios más n: n log n.
- T de n igual a T de n medios más log n: log al cuadrado de n.

## 6. Ejemplo resuelto: el parallel stack (final 2025-II)

Muchos hilos comparten un stack. Cada hilo saca un elemento, lo procesa y vuelve a meter un resultado. El pseudocódigo es: para cada elemento en paralelo, bloquear, hacer pop y revisar si el stack está lleno o vacío, desbloquear; procesar el elemento (esto sí en paralelo); bloquear, hacer push, revisar, desbloquear.

El punto clave es que el acceso al stack es secuencial: solo un hilo a la vez puede estar entre el bloqueo y el desbloqueo. Si hay n elementos, los accesos suman n pasos aunque haya p hilos. Revisar el estado cuesta del orden de p. El tiempo paralelo es n más p más el cómputo entre p.

Overhead: p por el tiempo paralelo, menos el trabajo útil, da n por p más p al cuadrado. Para que escale, el trabajo útil tendría que crecer como n por p. Es un diseño difícilmente escalable, salvo que el procesamiento de cada elemento sea muy pesado.

En OpenMP se usa una sección crítica dentro de push, pop y la revisión del estado, y en el programa principal una región paralela con secciones, compartiendo el arreglo y el índice del tope.

## 7. Ejemplo resuelto: máximo en un árbol k-ario (final 2025-II)

En lugar de reducir por pares, se reduce de a k elementos: en cada ronda cada grupo de k deja su máximo, y el número de elementos se divide entre k. El span es logaritmo en base k de n.

Con memoria distribuida: primero se reparten n entre p elementos a cada proceso con un Scatter, cada proceso calcula su máximo local, y luego los procesos forman un árbol k-ario. En cada nivel cada padre recibe un mensaje de cada uno de sus k menos 1 hijos. Hay logaritmo en base k de p niveles.

Tiempo: n entre p más k por logaritmo en base k de p, por alfa más beta, donde alfa es la latencia y beta el costo por dato.

¿Cuál es el mejor k? Hay un equilibrio: con k pequeño hay muchos niveles; con k grande cada padre recibe muchos mensajes. El término a minimizar es k por logaritmo en base k de p, que es igual a k entre logaritmo natural de k, por logaritmo natural de p. Derivando k entre ln k e igualando a cero se obtiene ln k igual a 1, es decir, k igual a e, aproximadamente 2,7. El entero más cercano es 3.

## 8. Ejemplo resuelto: DTS contra PWS (final 2026-I)

Se busca en un árbol con factor de ramificación b y profundidad d; el árbol tiene b elevado a d nodos, que es el trabajo W.

DTS reparte subárboles. Primero genera secuencialmente todos los nodos hasta la mitad de la profundidad: b elevado a d medios. Luego cada subárbol, también de tamaño b elevado a d medios, se explora en paralelo. Al final se reducen los resultados, lo que es despreciable. El span es b elevado a d medios, que es la raíz cuadrada de W. Con Brent, el tiempo es W entre p más raíz de W. El overhead es p por raíz de W. Para la isoeficiencia se iguala W a K por p por raíz de W; dividiendo entre raíz de W queda raíz de W igual a K por p, y elevando al cuadrado, W del orden de p al cuadrado.

PWS reparte ventanas de costo. Cada uno de los p procesadores explora desde la raíz con un límite distinto, y por culpa de una heurística que no separa bien las ventanas, cada uno repite trabajo del orden de W elevado a dos tercios. El tiempo es W entre p más W elevado a dos tercios. El overhead es p por W elevado a dos tercios. Despejando, W elevado a un tercio es K por p, y W es del orden de p al cubo.

Conclusión: p al cuadrado crece mucho más lento que p al cubo, así que DTS escala mejor. La causa es que el trabajo redundante de PWS, W a la dos tercios, es mayor que el span de DTS, W a la un medio.

Para recordar de la clase de búsqueda: DTS es bueno con árboles balanceados; PWS es óptimo para búsquedas heurísticas profundas; los clusters combinan ambos (un cluster equivale a DTS, p clusters equivalen a PWS).

## 9. Ejemplo resuelto: árbol 2-3 (final 2025-I)

Un árbol 2-3 tiene nodos internos con dos o tres hijos y todas las hojas a la misma profundidad, así que su altura es del orden de log n.

Buscar: se lanza la búsqueda en todos los hijos en paralelo y se sincroniza. La profundidad de la recursión es la altura, log n.

Insertar un elemento: se ubica su posición comparando en paralelo, lo que es constante. Si el nodo padre queda con cuatro hijos, se parte en dos y el problema sube un nivel. En el peor caso sube hasta la raíz: log n.

Insertar k elementos: cada inserción cambia el árbol, así que van una después de otra: k por log n.

## 10. Del PRAM a MPI y OpenMP

Cuando el examen pide agregar directivas, no hace falta código completo; hay que saber qué directiva va en cada fase.

- Repartir datos: en MPI con Scatter o Broadcast; en OpenMP es implícito, con un for paralelo.
- Calcular localmente: un bucle en cada proceso o un for paralelo con variables privadas.
- Combinar resultados: en MPI con Reduce o con envíos y recepciones en árbol; en OpenMP con reduction o con una sección crítica.
- Proteger una estructura compartida: en OpenMP con critical o con locks.
- Paralelismo recursivo: en OpenMP con task y taskwait.

Un error clásico del final 2025-I: usar atomic para proteger "si A de i es mayor que el máximo, actualizar el máximo". Atomic solo protege una operación simple como sumar o incrementar. Entre la comparación y la escritura otro hilo puede cambiar el valor. Se llama condición de carrera de tipo leer, comprobar y actualizar. La solución correcta es que cada hilo calcule su máximo local y al final los combine en una sección crítica, o mejor, usar reduction con el operador max.

## 11. Modelos más realistas que PRAM

PRAM ignora la comunicación. Las extensiones agregan un aspecto real cada una:

- APRAM: los procesadores avanzan a ritmos distintos y la sincronización es explícita.
- Phase PRAM: el cómputo se divide en fases separadas por barreras, y cada barrera tiene un costo.
- LPRAM: el acceso a memoria local es más barato que a la global.
- BPRAM: transferir un bloque cuesta alfa más beta por el tamaño; mandar n datos sueltos cuesta n por alfa más beta, mucho más que un solo bloque.
- BSP: el programa se divide en supersteps de cómputo, comunicación y barrera; cada uno cuesta w más g por h más L. Una reducción cuesta n entre p más g y L por log p.
- LogP: modelo asíncrono con latencia L, overhead o, gap g y P procesadores. Una reducción cuesta n entre p más log p por L más o más g.

## 12. Errores comunes

- Confundir trabajo con span.
- Olvidar que una región crítica serializa: su costo se suma, no se divide entre p.
- Escribir el PRAM sin decir el span o sin justificar de dónde sale.
- Usar atomic para operaciones compuestas.
- En la isoeficiencia, no despejar W cuando aparece en ambos lados de la ecuación.

## 13. Resumen en cinco frases

1. Trabajo es el total de operaciones; span es el camino crítico.
2. Brent: el tiempo con p procesadores es como máximo trabajo entre p más span.
3. Las estructuras compartidas protegidas con critical generan overhead proporcional a n por p.
4. En reducciones k-arias el k óptimo es e, aproximadamente 3.
5. DTS escala como p al cuadrado y PWS como p al cubo cuando PWS tiene trabajo redundante.
