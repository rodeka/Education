#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <float.h>

double getRandomDouble(){
    double res=1.0*rand()/RAND_MAX;
    return res;
}

typedef struct {
    __uint32_t size;
    double** M;
} Graph;

Graph* initGraph(__uint32_t size){
    Graph* graph = malloc(sizeof(Graph));
    graph->size=size;
    graph->M=malloc(graph->size*sizeof(double*));
    for(int i=0; i<graph->size; i++){
        graph->M[i]=malloc(graph->size*sizeof(double));
        for(int j=0; j<graph->size; j++){
            graph->M[i][j]=0;
        }
    }
    return graph;
}


Graph* parseGraph(char* fileName){
    FILE* file = fopen(fileName, "r");
    __uint32_t size;
    fscanf(file, "%d\n", &size);
    Graph* graph = initGraph(size);

    for(int i=0; i<graph->size; i++){
        for(int j=0; j<graph->size; j++){
            fscanf(file, "%le ", &graph->M[i][j]);
        }
    }
    return graph;
}

void copyGraph(Graph* from, Graph* to){
    to->size=from->size;
    for(int i=0; i<to->size; i++){
        for(int j=0; j<to->size; j++){
            to->M[i][j]=from->M[i][j];
        }
    }
}

void printGraph(Graph* graph){
    for(__uint32_t i=0; i<graph->size; i++){
        for(__uint32_t j=0; j<graph->size; j++){
            printf("%0.3f ", graph->M[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

typedef struct
{
    __uint32_t size;
    __uint32_t i;
    __uint32_t* M;
    __uint32_t mask;
    double length;
} Path;

Path* initPath(__uint32_t size){
    Path* path = malloc(sizeof(Path));
    path->size=size;
    path->M=malloc(path->size*sizeof(int));
    path->i=0;
    path->length=0;
    path->mask=0;
    return path;
}

void addPointPath(Path* path, Graph* graph, __uint32_t point){
    path->M[path->i]=point;
    if(path->i!=0)
        path->length+=graph->M[path->M[path->i-1]][path->M[path->i]];
    path->i++;
    path->mask |= 1 << point;
}

void copyPath(Path* from, Path* to){
    for(int i=0; i<from->size; i++){
        to->M[i]=from->M[i];
    }
    to->i=from->i;
    to->length=from->length;
    to->mask=from->mask;
}

void freePath(Path* path){
    free(path->M);
    free(path);
}

void printPath(Path* path){
    for(int i=0; i<path->size; i++){
        printf("%d ", path->M[i]);
    }
    printf("\nlength: %0.3f\ni: %d\nmask: %d\n", path->length, path->i, path->mask);
}

__uint8_t findEdgeinPath(Path* path, __uint32_t x, __uint32_t y){
    for(int i=0; i<path->size-1; i++){
        if(path->M[i]==x && path->M[i+1]==y){
            return 1;
        }
    }
    return 0;
}

Graph* prepareBrute(Graph* graph){
    Graph* graphBrute = initGraph(graph->size);
    copyGraph(graph, graphBrute);
    for(int i=0; i<graphBrute->size; i++){
        for(int j=0; j<graphBrute->size; j++){
            if(graphBrute->M[i][j]==0){
                graphBrute->M[i][j]=__INT32_MAX__/(graphBrute->size+1);
            }
        }
    }
    return graphBrute;
}

Path* bruteForce(Graph* graph, ...){
    va_list args;
    va_start(args, graph);
    Path* path = va_arg(args, Path*);
    Path* answer = va_arg(args, Path*);
    va_end(args);
    if(path==NULL || answer==NULL){
        answer = initPath(graph->size+1);
        answer->length=__INT32_MAX__;
        path = initPath(graph->size+1);
    }
    if(path->i==graph->size){
        addPointPath(path, graph, path->M[0]); 
        if(answer->length > path->length){
            copyPath(path, answer);
        }
    }
    else {
        for(int i=0; i<graph->size; i++){
            if((path->mask >> i) & 1)
                continue;
            Path* newpath=initPath(path->size);
            copyPath(path, newpath);
            addPointPath(newpath, graph, i);
            bruteForce(graph, newpath, answer);
        }
    }
    freePath(path);
    return answer;
}

Path* AntAlgorithm(Graph* graph, double alpha, double beta, double Q, double p){
    Graph* graphFeromon=initGraph(graph->size);
    Graph* graphFeromonCopy=initGraph(graph->size);
    Graph* graphInvDistance=initGraph(graph->size);

    for(__uint32_t i=0; i<graph->size; i++){
        for(__uint32_t j=0; j<graph->size; j++){
            graphFeromon->M[i][j]=0.5;
            if(graph->M[i][j]!=0.0)
                graphInvDistance->M[i][j]=1/graph->M[i][j];
        }
    }

    Path* answer = initPath(graph->size+1);
    answer->length=__INT32_MAX__;
    int count=100;

    while (count--){
        Path** ants=malloc(graph->size*sizeof(Path*));
        for(__uint32_t i=0; i<graph->size; i++){
            ants[i]=initPath(graph->size+1);
            addPointPath(ants[i], graph, i);
            while(ants[i]->i != ants[i]->size -1){
                double total=0.0;
                for(int j=0; j<graph->size; j++){
                    if((ants[i]->mask >> j)&1)
                        continue;
                    if(graph->M[ants[i]->M[ants[i]->i-1]][j]==0.0)
                        continue;
                    total+=pow(graphFeromon->M[ants[i]->M[ants[i]->i-1]][j], alpha)*pow(graphInvDistance->M[ants[i]->M[ants[i]->i-1]][j], beta);
                }
                if(total==0.0)
                    break;

                double random = getRandomDouble();
                double P = 0.0;
                for(int j=0; j<graph->size; j++){
                    if((ants[i]->mask >> j)&1)
                        continue;
                    if(graph->M[ants[i]->M[ants[i]->i-1]][j]==0.0)
                        continue;
                    P+=pow(graphFeromon->M[ants[i]->M[ants[i]->i-1]][j], alpha)*pow(graphInvDistance->M[ants[i]->M[ants[i]->i-1]][j], beta)/total;
                    if(P>random){
                        addPointPath(ants[i], graph, j);
                        break;
                    }
                }
            }
            if((ants[i]->i != ants[i]->size -1))
                continue;
            addPointPath(ants[i], graph, i);
            if(ants[i]->length < answer->length){
                copyPath(ants[i], answer);
            }
        }
        copyGraph(graphFeromon, graphFeromonCopy);
        for(__uint32_t i=0; i<graphFeromon->size; i++){
            for(__uint32_t j=0; j<graphFeromon->size; j++){
                double dFeromon=0;
                for(__uint32_t k=0; k<graph->size; k++){
                    if(ants[k]->i!=ants[k]->size)
                        continue;
                    if(findEdgeinPath(ants[k], i, j)){
                        dFeromon = dFeromon + Q / ants[k]->length;
                    }
                }
                graphFeromon->M[i][j]=(1-p)*graphFeromonCopy->M[i][j]+dFeromon;
            }
        }
        printGraph(graphFeromon);
        for(__uint32_t i=0; i<graph->size; i++){
            free(ants[i]);
        }
        free(ants);

    }
    free(graphFeromon);
    free(graphFeromonCopy);
    free(graphInvDistance);
    
    return answer;
}

void freeAll(Graph* graph1, Graph* graph2, Path* path1, Path* path2){
    for(int i = 0; i < graph1->size; i++){
        free(graph1->M[i]);
        free(graph2->M[i]);
    }
    free(graph1->M);
    free(graph1);
    free(graph2->M);
    free(graph2);
    freePath(path1);
    freePath(path2);
}

int main(){
    Graph* graph;
    graph = parseGraph("table.txt");
    printGraph(graph);
    Graph* graphBrute = prepareBrute(graph);
    Path* ansBrute = bruteForce(graphBrute, 0);
    printPath(ansBrute);

    int alpha=2.0, beta=1.0;
    double Q=5.0, p=0.1;
    Path* ansAnt = AntAlgorithm(graph, alpha, beta, Q, p);
    printPath(ansAnt);
    freeAll(graph, graphBrute, ansBrute, ansAnt);
    return 0;
}