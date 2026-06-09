#define _GNU_SOURCE
# include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*cheacking integers validation*/
bool parse_strict_integer(const char *str, int *out_value) {
    char *endptr;
    double val = strtod(str, &endptr);
    if (str == endptr) {
        return false;
    }
    if (*endptr != '\0' && *endptr != '\n') {
        return false;
    }
    if (val != (int)val) {
        return false; 
    }
    *out_value = (int)val;
    return true;
}


int main(int argc, char *argv[]) {
    int N=0, K=0, d=0;
    size_t len=0;
    float **pListOfVec=NULL;
    float *pVector=NULL, *pCentroid=NULL;
    char *pLine=NULL;
    int iter=2;
    float static epsilon = 0.001;
    int capacity = 5, capacityVecList = 5; 
    pVector = malloc(capacity * sizeof(float));
    pListOfVec = malloc(capacityVecList * sizeof(float*));

    if (argc > 3 || argc < 2) {
		fprintf (stderr, "wrong number of arguments!\n");
		return 1;
	}
    
    if (!parse_strict_integer(argv[1], &K)) {
        fprintf(stderr, "Invalid K value!\n");
        return 1;
    }
    K = atoi(argv[1]);
    if(argc == 3){
        if (!parse_strict_integer(argv[2], &iter)) {
            fprintf(stderr, "Invalid iteration value!\n");
            return 1;
        }
        iter = atoi(argv[2]);
    }
    else{
        iter=400;
    }

    if(iter > 799 || iter < 2){
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
    /*read the vectors*/
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
            float **temp = realloc(pListOfVec, capacityVecList * sizeof(float*));
            if (temp == NULL) {
                /*free memory*/
                for (int i = 0; i < capacityVecList; i++) {
                    free(pListOfVec[i]);
                }
                free(pListOfVec);
                return 1;
            }
            pListOfVec = temp;
        }
        pListOfVec[N] = realloc(pVector, d * sizeof(float));
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