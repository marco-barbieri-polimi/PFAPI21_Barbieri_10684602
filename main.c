#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define left(x) ((2*x)+1)
#define right(x) ((2*x)+2)
#define parent(x) ((x-1)/2)

//strutture heap
typedef struct{
    int vertex_id;
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

int d, k;

//gestione dell'input
int manage_operation(list_t *list);
//gestione dei grafi
unsigned int Dijkstra_shortest_path(unsigned int graph[][d]);
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
    if(scanf("%d %d\n", &d, &k) == 0)
        return -1;

    list_t *list = create_list();
    while(manage_operation(list) == 0){}
    free_list(list);
    return 0;
}

int manage_operation(list_t* list){
    int operation = getchar_unlocked();
    if(operation == 'A'){
        for(int i = 0; i < 13; i++)
            getchar_unlocked();

        unsigned int graph[d][d];
        for(int i = 0; i < d; i++){
            for(int j = 0; j < d; j++){
                unsigned int number = 0;
                int digit;

                digit = getchar_unlocked();
                while(digit != ',' && digit != '\n'){
                    number = 10*number + (digit - '0');
                    digit = getchar_unlocked();
                }

                graph[i][j] = number;
            }
        }
        list_inorder_insert(list, Dijkstra_shortest_path(graph));
        return 0;
    }
    if(operation == 'T'){
        for(int i = 0; i < 4; i++)
            getchar_unlocked();

        list_node_t *node = list->head;

        if(node == NULL){
            printf("\n");
            return 0;
        }

        for(int i = 0; i < k-1; i++){
            if(node->next == NULL)
                break;

            printf("%d ", node->graph_id);
            node = node->next;
        }

        printf("%d\n", node->graph_id);
        return 0;
    }
    return -1;
}

unsigned int Dijkstra_shortest_path(unsigned int graph[][d]){
    unsigned int dist[d];

    //inizializza lo heap
    heap_t heap;
    heap.data = (heap_node_t *)malloc(d * sizeof(heap_node_t));
    heap.pos = (int *)malloc(d * sizeof(int));
    heap.heap_size = 0;

    //inserisci la sorgente nello heap
    min_heap_insert(&heap, 0, 0);
    dist[0] = 0;

    //inserisci tutti i vertici nello heap
    for(int i = 1; i < d; i++) {
        min_heap_insert(&heap, i, UINT_MAX);
        dist[i] = UINT_MAX;
    }

    //fintanto che lo heap non è vuoto
    while(heap.heap_size > 0){
        //prendi il vertice con distanza minima dalla sorgente
        heap_node_t node = heap_extract_min(&heap);
        int u = node.vertex_id;
        //per tutti i nodi v adiacenti a u
        for(int v = 0; v < d; v++){
            //condizione di adiacenza && condizione cammino minimo
            if(graph[u][v] != 0 && dist[v] > dist[u] + graph[u][v] && dist[u] != UINT_MAX) {
                //aggiorna la distanza da v con la distanza minima
                dist[v] = dist[u] + graph[u][v];
                heap_decrease_key(&heap, v, dist[v]);
            }
        }
    }

    //computa la somma dei cammini minimi
    unsigned int sum = 0;
    for(int i = 0; i < d; i++) {
        if (dist[i] == UINT_MAX)
            continue;
        sum += dist[i];
    }

    free(heap.data);
    free(heap.pos);
    return sum;
}

void swap_nodes(heap_node_t *a, heap_node_t *b){
    heap_node_t temp = *a;
    *a = *b;
    *b = temp;
}
void swap_heap_pos(heap_t *heap, int a, int b){
    heap->pos[heap->data[a].vertex_id] = b;
    heap->pos[heap->data[b].vertex_id] = a;
}
void min_heap_insert(heap_t *heap, int v, unsigned int dist){
    //inizializza i valori del nodo
    heap_node_t node;
    node.dist = dist;
    node.vertex_id = v;

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
    //estrai minimo
    heap_node_t min = heap->data[0];

    //metti l'ultimo elemento alla radice e sistema il min heap
    heap->heap_size--;
    heap_node_t last = heap->data[heap->heap_size];
    heap->data[0] = last;

    //swap node positions
    heap->pos[min.vertex_id] = heap->heap_size;
    heap->pos[last.vertex_id] = 0;

    min_heapify(heap, 0);

    return min;
}
void heap_decrease_key(heap_t *heap, int v, unsigned int dist){
    int i = heap->pos[v];

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

    if(head == NULL || head->metric > new_node->metric){
        new_node->next = head;
        list->head = new_node;
        list->graphs_number++;
    }else{
        list_node_t *cur_node = head;

        int count = 0;
        while(cur_node->next != NULL && (cur_node->next)->metric <= new_node->metric){
            if(count >= k){
                free(new_node);
                list->graphs_number++;
                return;
            }
            cur_node = cur_node->next;
            count++;
        }

        new_node->next = cur_node->next;
        cur_node->next = new_node;

        list->graphs_number++;
    }
}