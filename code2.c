#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define INPUT_DIR "inputfiles/"
#define EPSILON 1e-6  
#define MAX_LINE_LENGTH 16384  

void read_matrix(const char *filename, int n, double **matrix);
void read_vector(const char *filename, int n, double *vector);
int is_eigenvector(double **matrix, double *vector, int n, double *eigenvalue);
//void write_result(const char *filename, const char *message, double eigenvalue);

int main() {
    int n, vecnum;
    char matrix_filename[100], vector_filename[100];
    char input_filepath[100];

    sprintf(input_filepath, "%s%s", INPUT_DIR, "input.in");
    FILE *input_file = fopen(input_filepath, "r");
    if (!input_file) {
        perror("Error opening input.in");
        return 1;
    }
    fscanf(input_file, "%d", &n);
    fclose(input_file);

    double **matrix = (double **)malloc(n * sizeof(double *));
    if (!matrix) {
        fprintf(stderr, "Memory allocation failed for matrix rows\n");
        return 1;
    }
    
    for (int i = 0; i < n; i++) {
        matrix[i] = (double *)malloc(n * sizeof(double));
        if (!matrix[i]) {
            fprintf(stderr, "Memory allocation failed for matrix column %d\n", i);
            
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return 1;
        }
    }

    sprintf(matrix_filename, "%smat_%06d.in", INPUT_DIR, n);
    read_matrix(matrix_filename, n, matrix);

    for (vecnum = 1;; vecnum++) {
        sprintf(vector_filename, "%svec_%06d_%06d.in", INPUT_DIR, n, vecnum);
        FILE *vector_file = fopen(vector_filename, "r");
        if (!vector_file) {
            break;
        }
        fclose(vector_file);

        double *vector = (double *)malloc(n * sizeof(double));
        if (!vector) {
            fprintf(stderr, "Memory allocation failed for vector\n");
            continue;
        }

        read_vector(vector_filename, n, vector);

        double eigenvalue = 0;
        if (is_eigenvector(matrix, vector, n, &eigenvalue)) {
            printf("vec_%06d_%06d.in : Yes : %.10f\n", n, vecnum, eigenvalue);
            //write_result(vector_filename, "Yes", eigenvalue);
        } else {
            printf("vec_%06d_%06d.in : Not an eigenvector\n", n, vecnum);
            //write_result(vector_filename, "Not an eigenvector", 0);
        }

        free(vector);
    }

    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}

void read_matrix(const char *filename, int n, double **matrix) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening matrix file");
        exit(1);
    }

    char *line = (char *)malloc(MAX_LINE_LENGTH);
    if (!line) {
        perror("Memory allocation failed for line buffer");
        fclose(file);
        exit(1);
    }

    char *token;
    int i = 0, j;
    
    while (fgets(line, MAX_LINE_LENGTH, file) && i < n) {
        line[strcspn(line, "\n")] = 0;
        token = strtok(line, ",");
        j = 0;
        
        while (token != NULL && j < n) {
            matrix[i][j] = atof(token);
            token = strtok(NULL, ",");
            j++;
        }
        
        if (j != n) {
            fprintf(stderr, "Error: Line %d contains %d elements, expected %d\n", i + 1, j, n);
            free(line);
            fclose(file);
            exit(1);
        }
        i++;
    }

    if (i != n) {
        fprintf(stderr, "Error: File contains %d lines, expected %d\n", i, n);
        free(line);
        fclose(file);
        exit(1);
    }

    free(line);
    fclose(file);
}

void read_vector(const char *filename, int n, double *vector) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening vector file");
        exit(1);
    }

    char *line = (char *)malloc(MAX_LINE_LENGTH);
    if (!line) {
        perror("Memory allocation failed for line buffer");
        fclose(file);
        exit(1);
    }

    if (fgets(line, MAX_LINE_LENGTH, file)) {
        line[strcspn(line, "\n")] = '\0';
        line[strcspn(line, "\r")] = '\0';

        char *token = strtok(line, ",");
        int i = 0;

        while (token != NULL && i < n) {
            while (isspace((unsigned char)*token)) token++;
            char *end = token + strlen(token) - 1;
            while (end > token && isspace((unsigned char)*end)) *end-- = '\0';

            vector[i] = atof(token);
            token = strtok(NULL, ",");
            i++;
        }

        if (i != n) {
            fprintf(stderr, "Error: Expected %d elements, but found %d in file %s\n", n, i, filename);
            free(line);
            fclose(file);
            exit(1);
        }
    } else {
        fprintf(stderr, "Error: Empty or malformed vector file %s\n", filename);
        free(line);
        fclose(file);
        exit(1);
    }

    free(line);
    fclose(file);
}

int is_eigenvector(double **matrix, double *vector, int n, double *eigenvalue) {
    double *result = (double *)malloc(n * sizeof(double));
    if (!result) {
        fprintf(stderr, "Memory allocation failed for result vector\n");
        return 0;
    }

    double max_component = 0;
    for (int i = 0; i < n; i++) {
        result[i] = 0;
        for (int j = 0; j < n; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
        max_component = fmax(max_component, fabs(result[i]));
    }

    if (max_component > 1e10) {
        for (int i = 0; i < n; i++) {
            result[i] /= max_component;
            vector[i] /= max_component;
        }
    }

    int best_index = 0;
    double best_ratio = 0;
    int eigenvalue_found = 0;

    for (int i = 0; i < n; i++) {
        if (fabs(vector[i]) > EPSILON) {
            double ratio = result[i] / vector[i];
            if (!eigenvalue_found || fabs(vector[i]) > fabs(vector[best_index])) {
                best_ratio = ratio;
                best_index = i;
                eigenvalue_found = 1;
            }
        }
    }

    if (!eigenvalue_found) {
        free(result);
        return 0;
    }

    *eigenvalue = best_ratio;

    
    for (int i = 0; i < n; i++) {
        if (fabs(vector[i]) > EPSILON) {
            if (fabs(result[i] - (*eigenvalue) * vector[i]) > EPSILON * fmax(fabs(result[i]), fabs(vector[i]))) {
                free(result);
                return 0;
            }
        } else {
            if (fabs(result[i]) > EPSILON) {
                free(result);
                return 0;
            }
        }
    }

    free(result);
    return 1;
}

// void write_result(const char *filename, const char *message, double eigenvalue) {
//     FILE *file = fopen(filename, "a");
//     if (!file) {
//         perror("Error opening vector file for writing");
//         exit(1);
//     }

//     if (strcmp(message, "Yes") == 0) {
//         fprintf(file, "\n%s : %.10f", message, eigenvalue);
//     } else {
//         fprintf(file, "\n%s", message);
//     }
//     fclose(file);
// }