#include <stdio.h>

#define N 1E10
double paso;

int main(int argc, char const *argv[]){
	double x,pi,sum=0.0;
    paso=1.0/N;

	for(int i = 0; i < N; i++){
		x = (i+0.5)*paso;
		sum = sum+4.0/(1.0+x*x);
	}
    pi=paso*sum;
	printf("El resultado es: %f\n", pi);
	return 0;
}