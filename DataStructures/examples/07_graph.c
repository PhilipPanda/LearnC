/*
 * 07_graph.c
 * 
 * Graph implementation with adjacency list.
 * Demonstrates DFS, BFS, and common graph algorithms.
 */

#include <stdio.h>
#include <stdlib.h>

// Node in adjacency list
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

// Graph structure
typedef struct Graph {
    int num_vertices;
    Node** adj_lists;
} Graph;

// Queue for BFS
typedef struct Queue {
    int* items;
    int front, rear, size, capacity;
} Queue;

// Create graph
Graph* create_graph(int vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->num_vertices = vertices;
    graph->adj_lists = (Node**)malloc(vertices * sizeof(Node*));
    
    for (int i = 0; i < vertices; i++) {
        graph->adj_lists[i] = NULL;
    }
    
    return graph;
}

// Add edge (undirected)
void add_edge(Graph* graph, int src, int dest) {
    // Add dest to src's list
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->vertex = dest;
    new_node->next = graph->adj_lists[src];
    graph->adj_lists[src] = new_node;
    
    // Add src to dest's list (for undirected graph)
    new_node = (Node*)malloc(sizeof(Node));
    new_node->vertex = src;
    new_node->next = graph->adj_lists[dest];
    graph->adj_lists[dest] = new_node;
}

// Print graph
void print_graph(Graph* graph) {
    printf("Graph adjacency list:\n");
    for (int i = 0; i < graph->num_vertices; i++) {
        printf("  %d: ", i);
        Node* current = graph->adj_lists[i];
        while (current != NULL) {
            printf("%d ", current->vertex);
            if (current->next != NULL) printf("-> ");
            current = current->next;
        }
        printf("\n");
    }
}

// DFS helper (recursive)
void dfs_helper(Graph* graph, int vertex, int visited[]) {
    visited[vertex] = 1;
    printf("%d ", vertex);
    
    Node* current = graph->adj_lists[vertex];
    while (current != NULL) {
        if (!visited[current->vertex]) {
            dfs_helper(graph, current->vertex, visited);
        }
        current = current->next;
    }
}

// Depth-First Search
void dfs(Graph* graph, int start) {
    int* visited = (int*)calloc(graph->num_vertices, sizeof(int));
    
    printf("DFS starting from %d: ", start);
    dfs_helper(graph, start, visited);
    printf("\n");
    
    free(visited);
}

// Queue operations for BFS
Queue* create_queue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = 0;
    queue->size = 0;
    queue->rear = capacity - 1;
    queue->items = (int*)malloc(capacity * sizeof(int));
    return queue;
}

int is_empty(Queue* queue) {
    return queue->size == 0;
}

void enqueue(Queue* queue, int item) {
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->items[queue->rear] = item;
    queue->size++;
}

int dequeue(Queue* queue) {
    int item = queue->items[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return item;
}

// Breadth-First Search
void bfs(Graph* graph, int start) {
    int* visited = (int*)calloc(graph->num_vertices, sizeof(int));
    Queue* queue = create_queue(graph->num_vertices);
    
    visited[start] = 1;
    enqueue(queue, start);
    
    printf("BFS starting from %d: ", start);
    
    while (!is_empty(queue)) {
        int vertex = dequeue(queue);
        printf("%d ", vertex);
        
        Node* current = graph->adj_lists[vertex];
        while (current != NULL) {
            int adj_vertex = current->vertex;
            if (!visited[adj_vertex]) {
                visited[adj_vertex] = 1;
                enqueue(queue, adj_vertex);
            }
            current = current->next;
        }
    }
    printf("\n");
    
    free(visited);
    free(queue->items);
    free(queue);
}

// Check if path exists (using DFS)
int has_path_helper(Graph* graph, int current, int target, int visited[]) {
    if (current == target) return 1;
    
    visited[current] = 1;
    
    Node* adj = graph->adj_lists[current];
    while (adj != NULL) {
        if (!visited[adj->vertex]) {
            if (has_path_helper(graph, adj->vertex, target, visited)) {
                return 1;
            }
        }
        adj = adj->next;
    }
    
    return 0;
}

int has_path(Graph* graph, int start, int end) {
    int* visited = (int*)calloc(graph->num_vertices, sizeof(int));
    int result = has_path_helper(graph, start, end, visited);
    free(visited);
    return result;
}

// Count connected components
void dfs_component(Graph* graph, int vertex, int visited[]) {
    visited[vertex] = 1;
    
    Node* current = graph->adj_lists[vertex];
    while (current != NULL) {
        if (!visited[current->vertex]) {
            dfs_component(graph, current->vertex, visited);
        }
        current = current->next;
    }
}

int count_components(Graph* graph) {
    int* visited = (int*)calloc(graph->num_vertices, sizeof(int));
    int count = 0;
    
    for (int i = 0; i < graph->num_vertices; i++) {
        if (!visited[i]) {
            dfs_component(graph, i, visited);
            count++;
        }
    }
    
    free(visited);
    return count;
}

// Free graph
void free_graph(Graph* graph) {
    for (int i = 0; i < graph->num_vertices; i++) {
        Node* current = graph->adj_lists[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->adj_lists);
    free(graph);
}

int main(void) {
    printf("=== Graph Data Structure ===\n\n");
    
    // Example 1: Building a graph
    printf("Example 1: Building a graph\n");
    printf("---------------------------\n");
    printf("Creating graph with 6 vertices:\n\n");
    printf("     0 --- 1\n");
    printf("     |     |\n");
    printf("     2 --- 3\n");
    printf("     |\n");
    printf("     4 --- 5\n\n");
    
    Graph* graph = create_graph(6);
    
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 3);
    add_edge(graph, 2, 3);
    add_edge(graph, 2, 4);
    add_edge(graph, 4, 5);
    
    print_graph(graph);
    
    // Example 2: DFS traversal
    printf("\n\nExample 2: Depth-First Search (DFS)\n");
    printf("------------------------------------\n");
    dfs(graph, 0);
    printf("(Explores as deep as possible before backtracking)\n");
    
    // Example 3: BFS traversal
    printf("\n\nExample 3: Breadth-First Search (BFS)\n");
    printf("--------------------------------------\n");
    bfs(graph, 0);
    printf("(Explores neighbors before going deeper)\n");
    
    // Example 4: Path finding
    printf("\n\nExample 4: Path finding\n");
    printf("-----------------------\n");
    int pairs[][2] = {{0, 5}, {0, 3}, {1, 5}};
    for (int i = 0; i < 3; i++) {
        int start = pairs[i][0];
        int end = pairs[i][1];
        if (has_path(graph, start, end)) {
            printf("  Path exists from %d to %d\n", start, end);
        } else {
            printf("  No path from %d to %d\n", start, end);
        }
    }
    
    // Example 5: Disconnected graph
    printf("\n\nExample 5: Connected components\n");
    printf("-------------------------------\n");
    
    printf("Creating disconnected graph:\n\n");
    printf("  0 --- 1     2 --- 3     4\n\n");
    
    Graph* disconnected = create_graph(5);
    add_edge(disconnected, 0, 1);
    add_edge(disconnected, 2, 3);
    
    print_graph(disconnected);
    
    int components = count_components(disconnected);
    printf("\nNumber of connected components: %d\n", components);
    
    // Example 6: Social network example
    printf("\n\nExample 6: Social network simulation\n");
    printf("-------------------------------------\n");
    printf("People: Alice(0), Bob(1), Charlie(2), Dave(3), Eve(4)\n\n");
    
    Graph* social = create_graph(5);
    
    // Friendships
    add_edge(social, 0, 1);  // Alice - Bob
    add_edge(social, 0, 2);  // Alice - Charlie
    add_edge(social, 1, 3);  // Bob - Dave
    add_edge(social, 2, 3);  // Charlie - Dave
    add_edge(social, 3, 4);  // Dave - Eve
    
    printf("Friendships:\n");
    const char* names[] = {"Alice", "Bob", "Charlie", "Dave", "Eve"};
    for (int i = 0; i < 5; i++) {
        printf("  %s's friends: ", names[i]);
        Node* current = social->adj_lists[i];
        while (current != NULL) {
            printf("%s ", names[current->vertex]);
            current = current->next;
        }
        printf("\n");
    }
    
    printf("\nFinding connections (BFS from Alice):\n");
    bfs(social, 0);
    printf("(Shows order of discovery in friend network)\n");
    
    // Cleanup
    free_graph(graph);
    free_graph(disconnected);
    free_graph(social);
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Graph Concepts:
 * 
 * What is a Graph?
 * - Set of vertices (nodes) connected by edges
 * - More flexible than trees (no hierarchy)
 * - Models relationships, networks, maps
 * 
 * Graph Types:
 * 
 * 1. Directed vs Undirected:
 *    - Undirected: A-B means A connects to B AND B connects to A
 *    - Directed: A->B means only A points to B
 * 
 * 2. Weighted vs Unweighted:
 *    - Weighted: Edges have values (distance, cost)
 *    - Unweighted: All edges equal
 * 
 * 3. Cyclic vs Acyclic:
 *    - Cyclic: Has loops (A->B->C->A)
 *    - Acyclic: No loops (DAG - Directed Acyclic Graph)
 * 
 * Representation:
 * 
 * Adjacency Matrix (2D array):
 * - matrix[i][j] = 1 if edge from i to j
 * - O(V²) space
 * - O(1) to check if edge exists
 * - Good for dense graphs
 * 
 * Adjacency List (we use this):
 * - Array of linked lists
 * - O(V + E) space
 * - O(degree) to check if edge exists
 * - Good for sparse graphs
 * 
 * Traversals:
 * 
 * DFS (Depth-First Search):
 * - Use stack (or recursion)
 * - Go deep before wide
 * - Uses: Path finding, cycle detection, topological sort
 * - Complexity: O(V + E)
 * 
 * BFS (Breadth-First Search):
 * - Use queue
 * - Explore neighbors first
 * - Uses: Shortest path (unweighted), level-order
 * - Complexity: O(V + E)
 * 
 * DFS vs BFS:
 * 
 * Example graph:
 *     1
 *    / \
 *   2   3
 *  / \
 * 4   5
 * 
 * DFS from 1: 1 2 4 5 3 (goes deep)
 * BFS from 1: 1 2 3 4 5 (level by level)
 * 
 * Common Problems:
 * 
 * - Is there a path? (DFS/BFS)
 * - Shortest path? (BFS for unweighted, Dijkstra for weighted)
 * - Is it connected? (DFS from any node, check if all visited)
 * - Has cycle? (DFS with recursion stack)
 * - Topological sort? (DFS on DAG)
 * - Minimum spanning tree? (Kruskal's, Prim's)
 * 
 * Applications:
 * 
 * - Social networks (people and friendships)
 * - Maps (cities and roads)
 * - Web pages (pages and links)
 * - Dependencies (tasks and prerequisites)
 * - Computer networks (routers and connections)
 * - Game boards (states and moves)
 * - Recommendation systems
 * 
 * Complexity:
 * - Space: O(V + E) for adjacency list
 * - DFS/BFS: O(V + E)
 * - Add edge: O(1)
 * - Remove edge: O(degree)
 * - Check edge: O(degree)
 * 
 * Advanced Algorithms:
 * - Dijkstra: Shortest path (weighted, non-negative)
 * - Bellman-Ford: Shortest path (with negative weights)
 * - Floyd-Warshall: All-pairs shortest paths
 * - Kruskal/Prim: Minimum spanning tree
 * - Tarjan: Strongly connected components
 * - A*: Heuristic pathfinding
 * 
 * Try:
 * - Implement directed graph variant
 * - Add weighted edges
 * - Implement cycle detection
 * - Find shortest path with BFS
 * - Implement topological sort
 */
