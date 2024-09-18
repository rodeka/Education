#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

void swap(int *a, int *b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
    return;
}

void printM(int* M, int len){
    for(int i = 0; i<len; i++)
        printf("%d ", M[i]);
    printf("\n");
    return;
}

void sort_shell(int* M, int len) {
    for (int d = len / 2; d != 0; d /= 2) {
        for (int i = d; i < len; ++i) {
            for (int j = i - d; j>=0 && M[j]>M[j+d]; j -= d) {
                swap(&M[j], &M[j+d]);
            }
        }
    }
    return;
}

int leonardo(int k){
    if (k < 2)
        return 1;
    return leonardo(k - 1) + leonardo(k - 2) + 1;
}

void heapify(int* M, int s, int e){
    int i = s;
    int j = 0;
    int k = 0;

    while (k < e - s + 1){
        if (k & 0xAAAAAAAA) {
            j += i;
            i >>= 1;
        }
        else {
            i += j;
            j >>= 1;
        }
        ++k;
    }

    while (i > 0) {
        j >>= 1;
        k = i + j;
        while (k < e) {
            if(M[k] > M[k-i])
                break;
            swap(&M[k], &M[k-i]);
            k += i;
        }
        i = j;
    }
}

void sort_smooth(int* M, int len){
    int p = len - 1;
    int q = p;
    int r = 0;

    while (p > 0) {
        if ((r & 0x03) == 0)
            heapify(M, r, q);
        if (leonardo(r) == p){
            ++r;
        }
        else{
            --r;
            q -= leonardo(r);
            heapify(M, r, q);
            q = r-1;
            --r;
        }
        swap(&M[0], &M[p]);
        --p;
    }

    for (int i = 0; i < len-1; ++i) {
        for (int j = i + 1; j > 0 && M[j] < M[j-1]; --j) {
            swap(&M[j], &M[j-1]);
        }
    }
    return;
}

void sort_shaker(int* M, int len){
    int ctrl=len-1;
    int l=0, r=ctrl;
    do{
        for(int i=l; i<r; i++){
            if(M[i]>M[i+1]){
                swap(&M[i], &M[i+1]);
                ctrl=i;
            }
        }
        r=ctrl;
        for(int i=r; i>l; i--){
            if(M[i]<M[i-1]){
                swap(&M[i], &M[i-1]);
                ctrl=i;
            }
        }
        l=ctrl;
    } while(l<r);

    return;
}

void shuffle(int* M, int len){
    for(int i=0; i<len; i++)
        swap(&M[i], &M[(i+rand())%len]);
    
    return;
}

void test_sort(int* M, int len, void  (*sort)(int*, int)){
    shuffle(M, len);
    sort(M, len);
    printM(M, len);
    return;
}

int main(){
    srand(0);

    int len=10;
    int M[len];
    for(int i=0; i<len; i++)
        M[i]=i;

    void (*sorts[])(int*, int) = {sort_shell, sort_smooth, sort_shaker};
    int size = sizeof(sorts)/sizeof(sorts[0]);

    clock_t start, end;
    for(int i=0; i<size; i++){
        start = clock();
        for(int j=0; j<10; j++)
            test_sort(M, len, sorts[i]);
        end = clock();
        printf("time to sort an array of size %d numbers size 1000 times : %ld clocks\n", len, (end-start));
    }
    return 0;
}