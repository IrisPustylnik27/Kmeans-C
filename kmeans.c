#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

void *safe_realloc(void *pointer, size_t new_size);

/*cheacking integers validation*/
bool parse_strict_integer(const char *str, int *out_value);

/*parsing the arguments and validating them*/
bool parse_arguments(int argc, char *argv[], int *K, int *iter);

/*calculating the euclidian distance between two vectors*/
double calc_euclidian_distance(double *vec1, double *vec2, int d);

double **read_vectors_from_file(FILE *file, int *N, int *d);

double **init_centroids(double **pListOfVec, int K, int d);

void assign_vectors_to_clusters(double **pListOfVec, double **pCentroids, int *assignmentsToClusters, int N, int K, int d);

double *update_centroid(double **pListOfVec, int *assignmentsToClusters, int clusterIndex, int N, int d, double *oldCentroid);

void cleanup(double **pListOfVec, double **pCentroids, double *pCentroid, int *assignmentsToClusters, int N, int K, char *pLine, double *pVector);

void print_centroids(double **pCentroids, int K, int d);

int main(int argc, char *argv[])
{
    int N = 0, K = 0, d = 0,counter = 0,j=0;
    double **pCentroids = NULL, **pListOfVec = NULL;
    int iter = 2;
    const double EPSILON = 0.001;
    double deltaCentroids = 0.0;
    int *assignmentsToClusters;
    bool isConverged = false;

    if (!parse_arguments(argc, argv, &K, &iter))
    {
        return 1;
    }

    /*read the vectors*/
    pListOfVec = read_vectors_from_file(stdin, &N, &d);

    if (N <= K) || (K < 2)
    {
        printf("Incorrect number of clusters!\n");
        return 1;
    }


    /*fun begins*/
    assignmentsToClusters = malloc(N * sizeof(int));

    /*initialize the centroids*/
    pCentroids = init_centroids(pListOfVec, K, d);

    while (counter < iter && !isConverged)
    {
        isConverged = true;
        /* step 1: assign vectors to clusters */
        assign_vectors_to_clusters(pListOfVec, pCentroids, assignmentsToClusters, N, K, d);

        /* step 2: update centroids and calculate the difference */
        for (j = 0; j < K; j++)
        {
            double *oldCentroid = pCentroids[j];
            double *newCentroid = update_centroid(pListOfVec, assignmentsToClusters, j, N, d, oldCentroid);
            deltaCentroids = calc_euclidian_distance(newCentroid, oldCentroid, d);
            if (deltaCentroids >= EPSILON)
            {
                isConverged = false;
            }
            pCentroids[j] = newCentroid;
            cleanup(NULL, NULL, oldCentroid, NULL, 0, 0, NULL, NULL);
        }
        counter++;
    }
    print_centroids(pCentroids, K, d);
    cleanup(pListOfVec, pCentroids, NULL, assignmentsToClusters, N, K, NULL, NULL);
    
    return 0;
}

void *safe_realloc(void *pointer, size_t new_size)
{
    void *new_pointer = realloc(pointer, new_size);
    if (new_pointer == NULL)
    {
        fprintf(stderr, "An Error Has Occured\n");
        free(pointer);
        exit(1);
        return NULL;
    }
    return new_pointer;
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

double calc_euclidian_distance(double *vec1, double *vec2, int d)
{
    int i;
    double sum = 0.0;
    for (i = 0; i < d; i++)
    {
        sum += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
    }
    return sqrt(sum);
}

bool parse_arguments(int argc, char *argv[], int *K, int *iter)
{
    if (argc > 3 || argc < 2)
    {
        fprintf(stderr, "An Error Has Occured\n");
        return false;
    }

    if (!parse_strict_integer(argv[1], K))
    {
        fprintf(stderr, "Incorrect number of clusters!\n");
        return false;
    }

    if (argc == 3)
    {
        if (!parse_strict_integer(argv[2], iter))
        {
            fprintf(stderr, "Incorrect maximum iteration!\n");
            return false;
        }
    }
    else
    {
        *iter = 400;
    }

    if (*iter > 799 || *iter < 2)
    {
        printf("Incorrect maximum iteration!\n");
        return false;
    }
    return true;
}

double **read_vectors_from_file(FILE *file, int *N, int *d)
{
    size_t len = 0;
    char *pLine = NULL;
    int capacity = 5, capacityVecList = 5;
    double **pListOfVec = malloc(capacityVecList * sizeof(double *));

    while ((getline(&pLine, &len, file)) != -1)
    {
        int current_d = 0;
        double *pVector = malloc(capacity * sizeof(double));
        char *token = strtok(pLine, ",");
        double **temp;
        /* initialize the vector */
        while (token != NULL)
        {
            if (current_d == capacity)
            {
                capacity *= 2;
                pVector = safe_realloc(pVector, capacity * sizeof(double));
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
            temp = safe_realloc(pListOfVec, capacityVecList * sizeof(double *));
            pListOfVec = temp;
        }
        pListOfVec[*N] = safe_realloc(pVector, *d * sizeof(double));
        (*N)++;
    }
    cleanup(NULL, NULL, NULL, NULL, 0, 0, pLine, NULL);
    return pListOfVec;
}

double **init_centroids(double **pListOfVec, int K, int d)
{
    int i,j;
    double **pCentroids = malloc(K * sizeof(double *));
    for (i = 0; i < K; i++)
    {
        pCentroids[i] = malloc(d * sizeof(double));
        for (j = 0; j < d; j++)
        {
            pCentroids[i][j] = pListOfVec[i][j];
        }
    }
    return pCentroids;
}

void assign_vectors_to_clusters(double **pListOfVec, double **pCentroids, int *assignmentsToClusters, int N, int K, int d)
{
    int i,j;
    for (i = 0; i < N; i++)
    {
        double minDistance = calc_euclidian_distance(pListOfVec[i], pCentroids[0], d);
        int closestCentroid = 0;
        for (j = 1; j < K; j++)
        {
            double distance = calc_euclidian_distance(pListOfVec[i], pCentroids[j], d);
            if (distance < minDistance)
            {
                minDistance = distance;
                closestCentroid = j;
            }
        }
        assignmentsToClusters[i] = closestCentroid;
    }
}

double *update_centroid(double **pListOfVec, int *assignmentsToClusters, int clusterIndex, int N, int d, double *oldCentroid)
{
    int i,j,total = 0;
    double *newCentroid;
    newCentroid = calloc(d, sizeof(double));
    for (i = 0; i < N; i++)
    {
        if (assignmentsToClusters[i] == clusterIndex)
        {
            for (j = 0; j < d; j++)
            {
                newCentroid[j] += pListOfVec[i][j];
            }
            total++;
        }
    } 
    if (total == 0)
    {
        for (i = 0; i < d; i++)
        {
            newCentroid[i] = oldCentroid[i];
        }
        return newCentroid;
    }
    for (i = 0; i < d; i++)
    {
        newCentroid[i] = newCentroid[i] / total;
    }
    return newCentroid;
}

void cleanup(double **pListOfVec, double **pCentroids, double *pCentroid, int *assignmentsToClusters, int N, int K, char *pLine, double *pVector)
{
    int i;
    if (pLine != NULL){
        free(pLine);
    }
    if (pVector != NULL)
    {
        free(pVector);
    }
    if (assignmentsToClusters != NULL)
    {
        free(assignmentsToClusters);
    }
    if (pListOfVec != NULL)
    {
        for (i = 0; i < N; i++)
        {
            free(pListOfVec[i]);
        }
        free(pListOfVec);
    }
    if (pCentroids != NULL)
    {
        for (i = 0; i < K; i++)
        {
            free(pCentroids[i]);
        }
        free(pCentroids);
    }
    if (pCentroid != NULL)
    {
        free(pCentroid);
    }
}

void print_centroids(double **pCentroids, int K, int d)
{
    int i, j;
    for (i = 0; i < K; i++)
    {
        for (j = 0; j < d; j++)
        {
            printf("%.4f", pCentroids[i][j]);
            if (j < d - 1)
            {
                printf(",");
            }
        }
        printf("\n");
    }
}
