#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int **data;
    int n;
}graph_t;
typedef int *line_t;

void init_parameters(int *d, int *k);
graph_t create_graph(int n);
line_t insert_graph_line(int n);
void print_graph(graph_t graph);
int manage_operation(int n, int k);

int main(){
    int d, k;
    init_parameters(&d, &k);
    while(manage_operation(d, k) == 0);
    return 0;
}

void init_parameters(int *d, int *k){
    char *line = NULL;
    size_t line_length = 0;
    getline(&line, &line_length, stdin);
    *d = (int)strtol(line, &line, 10);
    *k = (int)strtol(line, &line, 10);
    free(line);
}

graph_t create_graph(int n){
    graph_t graph;
    graph.n = n;
    graph.data = malloc(n * sizeof(int*));
    for(int i = 0; i < n; i++)
        *(graph.data + i) = insert_graph_line(n);
    return graph;
}

line_t insert_graph_line(int n){
    char *input_line = NULL;
    size_t length = 0;
    getline(&input_line, &length, stdin);

    line_t line_p = malloc(n * sizeof(int));

    int token_count = 0;
    //le linee sono stringhe di interi suddivisi dal carattere "," e finiscono con "\n" che tratto come separatore
    char *delimiters = ",\n";
    //legge la prima stringa di interi (token) della linea
    char *token = strtok(input_line, delimiters);
    while(token != NULL){
        //e fa un parse da string a int
        *(line_p + token_count) = (int)strtol(token, NULL, 10);
        token_count++;
        //TODO controllo errori
        //mi aspetto che element contenga una stringa di numeri, se la stringa è vuota allora c'è un errore.
        //se element non contiene interi c'è un errore.
        //mi aspetto di ricevere n numeri interi: se count > n, qualcosa è andato storto.

        //legge il token successivo
        token = strtok(NULL, delimiters);
    }
    free(input_line);
    return line_p;
}

void print_graph(graph_t graph){
    int n = graph.n;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++)
            printf("%d ", *(*(graph.data + i) + j));
        printf("\n");
    }
}

int manage_operation(int n, int k){
    char *line = NULL;
    size_t line_length = 0;
    getline(&line, &line_length, stdin);

    if(strcmp(line, "AggiungiGrafo\n") == 0) {
        graph_t graph = create_graph(n);
        print_graph(graph);
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