#include <stdio.h>
#include <stdlib.h>

void print_to_file(int n, double **A, int format_flag) {
    char filename[20];
    int i,j;
    snprintf(filename, sizeof(filename), "array_%06d_%s.out", n, format_flag == 0 ? "asc" : "bin");

    FILE *file = fopen(filename, format_flag == 0 ? "w" : "wb");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        exit(1);
    }

    if (format_flag == 0) { 
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                fprintf(file, "%.15f ", A[i][j]);
            }
            fprintf(file, "\n");
        }
    } else { 
        for (i = 0; i < n; i++) {
            fwrite(A[i], sizeof(double), n, file);
        }
    }

    fclose(file);
}

int main() {
    

    int n;
    int i,j;
    
    n = 4000;

    
    double **A = (double **)malloc(n * sizeof(double *));
    if (!A) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 1;
    }
    for (i = 0; i < n; i++) {
        A[i] = (double *)malloc(n * sizeof(double));
        if (!A[i]) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            return 1;
        }
    }

    
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            A[i][j] = (double)(i + j);
        }
    }

   
    print_to_file(n, A, 0); 
    print_to_file(n, A, 1); 

   
    for (i = 0; i < n; i++) {
        free(A[i]);
    }
    free(A);

    return 0;
}
