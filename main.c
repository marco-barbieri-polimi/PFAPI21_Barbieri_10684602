#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int** graph_t;

void init_parameters(int *d, int *k);
graph_t create_graph(int n);
graph_t set_matrix_values(graph_t graph, int n);
void insert_matrix_line(int **line_p, int n);
void print_matrix(int** matrix, int n);
int manage_operation(int n, int k);

int main(){
    int d, k;
    init_parameters(&d, &k);
    while(manage_operation(d, k) == 0);
    return 0;
}

void init_parameters(int *d, int *k){
    char* line = NULL;
    size_t line_length = 0;
    getline(&line, &line_length, stdin);
    *d = strtol(line, &line, 10);
    *k = strtol(line, &line, 10);
    free(line);
}

graph_t create_graph(int n){
    graph_t graph_p = malloc(n*sizeof(int*));
    for(int i = 0; i < n; i++){
        int* line_p = malloc(n*sizeof(int));
        *(graph_p + i) = line_p;
    }

    return set_matrix_values(graph_p, n);
}

graph_t set_matrix_values(graph_t graph_p, int n){
    for(int i = 0; i < n; i++)
        insert_matrix_line((graph_p + i), n);

    return graph_p;
}

void insert_matrix_line(int **line_p, int n){
    char* line = NULL;
    size_t length = 0;
    getline(&line, &length, stdin);

    int token_count = 0;
    //le linee sono stringhe di interi suddivisi dal carattere "," e finiscono con "\n" che tratto come separatore
    char* delimiters = ",\n";
    //legge la prima stringa di interi (token) della linea
    char* token = strtok(line, delimiters);
    while(token != NULL){
        //e fa un parse da string a int
        *(*line_p + token_count) = (int)strtol(token, NULL, 10);
        token_count++;
        //TODO controllo errori
        //mi aspetto che element contenga una stringa di numeri, se la stringa è vuota allora c'è un errore.
        //se element non contiene interi c'è un errore.
        //mi aspetto di ricevere n numeri interi: se count > n, qualcosa è andato storto.

        //legge il token successivo
        token = strtok(NULL, delimiters);
    }
    free(line);
}

void print_matrix(int** matrix, int n){
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++)
            printf("%d ", *(*(matrix + i) + j));
        printf("\n");
    }
}

int manage_operation(int n, int k){
    char* line = NULL;
    size_t line_length = 0;
    getline(&line, &line_length, stdin);

    if(strcmp(line, "AggiungiGrafo\n") == 0) {
        graph_t graph = create_graph(n);
        print_matrix(graph, n);
        free(line);
        return 0;
    }
    if(strcmp(line, "TopK\n") == 0) {
        //call TopK function
        free(line);
        return 1;
    }
    //TODO errore
    //il comando letto è inesistente
    free(line);
    return -1;
}