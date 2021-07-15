#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define left(x) ((2*x)+1)
#define right(x) ((2*x)+2)
#define parent(x) ((x-1)/2)

//strutture grafi
typedef struct{
    int **data;
    int n;
}graph_t;

typedef int *line_t;

//strutture heap
typedef struct{
    int v;
    unsigned int dist;
}heap_node_t;

typedef struct{
    heap_node_t *data;
    int *pos;
    int heap_size;
}heap_t;

//gestione dell'input
void init_parameters(int *d, int *k);
int manage_operation(int n, int k);
//gestione dei grafi
graph_t create_graph(int n);
line_t insert_graph_line(int n);
void print_graph(graph_t graph);
unsigned int *Dijkstra_shortest_path(graph_t graph);
//gestione dello heap
void swap_nodes(heap_node_t *a, heap_node_t *b);
void swap_heap_pos(heap_t *heap, int a, int b);
void min_heap_insert(heap_t *heap, int v, unsigned int dist);
void min_heapify(heap_t* heap, int i);
heap_node_t heap_extract_min(heap_t *heap);
void heap_decrease_key(heap_t *heap, int v, unsigned int dist);

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
int manage_operation(int n, int k){
    char *line = NULL;
    size_t line_length = 0;
    getline(&line, &line_length, stdin);

    if(strcmp(line, "AggiungiGrafo\n") == 0) {
        graph_t graph = create_graph(n);
        unsigned int *dist = Dijkstra_shortest_path(graph);
        unsigned int sum = 0;

        for(int i = 0; i < n; i++)
            sum += dist[i];

        //TODO free dist
        printf("\ngraph:\n");
        print_graph(graph);
        printf("somma dei cammini minimi: %u\n", sum);

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
unsigned int *Dijkstra_shortest_path(graph_t graph){
    int n = graph.n;
    unsigned int *dist = malloc(n * sizeof(unsigned int));

    //inizializza lo heap
    heap_t *heap = malloc(sizeof(heap_t));
    heap->data = malloc(n * sizeof(heap_node_t));
    heap->pos = malloc(n * sizeof(int));
    heap->heap_size = 0;

    //inserisci la sorgente nello heap
    min_heap_insert(heap, 0, 0);
    dist[0] = 0;

    //inserisci tutti i vertici nello heap
    for(int i = 1; i < n; i++) {
        min_heap_insert(heap, i, UINT_MAX);
        dist[i] = UINT_MAX;
    }

    //fintanto che lo heap non è vuoto
    while(heap->heap_size >= 1){
        //prendi il vertice con distanza minima dalla sorgente
        heap_node_t node = heap_extract_min(heap);
        int u = node.v;
        //per tutti i nodi v adiacenti ad u
        for(int v = 0; v < n; v++){
            //condizione di adiacenza
            if(graph.data[u][v] != 0) {
                //condizione cammino minimo
                if (dist[v] > dist[u] + graph.data[u][v] && u != UINT_MAX) {
                    //aggiorna la distanza da v con la distanza minima
                    dist[v] = dist[u] + graph.data[u][v];
                    heap_decrease_key(heap, v, dist[v]);
                }
            }
        }
    }

    //sistema i vertici non raggiungibili dalla sorgente
    for(int i = 0; i < n; i++){
        if(dist[i] == UINT_MAX)
            dist[i] = 0;
    }

    free(heap);
    return dist;
}

void swap_nodes(heap_node_t *a, heap_node_t *b){
    heap_node_t temp = *a;
    *a = *b;
    *b = temp;
}
void swap_heap_pos(heap_t *heap, int a, int b){
    heap->pos[heap->data[a].v] = b;
    heap->pos[heap->data[b].v] = a;
}
void min_heap_insert(heap_t *heap, int v, unsigned int dist){
    //inizializza i valori del nodo
    heap_node_t node;
    node.dist = dist;
    node.v = v;

    //inserisci il nodo nello heap
    heap->data[heap->heap_size] = node;
    heap->pos[v] = v;

    heap_decrease_key(heap, heap->heap_size, dist);
    heap->heap_size++;
}
void min_heapify(heap_t* heap, int i){
    int l = left(i);
    int r = right(i);
    int min;

    if (l < heap->heap_size && heap->data[l].dist < heap->data[i].dist)
        min = l;
    else
        min = i;

    if (r < heap->heap_size && heap->data[r].dist < heap->data[min].dist)
        min = r;

    if(min != i){
        swap_heap_pos(heap, i, min);
        swap_nodes(&heap->data[i], &heap->data[min]);
        min_heapify(heap, min);
    }
}
heap_node_t heap_extract_min(heap_t *heap){
    if(heap->heap_size < 1) {
        printf("error: underflow\n");
        return *(volatile heap_node_t*)NULL;
    }

    //estrai minimo
    heap_node_t min = heap->data[0];

    //metti l'ultimo elemento alla radice e sistema il min heap
    heap->heap_size--;
    heap_node_t last = heap->data[heap->heap_size];
    heap->data[0] = last;

    //swap node positions
    heap->pos[min.v] = heap->heap_size;
    heap->pos[last.v] = 0;

    min_heapify(heap, 0);

    return min;
}
void heap_decrease_key(heap_t *heap, int v, unsigned int dist){
    int i = heap->pos[v];
    if(dist > heap->data[i].dist) {
        printf("error: new key greater than older\n");
        return;
    }

    heap->data[i].dist = dist;
    while(i > 0 && heap->data[parent(i)].dist > heap->data[i].dist){
        swap_heap_pos(heap, i, parent(i));
        swap_nodes(&heap->data[i], &heap->data[parent(i)]);
        i = parent(i);
    }
}