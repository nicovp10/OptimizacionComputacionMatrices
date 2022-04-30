#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <immintrin.h>

#define NUMVECES 10
#define N 250

void start_counter();

double get_counter();

double mhz();


/* Initialize the cycle counter */


static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;


/* Set *hi and *lo to the high and low order bits of the cycle counter.
Implementation requires assembly code to use the rdtsc instruction. */
void access_counter(unsigned *hi, unsigned *lo) {
    asm("rdtsc; movl %%edx,%0; movl %%eax,%1" /* Read cycle counter */
    : "=r" (*hi), "=r" (*lo) /* and move results to */
    : /* No input */ /* the two outputs */
    : "%edx", "%eax");
}

/* Record the current value of the cycle counter. */
void start_counter() {
    access_counter(&cyc_hi, &cyc_lo);
}

/* Return the number of cycles since the last call to start_counter. */
double get_counter() {
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;

    /* Get cycle counter */
    access_counter(&ncyc_hi, &ncyc_lo);

    /* Do double precision subtraction */
    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double) hi * (1 << 30) * 4 + lo;
    if (result < 0) {
        fprintf(stderr, "Error: counter returns neg value: %.0f\n", result);
    }
    return result;
}

double mhz(int verbose, int sleeptime) {
    double rate;

    start_counter();
    sleep(sleeptime);
    rate = get_counter() / (1e6 * sleeptime);
    if (verbose)
        printf("\n Processor clock rate = %.1f MHz\n", rate);
    return rate;
}

void barajar(int *v, int tam) {
    int aux, ran;

    srand(0);

    for (int i = 0; i < tam; i++) {
        ran = rand() % tam;
        aux = v[i];
        v[i] = v[ran];
        v[ran] = aux;
    }
}

void quicksort(double *array, int primerInd, int ultimoInd) {
    int i, j, pivote;
    double temp;

    if (primerInd < ultimoInd) {
        pivote = primerInd;
        i = primerInd;
        j = ultimoInd;

        while (i < j) {
            while (array[i] <= array[pivote] && i < ultimoInd)
                i++;
            while (array[j] > array[pivote])
                j--;
            if (i < j) {
                temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }

        temp = array[pivote];
        array[pivote] = array[j];
        array[j] = temp;
        quicksort(array, primerInd, j - 1);
        quicksort(array, j + 1, ultimoInd);
    }
}


int main() {
    int ind[N];
    double **a = NULL, **b = NULL, **bt = NULL, *c = NULL, **d = NULL, *e = NULL;
    double clocks[NUMVECES];
    double f;
    int i, j, k, l;


    // Reserva dinámica de memoria
    a = malloc(N * sizeof(double *));
    for (i = 0; i < N; i++) {
        a[i] = _mm_malloc(8 * sizeof(double), 16);
    }

    b = malloc(8 * sizeof(double *));
    for (i = 0; i < 8; i++) {
        b[i] = _mm_malloc(N * sizeof(double), 16);
    }

    bt = malloc(N * sizeof(double *));
    for (i = 0; i < N; i++) {
        bt[i] = _mm_malloc(8 * sizeof(double), 16);
    }

    c = _mm_malloc(8 * sizeof(double), 16);

    d = malloc(N * sizeof(double));
    for (i = 0; i < N; i++) {
        d[i] = _mm_malloc(N * sizeof(double), 16);
    }

    e = malloc(N * sizeof(double));


    // Inicialización aleatoria de a, b y c
    srand(0);
    for (i = 0; i < N; i++) {
        for (j = 0; j < 8; j++) {
            a[i][j] = (double) (rand() % 50) / 53;
        }
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < N; j++) {
            b[i][j] = (double) (rand() % 50) / 53;
        }
    }

    for (i = 0; i < 8; i++) {
        c[i] = (double) (rand() % 50) / 53;
    }

    for(i = 0; i < 8; i++){
        for(j = 0; j < N; j++){
            bt[j][i] = b[i][j];
        }
    }

    for (i = 0; i < N; i++)
        ind[i] = i;
    barajar(ind, N);


    printf("\n");
    for (l = 0; l < NUMVECES; l++) {
        f = 0;

        start_counter();

	__m128d rA, rB, rC, rD, vec2, valor;
   	vec2 = _mm_set_pd(2.0, 2.0);
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j += 2) {
                rD = _mm_set_pd(0.0, 0.0);
                _mm_store_pd(d[i] + j, rD);
                for (k = 0; k < 8; k += 2) {
                    rA = _mm_load_pd(a[i] + k);
                    rB = _mm_load_pd(bt[j] + k);
                    rC = _mm_load_pd(c + k);

                    valor = _mm_sub_pd(rB,  rC);
                    valor = _mm_mul_pd(rA, valor);
                    valor = _mm_mul_pd(valor, vec2);

                    rD = _mm_add_pd(rD, valor);


                    rB = _mm_load_pd(bt[j + 1] + k);

                    valor = _mm_sub_pd(rB, rC);
                    valor = _mm_mul_pd(rA, valor);
                    valor = _mm_mul_pd(valor, vec2);

                    rD = _mm_add_pd(rD, valor);


                    //d[i][j] += 2 * a[i][k] * (b[k][j] - c[k]);
                }
		    _mm_store_pd(d[i] + j, rD);
            }
        }

        for (i = 0; i < N; i++) {
            e[i] = d[ind[i]][ind[i]] / 2;
            f += e[i];
        }

        clocks[l] = get_counter();

        printf("[Rep. %d] Ciclos: %1.10lf\tf = %lf\n", l, clocks[l], f);
    }
    printf("\n");


    /* Esta rutina imprime a frecuencia de reloxo estimada coas rutinas start_counter/get_counter */
    mhz(1, 1);

    quicksort(clocks, 0, NUMVECES);
    printf("\nMediana de ciclos: %lf\n\n", (clocks[4] + clocks[5]) / 2);

    for (i = 0; i < N; i++) {
        free(a[i]);
    }
    free(a);

    for (i = 0; i < 8; i++) {
        free(b[i]);
    }
    free(b);
    
    for (i = 0; i < N; i++) {
        free(bt[i]);
    }
    free(bt);

    free(c);

    for (i = 0; i < N; i++) {
        free(d[i]);
    }
    free(d);

    free(e);

    exit(EXIT_SUCCESS);
}
