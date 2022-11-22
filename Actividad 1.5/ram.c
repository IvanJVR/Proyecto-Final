#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdarg.h>

#define N 20000
#define A 6

FILE *fptr;

typedef struct arrayTW{
    //double t[N];
    double w[N];
}ArrayTW;

void iteracion(ArrayTW *x, int index);

int main(){
    fptr=fopen("Euler_RAM.txt","w");

    ArrayTW dataArray[A];
    ArrayTW * pda = dataArray;

    double t1, t2, tiempo;
    const double startTime = omp_get_wtime();
    t1 = omp_get_wtime();

   #pragma omp parallel
    {
     #pragma omp sections
        {
       #pragma omp section
         (void)iteracion(pda, 0);//20//2000900
       #pragma omp section
         (void)iteracion(pda, 1);//200//2000400
       #pragma omp section
         (void)iteracion(pda, 2);//2000//2000700
       #pragma omp section
         (void)iteracion(pda, 3);//20000//2000800
       #pragma omp section
         (void)iteracion(pda, 4);//200000//2000500
       #pragma omp section
         (void)iteracion(pda, 5);//2000000//2000100
        }
    }

    printf("Final\n");

    const double endTime = omp_get_wtime();

    t2 = omp_get_wtime();
    tiempo = t2 - t1;

    printf("tomo (%lf) segundos\n", tiempo);

    for(int i=0;i<A;i++){
      fprintf(fptr,"Para el hilo [ %d ]\n", i + 1);
      for (int j = 0; j < N; j++)
      {
        fprintf(fptr,"%lf\t\n", dataArray[i].w[j]);
      }
    }

    fclose(fptr);
}

void iteracion(ArrayTW *x, int index){
printf("Numero de pasos:%d \n", N);
//fprintf(fptr, "Datos que encuentra el metodo de Euler(variable ind.\t variable dep.\t numero de thread)\n");
    //fprintf(fptr, "%f\t %f\n", a, w);
    double h, w0 = 0.5, a = 0, b = 2, t[N];
    h = (b - a)/N;
    t[0] = a;
    x[index].w[0] = w0;
    for(int i=0;i<N;i++){
        t[i] = a + (h * i);
        x[index].w[i] = x[index].w[i - 1] + h * (1 + t[i-1] * t[i - 1] - x[index].w[i - 1]);
    }
}
