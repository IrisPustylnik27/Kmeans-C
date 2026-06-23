# K-Means Clustering in C

This project implements the K-Means clustering algorithm in C.

The program receives a set of numerical vectors from standard input, initializes the first `K` vectors as the initial centroids, and iteratively updates the clusters until convergence or until the maximum number of iterations is reached.

The project was developed as a team project by 2 students.

## Overview

K-Means is an unsupervised machine learning algorithm used to partition data points into `K` clusters.
Each vector is assigned to the closest centroid according to Euclidean distance, and each centroid is updated to be the average of the vectors assigned to its cluster.

The implementation focuses on low-level programming in C, including dynamic memory allocation, input parsing, pointer-based data structures, and manual resource cleanup.

## Features

* K-Means clustering implementation in C
* Reads vectors from standard input
* Supports configurable number of clusters `K`
* Supports configurable maximum number of iterations
* Uses Euclidean distance for cluster assignment
* Initializes centroids from the first `K` input vectors
* Stops when centroids converge using `epsilon = 0.001`
* Handles empty clusters by keeping the previous centroid
* Prints final centroids with 4 digits after the decimal point
* Includes centralized memory cleanup

## Technologies

* C
* GCC
* Standard input / output
* Dynamic memory allocation
* Pointer-based programming

## Main Concepts Practiced

* K-Means clustering algorithm
* Euclidean distance calculation
* Iterative centroid updates
* Dynamic arrays using `malloc`, `calloc`, and `realloc`
* Manual memory management
* Input parsing and validation
* Working with multidimensional vectors
* Error handling in C
* Modular function-based program design

## Usage

Compile the program using GCC:

```bash
gcc -Wall -Wextra -Werror -ansi -pedantic-errors kmeans.c -lm -o kmeans
```

Run the program with:

```bash
./kmeans K [iter] < input.txt
```

Where:

* `K` is the number of clusters
* `iter` is an optional maximum number of iterations
* If `iter` is not provided, the default value is `400`
* Input vectors are provided through standard input

## Input Format

The input should contain numerical vectors, one vector per line.
Coordinates should be separated by commas.

Example:

```text
1.0,2.0,3.0
1.5,2.2,3.1
8.0,9.0,10.0
8.5,9.2,10.3
```

## Output Format

The program prints the final centroids, one centroid per line.
Each coordinate is printed with 4 digits after the decimal point.

Example:

```text
1.2500,2.1000,3.0500
8.2500,9.1000,10.1500
```

## Algorithm Flow

1. Parse and validate command-line arguments
2. Read vectors from standard input
3. Initialize the first `K` vectors as centroids
4. Assign each vector to the closest centroid
5. Update each centroid based on the average of its assigned vectors
6. Check convergence using Euclidean distance and `epsilon = 0.001`
7. Repeat until convergence or until the iteration limit is reached
8. Print the final centroids
9. Free all allocated memory

## Main Functions

* `parse_arguments` — validates command-line arguments
* `read_vectors_from_file` — reads vectors from standard input
* `init_centroids` — initializes the first `K` vectors as centroids
* `calc_euclidian_distance` — calculates Euclidean distance between two vectors
* `assign_vectors_to_clusters` — assigns each vector to the closest centroid
* `update_centroid` — calculates a new centroid for a cluster
* `print_centroids` — prints the final centroids
* `cleanup` — frees allocated memory

## Error Handling

The program validates the number of clusters and the maximum number of iterations.

Possible error messages include:

```text
Incorrect number of clusters!
Incorrect maximum iteration!
An Error Has Occured
```

## Team

This project was developed by:

* Ksenia Iaremenko
* Iris Pustylnik

## Notes

This project was created for academic purposes as part of a programming / software project course.
