#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define left(x) ((2*x)+1)
#define right(x) ((2*x)+2)
#define parent(x) ((x-1)/2)

//strutture grafi
typedef struct{
    unsigned int **data;
    int n;
}graph_t;

typedef unsigned int *line_t;

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

//struttura lista (per la classifica dei grafi)
typedef struct node{
    int graph_id;
    unsigned int metric;
    struct node *next;
}list_node_t;

typedef struct{
    int graphs_number;
    list_node_t *head;
}list_t;

//gestione dell'input
int init_parameters(int *d, int *k);
int manage_operation(int n, int k, list_t *list);
//gestione dei grafi
graph_t *create_graph(int n);
void free_graph(graph_t *graph);
line_t insert_graph_line(int n);
unsigned int Dijkstra_shortest_path(graph_t graph);
//gestione dello heap
void swap_nodes(heap_node_t *a, heap_node_t *b);
void swap_heap_pos(heap_t *heap, int a, int b);
void min_heap_insert(heap_t *heap, int v, unsigned int dist);
void min_heapify(heap_t* heap, int i);
heap_node_t heap_extract_min(heap_t *heap);
void heap_decrease_key(heap_t *heap, int v, unsigned int dist);
//gestione della lista
list_t *create_list();
void free_list(list_t *list);
void list_inorder_insert(list_t *list, unsigned int metric);

int main(){
    int d, k;
    if(init_parameters(&d, &k) == -1)
        exit(EXIT_FAILURE);

    list_t *list = create_list();
    while(manage_operation(d, k, list) == 0){}
    free_list(list);

    exit(EXIT_SUCCESS);
}

int init_parameters(int *d, int *k){
    //22 perché un int è lungo al massimo 10 caratteri (x2) + un carattere spazio + il carattere \0
    char *line = calloc(22, sizeof(char));
    if(fgets(line, 22, stdin) == NULL && ferror(stdin) != 0){
    	fprintf(stderr, "errore nella lettura dei parametri");
    	perror("");
    	return -1;
    }

    char *remain;
    *d = strtol(line, &remain, 10);
    *k = strtol(remain, NULL, 10);
    free(line);
    return 0;
}
int manage_operation(int n, int k, list_t* list){
    char *line = calloc(20, sizeof(char));
    if(fgets(line, 20, stdin) == NULL && ferror(stdin) != 0){
        free(line);
        fprintf(stderr, "errore nella lettura di un'istruzione\n");
        perror("");
        return -1;
    }

    if(strcmp(line, "AggiungiGrafo\n") == 0) {
        free(line);
        graph_t *graph = create_graph(n);
        list_inorder_insert(list, Dijkstra_shortest_path(*graph));
        free_graph(graph);
        return 0;
    }
    if(strcmp(line, "TopK\n") == 0) {
    	free(line);
        list_node_t *node = list->head;

        if(node == NULL){
            printf("\n");
            return 0;
        }

        for(int i = 0; i < k-1; i++){
            if(node->next == NULL) {
                break;
            }
            printf("%d ", node->graph_id);
            node = node->next;
        }

        printf("%d\n", node->graph_id);
        return 0;
    }
    free(line);
    return -1;
}

graph_t *create_graph(int n){
    graph_t *graph = (graph_t *)malloc(sizeof(graph_t));
    graph->n = n;
    graph->data = (unsigned int **)malloc(n * sizeof(unsigned int *));
    for(int i = 0; i < n; i++)
        *(graph->data + i) = insert_graph_line(n);
    return graph;
}
void free_graph(graph_t *graph){
    int n = graph->n;

    for(int i = 0; i < n; i++)
        free(graph->data[i]);
    free(graph->data);
    free(graph);
}
line_t insert_graph_line(int n){
    line_t line = (unsigned int*)malloc(n * sizeof(unsigned int));

    for(int i = 0; i < n; i++){
        unsigned int number = 0;
        int digit;

        digit = getchar();
        while(digit != ',' && digit != '\n'){
            number = 10*number + (digit - '0');
            digit = getchar();
        }

        *(line + i) = number;
    }

    return line;
}
unsigned int Dijkstra_shortest_path(graph_t graph){
    int n = graph.n;
    unsigned int *dist = (unsigned int *)malloc(n * sizeof(unsigned int));

    //inizializza lo heap
    heap_t heap;
    heap.data = (heap_node_t *)malloc(n * sizeof(heap_node_t));
    heap.pos = (int *)malloc(n * sizeof(int));
    heap.heap_size = 0;

    //inserisci la sorgente nello heap
    min_heap_insert(&heap, 0, 0);
    dist[0] = 0;

    //inserisci tutti i vertici nello heap
    for(int i = 1; i < n; i++) {
        min_heap_insert(&heap, i, UINT_MAX);
        dist[i] = UINT_MAX;
    }

    //fintanto che lo heap non è vuoto
    while(heap.heap_size > 0){
        //prendi il vertice con distanza minima dalla sorgente
        heap_node_t node = heap_extract_min(&heap);
        int u = node.v;
        //per tutti i nodi v adiacenti ad u
        for(int v = 0; v < n; v++){
            //condizione di adiacenza && condizione cammino minimo
            if(graph.data[u][v] != 0 && dist[v] > dist[u] + graph.data[u][v] && dist[u] != UINT_MAX) {
                //aggiorna la distanza da v con la distanza minima
                dist[v] = dist[u] + graph.data[u][v];
                heap_decrease_key(&heap, v, dist[v]);
            }
        }
    }

    //sistema i vertici non raggiungibili dalla sorgente
    unsigned int sum = 0;
    for(int i = 0; i < n; i++) {
        if (dist[i] == UINT_MAX)
            continue;
        sum += dist[i];
    }
    free(heap.data);
    free(heap.pos);
    free(dist);

    return sum;
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
    int min = i;

    if (l < heap->heap_size && heap->data[l].dist < heap->data[min].dist)
        min = l;

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
        //TODO sistema questo
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

list_t *create_list(){
    list_t *new_list = (list_t *)malloc(sizeof(list_t));
    new_list->head = NULL;
    new_list->graphs_number = 0;
    return new_list;
}

void free_list(list_t *list){
    list_node_t *curr = list->head;
    list_node_t *next;

    while(curr != NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }

    free(list);
}

void list_inorder_insert(list_t *list, unsigned int metric){
    list_node_t *head = list->head;
    list_node_t *new_node = (list_node_t*)malloc(sizeof(list_node_t));
    new_node->metric = metric;
    new_node->graph_id = list->graphs_number;

    if(head == NULL || head->metric >= new_node->metric){
        new_node->next = head;
        list->head = new_node;
        list->graphs_number++;
    }else{
        list_node_t *cur_node = head;

        while(cur_node->next != NULL && (cur_node->next)->metric < new_node->metric)
            cur_node = cur_node->next;

        new_node->next = cur_node->next;
        cur_node->next = new_node;
        list->graphs_number++;
    }
}