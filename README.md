# Optimización de computación con matrices mediante diferentes técnicas

## Introducción
Este estudio ha sido realizado para aprender de forma didáctica diferentes técnicas de optimización en C. El desarrollo de dicho estudio ha sido realizado por [Pablo Souto Soneira](https://github.com/Pabloo-ss) y yo, Nicolás Vilela Pérez.

El repositorio está formado por cuatro archivos de código en C correspondientes a la [versión inicial](inicial.c) y a las versiones tras aplicar diferentes técnicas de optimización, y el [informe](Informe.pdf) asociado al estudio completo de las optimizaciones.

## Optimizaciones usadas
* __Mejoras de caché:__ modificación del código inicial tras realizar desenrrollamiento de bucles o *unrolling*. Código disponible en [`mejoras_cache.c`](mejoras_cache.c).

* __Extensiones vectoriales:__ uso de procesamiento vectorial SIMD mediante extensiones SSE3. Código disponible en [`extensiones_vectoriales.c`](extensiones_vectoriales.c).

* __OpenMP:__ uso de OpenMP para paralelizar la versión con mejoras caché, sin utilizar extensiones vectoriales, y variando el número de hilos hasta el máximo de cores del PC. Código disponible en [`openmp.c`](openmp.c).
