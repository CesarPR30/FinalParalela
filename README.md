# Final Paralela · CS4052

Material de repaso para el examen final de **Computación Paralela y Distribuida** (UTEC, 2026-II).

**Web:** https://cesarpr30.github.io/FinalParalela/

## Qué hay aquí

| Recurso | Archivo | Para qué |
|---|---|---|
| **Apuntes para imprimir** | [`pdf/Apuntes_Examen_Final.pdf`](pdf/Apuntes_Examen_Final.pdf) · [`apuntes.html`](apuntes.html) | Cuadernillo de 19 páginas para llevar al examen: recetas por pregunta, ejemplos resueltos, plantillas MPI/OpenMP/PRAM y tablas numéricas |
| Audio de repaso | [`audio/`](audio/) | Guion condensado (13 min) narrado en `.wav`, con su texto en `.txt` |
| Web de repaso | [`index.html`](index.html) | Los 3 bloques con teoría, fórmulas, preguntas de autoevaluación y checklist |
| Exámenes resueltos | [`examenes.html`](examenes.html) | Los 4 finales anteriores; cada subpregunta tiene la resolución oculta y una explicación de cómo hacerla |
| PDFs de repaso | [`pdf/`](pdf/) | Un PDF por bloque (respaldo para imprimir o leer sin conexión) |
| Textos para NotebookLM | [`notebooklm/`](notebooklm/) | Un `.md` por bloque, escrito como guion para generar un video explicativo |
| Videos | [`videos/`](videos/) | Resúmenes en video (NotebookLM): bloques 1, 2 y 3; también se ven dentro de la web |
| Guía PD06 | [`pd06/`](pd06/index.html) · [`pdf/Guia_PD06.pdf`](pdf/Guia_PD06.pdf) | Guía de la práctica dirigida 6 (oral) |
| Material original | [`material/`](material/) | Clases U1.1–U6.4, finales, PD06 y código de clase |

## Los 3 bloques

Los cuatro finales anteriores (2025-I, 2025-II S1, 2025-II S2, 2026-I) tienen la misma estructura de 3 preguntas. Cada bloque prepara una:

1. **GPU, CUDA y métricas** (pregunta 1, 6 pts): hilos y bloques, FLOPs por hilo, tiempo paralelo y secuencial, speedup, eficiencia, shared memory, Foster y Flynn.
2. **PRAM, estructuras y algoritmos** (pregunta 2, 5–7 pts): DAG, work/span, Brent, PRAM EREW/CREW/CRCW, parallel stack, árbol 2-3, árbol k-ario, DTS vs PWS, BSP/LogP.
3. **Escalabilidad, MPI, OpenMP e híbrido** (pregunta 3, 7 pts): overhead, isoeficiencia, filas vs bloques, compartida vs distribuida vs híbrida, topologías, MPI, OpenMP.

## Cómo usar los textos en NotebookLM

1. Crea un cuaderno en [NotebookLM](https://notebooklm.google.com) por bloque.
2. Sube el `.md` del bloque como fuente (puedes sumar el PDF del bloque y las clases correspondientes de `material/clases/`).
3. En *Studio → Resumen en video*, pega la "instrucción sugerida" que está al inicio de cada archivo.

## Activar GitHub Pages

*Settings → Pages → Build and deployment → Source: Deploy from a branch → Branch: `main` / `(root)` → Save.* En uno o dos minutos la web queda en el enlace de arriba.

---

Las clases y los exámenes son material del curso CS4052 (prof. José Fiestas). Las guías, explicaciones y notas de corrección son material de estudio.
