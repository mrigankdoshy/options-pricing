#include <stdio.h>
#include <math.h>
#include <string.h>
#include "mt19937ar.h"
#define N 1000000      // digit
 
int main(void){
    FILE *fp;
    double x, y;
    int z;
    int i;
    int cnt = 0;
    
    init_genrand(4234);
    fp = fopen("data.dat", "w+");
    
    for(i=1; i<=N; i++){
        x = (float)genrand_real1();
        y = (float)genrand_real1();
        
        if(x*x+y*y<=1){
            z = 1;    //in
            cnt++;
        } else {
            z = 0;    //out
        }
        fprintf(fp, "%.6f\t%.6f\t%1d\t%7d\n",x, y, z, cnt);
    }
 
    fclose(fp);
    return 0;
}