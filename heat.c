#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>

// Using Code given by Professor

#define N 100
#define LIMIT 10000
void print_domain(double domain[N+1][N+1]) {
    int step = floor(N/8), i, j;
    printf("*******************************************************\n");
    for (i = 1; i <= N; i+=step) {
        for (j = 1; j <= N; j+=step)
            printf("%f ", domain[i][j]);
        printf("\n");
    }
    printf("*******************************************************\n\n\n");
}
double get_final_temperature(double radtemp, double fracx, double fracy) {
    double (*t)[N+1][N+1] = malloc(sizeof(double[2][N+1][N+1]));
    int i, j, iter;
    for (i = 0; i <= N; i++)
            for (j = 0; j <= N; j++) {
                t[0][i][j] = 10;
                t[1][i][j] = 10;
            }


    int radstartindex = floor(0.3*N), radendindex = ceil(0.7*N);
    for(i=radstartindex; i<=radendindex; i++) {
        t[0][N][i] = radtemp;
        t[1][N][i] = radtemp;
    }
    for (iter = 0; iter < LIMIT; iter++) {
        int t0 = iter%2, t1 = (iter+1)%2;
        for (i = 1; i < N; i++)
                for (j = 1; j < N; j++)
                    t[t1][i][j] = 0.25 * (t[t0][i-1][j] + t[t0][i+1][j] + t[t0][i][j-1]
                                          + t[t0][i][j+1]);
    }

    int pointx = floor(N*fracx), pointy = floor(N*fracy);
    double result = t[(iter+1)%2][pointx][pointy];
    free(t);
    printf("Radiator temperature of %f leads to target temperature of %f\n",
           radtemp, result);
    return result;
}
