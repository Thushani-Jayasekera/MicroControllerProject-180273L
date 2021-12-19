#include <math.h>

float mean(float *buffer , int n){
    float tot=0.0;
    int c=0;
    for(int i=0; i<n; i++){
        if(buffer[i]>=0){
            c++;
            tot+=buffer[i];
        }
    }
    if(c==-1) return -1;
    return tot/c;
}

float standardDeviation(float* buffer, int n, float mean){
    float std_dev=0.0;
    int c=0;
    for(int i =0; i<n;i++){
        if(buffer[i]>=0){
            c++;
            std_dev+=pow(buffer[i]-mean, 2);
        }
    }
    if(c==-1) return -1;
    return pow(std_dev/c,0.5);
}