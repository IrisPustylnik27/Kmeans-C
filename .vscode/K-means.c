#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

/*cheacking integers validation*/
bool parse_strict_integer(const char *str, int *out_value);

/*parsing the arguments and validating them*/
bool parse_arguments(int argc, char *argv[], int *K, int *iter, char **filename);

/*calculating the euclidian distance between two vectors*/
float calc_euclidian_distance(float *vec1, float *vec2, int d);

float **read_vectors_from_file(FILE *file, int *N, int *d);

float **init_centroids(float **pListOfVec, int K, int d);

void assign_vectors_to_clusters(float **pListOfVec, float **pCentroids, int *assignmentsToClusters, int N, int K, int d);

float *update_centroid(float **pListOfVec, int *assignmentsToClusters, int clusterIndex, int N, int d);

int main(int argc, char *argv[])
{
    int N = 0, K = 0, d = 0;
    float *pVector = NULL, **pCentroids = NULL, **pListOfVec = NULL;
    char *filename = NULL;
    int iter = 2;
    const float EPSILON = 0.001;
    pVector = malloc(capacity * sizeof(float));
    pListOfVec = malloc(capacityVecList * sizeof(float *));

    if (!parse_arguments(argc, argv, &K, &iter, &filename))
    {
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open or find file '%s'\n", filename);
        return 1;
    }

    /*read the vectors*/
    pListOfVec = read_vectors_from_file(file, &N, &d);

    if (N <= K)
    {
        printf("Number of clusters must be less than number of vectors!\n");
        return 1;
    }

    fclose(file);

    /*fun begins*/
    int counter = 0;
    float deltaCentroids = 0.0;
    int *assignmentsToClusters = malloc(N * sizeof(int));
    bool isConverged = false;

    /*initialize the centroids*/
    pCentroids = init_centroids(pListOfVec, K, d);

    while (counter <= iter && !isConverged)
    {
        /* step 1: assign vectors to clusters */
        assign_vectors_to_clusters(pListOfVec, pCentroids, assignmentsToClusters, N, K, d);

        /* step 2: update centroids and calculate the difference */
        for (int j = 0; j < K; j++)
        {
            float *oldCentroid = pCentroids[j];
            float *newCentroid = update_centroid(pListOfVec, assignmentsToClusters, j, N, d);
            deltaCentroids = calc_euclidian_distance(newCentroid, oldCentroid, d);
            if (deltaCentroids < EPSILON)
            {
                isConverged = true;
            }
            pCentroids[j] = newCentroid;
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

float calc_euclidian_distance(float *vec1, float *vec2, int d)
{
    float sum = 0.0;
    for (int i = 0; i < d; i++)
    {
        sum += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
    }
    return sqrtf(sum);
}

bool parse_arguments(int argc, char *argv[], int *K, int *iter, char **filename)
{
    if (argc > 4 || argc < 3)
    {
        fprintf(stderr, "wrong number of arguments!\n");
        return false;
    }

    if (!parse_strict_integer(argv[1], &K))
    {
        fprintf(stderr, "Invalid K value!\n");
        return false;
    }
    K = atoi(argv[1]);
    if (argc == 4)
    {
        if (!parse_strict_integer(argv[2], &iter))
        {
            fprintf(stderr, "Invalid iteration value!\n");
            return false;
        }
        iter = atoi(argv[2]);
        filename = argv[3];
    }
    else
    {
        iter = 400;
        filename = argv[2];
    }

    if (iter > 799 || iter < 2)
    {
        printf("Incorrect maximum iteration!\n");
        return false;
    }
    return true;
}

float **read_vectors_from_file(FILE *file, int *N, int *d)
{
    size_t len = 0;
    char *pLine = NULL;
    int capacity = 5, capacityVecList = 5;
    float **pListOfVec = malloc(capacityVecList * sizeof(float *));

    while ((getline(&pLine, &len, file)) != -1)
    {
        int current_d = 0;
        float *pVector = malloc(capacity * sizeof(float));
        char *token = strtok(pLine, ",");
        /* initialize the vector */
        while (token != NULL)
        {
            if (current_d == capacity)
            {
                capacity *= 2;
                pVector = realloc(pVector, capacity * sizeof(float));
            }
            pVector[current_d] = atof(token);
            current_d++;
            token = strtok(NULL, ",");
        }

        if (*d == 0)
        {
            *d = current_d;
        }
        /*add the vector to the list*/
        if (*N == capacityVecList)
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
        pListOfVec[*N] = realloc(pVector, *d * sizeof(float));
        (*N)++;
    }
    free(pLine);
    return pListOfVec;
}

float **init_centroids(float **pListOfVec, int K, int d)
{
    float **pCentroids = malloc(K * sizeof(float *));
    for (int i = 0; i < K; i++)
    {
        pCentroids[i] = malloc(d * sizeof(float));
        for (int j = 0; j < d; j++)
        {
            pCentroids[i][j] = pListOfVec[i][j];
        }
    }
    return pCentroids;
}

void assign_vectors_to_clusters(float **pListOfVec, float **pCentroids, int *assignmentsToClusters, int N, int K, int d)
{
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
}

float *update_centroid(float **pListOfVec, int *assignmentsToClusters, int clusterIndex, int N, int d)
{
    int total = 0;
    float *newCentroid = calloc(d, sizeof(float));
    for (int i = 0; i < N; i++)
    {
        if (assignmentsToClusters[i] == clusterIndex)
        {
            for (int j = 0; j < d; j++)
            {
                newCentroid[j] += pListOfVec[i][j];
            }
            total++;
        }
    }
    if (total == 0)
    {
        return newCentroid;
    }
    for (int i = 0; i < d; i++)
    {
        newCentroid[i] = newCentroid[i] / total;
    }
    return newCentroid;
}
