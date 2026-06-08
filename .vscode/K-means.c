#define _GNU_SOURCE
# include <stdio.h>

int main(){
    int N=0, K=0, d=0;
    size_t len=0;
    int *pListOfVec=NULL;
    float *pVector=NULL, *pCentroid=NULL;
    char *pLine=NULL;
    int iter=0;
    float static epsilon = 0.001;
    int capacity = 5, capacityVecList = 5; 
    pVector = malloc(capacity * sizeof(float));
    pListOfVec = malloc(capacityVecList * sizeof(int));

    scanf("%d %d ", &K, &iter);

    if(iter > 799){
        printf("Incorrect maximum iteration!\n");
        return 0;
    }

    bool isLast= false;
    /*first line*/
    getline(&pLine, &len, stdin);
    int next_char = fgetc(stdin);
    if (next_char == EOF) {
        isLast = true;
    } 
    else {
        ungetc(next_char, stdin);
    }
    while (!isLast) {
        char *token = strtok(pLine, ",");
        /* initialize the vector */
        while (token != NULL) {
            if (d == capacity) {
                capacity *= 2;
                pVector = realloc(pVector, capacity * sizeof(float));
            }
            pVector[d] = atof(token);
            d++; 
            token = strtok(NULL, ",");
        }
        /*add the vector to the list*/
        if (N == capacityVecList) {
            capacityVecList *= 2;
            pListOfVec = realloc(pListOfVec, capacityVecList * sizeof(int));
        }
        pListOfVec[N] = atoi(pVector[0]); 
        N++;
        /*check if last*/
        next_char = fgetc(stdin);
        if (next_char == EOF) {
            isLast = true;
        } 
        else {
            ungetc(next_char, stdin);
            getline(&pLine, &len, stdin);
        }
    }

    return 0;
}