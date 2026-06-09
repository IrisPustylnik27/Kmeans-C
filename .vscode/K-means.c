#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

/*cheacking integers validation*/
bool parse_strict_integer(const char *str, int *out_value)
{
    char *endptr;
    double val = strtod(str, &endptr);
    if (str == endptr)
    {
        return false;
    }
    if (*endptr != '\0' && *endptr != '\n')
    {
        return false;
    }
    if (val != (int)val)
    {
        return false;
    }
    *out_value = (int)val;
    return true;
}

/*calculating the euclidian distance between two vectors*/
float calc_euclidian_distance(float *vec1, float *vec2, int d)
{
    float sum = 0.0;
    for (int i = 0; i < d; i++)
    {
        sum += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
    }
    return sqrtf(sum);
}

int main(int argc, char *argv[])
{
    int N = 0, K = 0, d = 0;
    size_t len = 0;
    float *pVector = NULL, **pCentroids = NULL, **pListOfVec = NULL;
    char *pLine = NULL;
    int iter = 2;
    float static epsilon = 0.001;
    int capacity = 5, capacityVecList = 5;
    pVector = malloc(capacity * sizeof(float));
    pListOfVec = malloc(capacityVecList * sizeof(float *));

    if (argc > 4 || argc < 2)
    {
        fprintf(stderr, "wrong number of arguments!\n");
        return 1;
    }

    if (!parse_strict_integer(argv[1], &K))
    {
        fprintf(stderr, "Invalid K value!\n");
        return 1;
    }
    K = atoi(argv[1]);
    if (argc == 4)
    {
        if (!parse_strict_integer(argv[2], &iter))
        {
            fprintf(stderr, "Invalid iteration value!\n");
            return 1;
        }
        iter = atoi(argv[2]);
        char *filename = argv[3];
    }
    else
    {
        iter = 400;
        char *filename = argv[2];
    }

    if (iter > 799 || iter < 2)
    {
        printf("Incorrect maximum iteration!\n");
        return 0;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open or find file '%s'\n", filename);
        return 1;
    }

    /*read the vectors*/
    while ((getline(&pLine, &len, file)) != -1)
    {
        char *token = strtok(pLine, ",");
        /* initialize the vector */
        while (token != NULL)
        {
            if (d == capacity)
            {
                capacity *= 2;
                pVector = realloc(pVector, capacity * sizeof(float));
            }
            pVector[d] = atof(token);
            d++;
            token = strtok(NULL, ",");
        }
        /*add the vector to the list*/
        if (N == capacityVecList)
        {
            capacityVecList *= 2;
            float **temp = realloc(pListOfVec, capacityVecList * sizeof(float *));
            if (temp == NULL)
            {
                /*free memory*/
                for (int i = 0; i < capacityVecList; i++)
                {
                    free(pListOfVec[i]);
                }
                free(pListOfVec);
                return 1;
            }
            pListOfVec = temp;
        }
        pListOfVec[N] = realloc(pVector, d * sizeof(float));
        N++;
    }

    if (N <= K)
    {
        printf("Number of clusters must be less than number of vectors!\n");
        return 1;
    }

    free(pLine);
    fclose(file);

    /*fun begins*/
    int counter = 0;
    float deltaCentroids = 0.0;
    int *assignmentsToClusters = malloc(N * sizeof(int));
    bool isConverged = false;

    /*initialize the centroids*/
    pCentroids = malloc(K * sizeof(float *));
    for (int i = 0; i < K; i++)
    {
        pCentroids[i] = realloc(pListOfVec[i], d * sizeof(float));
        assignmentsToClusters[i] = i;
    }

    while (counter <= iter && !isConverged)
    {
        /* step 1: assign vectors to clusters */
        for (int i = 0; i < N; i++)
        {
            float minDistance = calc_euclidian_distance(pListOfVec[i], pCentroids[0], d);
            int closestCentroid = 0;
            for (int j = 1; j < K; j++)
            {
                float distance = calc_euclidian_distance(pListOfVec[i], pCentroids[j], d);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    closestCentroid = j;
                }
            }
            assignmentsToClusters[i] = closestCentroid;
        }
        /* step 2: update centroids and calculate the difference */
        for (int j = 0; j < K; j++)
        {
            int total = 0;
            float *newCentroid = calloc(d, sizeof(float));
            float *oldCentroid = realloc(pCentroids[j], d * sizeof(float));
            for (int i = 0; i < N; i++)
            {
                if (assignmentsToClusters[i] == j)
                {
                    for (int k = 0; k < d; k++)
                    {
                        newCentroid[k] += pListOfVec[i][k];
                    }
                    total++;
                }
            }
            for (int i = 0; i < d; i++)
            {
                newCentroid[i] = newCentroid[i] / abs(total);
            }
            deltaCentroids = calc_euclidian_distance(newCentroid, oldCentroid, d);
            if (deltaCentroids < epsilon)
            {
                isConverged = true;
            }
            pCentroids[j] = realloc(newCentroid, d * sizeof(float));
            free(oldCentroid);
        }
        counter++;
    }
    free(assignmentsToClusters);
    free(pVector);
    for (int i = 0; i < N; i++)
    {
        free(pListOfVec[i]);
    }
    free(pListOfVec);
    for (int i = 0; i < K; i++)
    {
        free(pCentroids[i]);
    }
    free(pCentroids);
    return 0;
}