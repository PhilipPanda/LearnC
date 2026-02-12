# Graphs

## The Big Picture

Trees have one parent per node. Graphs are more flexible - nodes (vertices) can connect to any other nodes through edges. No hierarchy, no rules, just connections.

This models real-world relationships:
- Social networks (people and friendships)
- Maps (cities and roads)
- Web pages (pages and links)
- Dependencies (tasks and prerequisites)

Graphs are everywhere once you start looking.

## Graph Terminology

```
    A --- B
    |     |
    C --- D
```

- **Vertex (node):** A point (A, B, C, D)
- **Edge:** Connection between vertices (A-B, A-C, B-D, C-D)
- **Degree:** Number of edges connected to vertex (A has degree 2)
- **Path:** Sequence of vertices connected by edges (A-C-D-B)
- **Cycle:** Path that starts and ends at same vertex (A-C-D-B-A)

## Graph Types

### Directed vs Undirected

**Undirected:** Edge has no direction (A-B means A↔B)
```
A <-> B
```

**Directed (Digraph):** Edge has direction (A→B doesn't mean B→A)
```
A --> B
```

Examples:
- Roads between cities: Undirected (usually)
- Twitter follows: Directed (A follows B ≠ B follows A)
- Dependencies: Directed (task A requires B)

### Weighted vs Unweighted

**Unweighted:** Edges are equal
```
A --- B
```

**Weighted:** Edges have values (distance, cost, time)
```
A --5-- B
```

Examples:
- Friend connections: Unweighted
- Road distances: Weighted
- Network latency: Weighted

## Graph Representation

Two main approaches:

### Adjacency Matrix

2D array where `matrix[i][j] = 1` if edge from i to j exists.

```c
typedef struct Graph {
    int** adj_matrix;
    int num_vertices;
} Graph;

Graph* create_graph(int vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->num_vertices = vertices;
    graph->adj_matrix = (int**)malloc(vertices * sizeof(int*));
    
    for (int i = 0; i < vertices; i++) {
        graph->adj_matrix[i] = (int*)calloc(vertices, sizeof(int));
    }
    
    return graph;
}

void add_edge(Graph* graph, int src, int dest) {
    graph->adj_matrix[src][dest] = 1;
    graph->adj_matrix[dest][src] = 1;  // For undirected
}
```

Example:
```
    0 --- 1
    |     |
    2 --- 3

Matrix:
    0  1  2  3
0 [ 0  1  1  0 ]
1 [ 1  0  0  1 ]
2 [ 1  0  0  1 ]
3 [ 0  1  1  0 ]
```

**Pros:**
- O(1) to check if edge exists
- Simple to implement
- Good for dense graphs

**Cons:**
- O(V²) space (wasteful for sparse graphs)
- O(V) to find all neighbors

### Adjacency List

Array of linked lists. Each vertex has list of its neighbors.

```c
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

typedef struct Graph {
    Node** adj_lists;
    int num_vertices;
} Graph;

Graph* create_graph(int vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->num_vertices = vertices;
    graph->adj_lists = (Node**)malloc(vertices * sizeof(Node*));
    
    for (int i = 0; i < vertices; i++) {
        graph->adj_lists[i] = NULL;
    }
    
    return graph;
}

void add_edge(Graph* graph, int src, int dest) {
    // Add dest to src's list
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->vertex = dest;
    new_node->next = graph->adj_lists[src];
    graph->adj_lists[src] = new_node;
    
    // Add src to dest's list (for undirected)
    new_node = (Node*)malloc(sizeof(Node));
    new_node->vertex = src;
    new_node->next = graph->adj_lists[dest];
    graph->adj_lists[dest] = new_node;
}
```

Example:
```
    0 --- 1
    |     |
    2 --- 3

Lists:
0: [1] -> [2] -> NULL
1: [0] -> [3] -> NULL
2: [0] -> [3] -> NULL
3: [1] -> [2] -> NULL
```

**Pros:**
- O(V + E) space (efficient for sparse graphs)
- O(1) to add edge
- O(degree) to find all neighbors

**Cons:**
- O(degree) to check if edge exists
- Slightly more complex

**Rule of thumb:** Use adjacency list unless graph is very dense.

## Graph Traversal

Visit every vertex in the graph.

### Depth-First Search (DFS)

Explore as far as possible before backtracking. Like exploring a maze.

```c
void dfs_helper(Graph* graph, int vertex, int visited[]) {
    visited[vertex] = 1;
    printf("%d ", vertex);
    
    Node* current = graph->adj_lists[vertex];
    while (current != NULL) {
        int adj_vertex = current->vertex;
        if (!visited[adj_vertex]) {
            dfs_helper(graph, adj_vertex, visited);
        }
        current = current->next;
    }
}

void dfs(Graph* graph, int start) {
    int* visited = (int*)calloc(graph->num_vertices, sizeof(int));
    dfs_helper(graph, start, visited);
    free(visited);
}
```

Iterative with stack:
```c
void dfs_iterative(Graph* graph, int start) {
    int* visited = (int*)calloc(graph->num_vertices, sizeof(int));
    Stack* stack = create_stack(graph->num_vertices);
    
    push(stack, start);
    
    while (!is_empty(stack)) {
        int vertex = pop(stack);
        
        if (visited[vertex]) continue;
        
        visited[vertex] = 1;
        printf("%d ", vertex);
        
        Node* current = graph->adj_lists[vertex];
        while (current != NULL) {
            if (!visited[current->vertex]) {
                push(stack, current->vertex);
            }
            current = current->next;
        }
    }
    
    free(visited);
}
```

**Uses:**
- Finding paths
- Detecting cycles
- Topological sort
- Maze solving

**Complexity:** O(V + E)

### Breadth-First Search (BFS)

Explore neighbors first before going deeper. Like ripples in water.

```c
void bfs(Graph* graph, int start) {
    int* visited = (int*)calloc(graph->num_vertices, sizeof(int));
    Queue* queue = create_queue(graph->num_vertices);
    
    visited[start] = 1;
    enqueue(queue, start);
    
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
    
    free(visited);
}
```

**Uses:**
- Shortest path (unweighted)
- Level-order problems
- Finding connected components
- Social network distance

**Complexity:** O(V + E)

## Common Graph Problems

### Has Path?

```c
int has_path_dfs(Graph* graph, int start, int target, int visited[]) {
    if (start == target) return 1;
    
    visited[start] = 1;
    
    Node* current = graph->adj_lists[start];
    while (current != NULL) {
        if (!visited[current->vertex]) {
            if (has_path_dfs(graph, current->vertex, target, visited)) {
                return 1;
            }
        }
        current = current->next;
    }
    
    return 0;
}
```

### Cycle Detection

```c
int has_cycle_helper(Graph* graph, int vertex, int visited[], int rec_stack[]) {
    visited[vertex] = 1;
    rec_stack[vertex] = 1;
    
    Node* current = graph->adj_lists[vertex];
    while (current != NULL) {
        int adj = current->vertex;
        
        if (!visited[adj]) {
            if (has_cycle_helper(graph, adj, visited, rec_stack)) {
                return 1;
            }
        } else if (rec_stack[adj]) {
            return 1;  // Back edge found
        }
        
        current = current->next;
    }
    
    rec_stack[vertex] = 0;
    return 0;
}
```

### Connected Components

```c
int count_components(Graph* graph) {
    int* visited = (int*)calloc(graph->num_vertices, sizeof(int));
    int count = 0;
    
    for (int i = 0; i < graph->num_vertices; i++) {
        if (!visited[i]) {
            dfs_helper(graph, i, visited);
            count++;
        }
    }
    
    free(visited);
    return count;
}
```

### Topological Sort

Order vertices so all edges go left to right. Only works on directed acyclic graphs (DAG).

Example: Task scheduling with dependencies.

```c
void topological_sort_helper(Graph* graph, int vertex, int visited[], Stack* stack) {
    visited[vertex] = 1;
    
    Node* current = graph->adj_lists[vertex];
    while (current != NULL) {
        if (!visited[current->vertex]) {
            topological_sort_helper(graph, current->vertex, visited, stack);
        }
        current = current->next;
    }
    
    push(stack, vertex);  // Push after visiting all neighbors
}
```

## When to Use Graphs

**Use graphs when:**
- Modeling relationships
- Network analysis
- Finding paths
- Dependency resolution
- Recommendation systems
- Game maps

**Graph algorithms you'll encounter:**
- DFS/BFS: Traversal, reachability
- Dijkstra: Shortest path (weighted)
- Bellman-Ford: Shortest path (negative weights)
- Floyd-Warshall: All-pairs shortest path
- Kruskal/Prim: Minimum spanning tree
- Network flow: Max flow, min cut

## Summary

Graphs are the ultimate flexible data structure. Vertices and edges model countless real-world systems. Two representations (matrix vs list) trade space for speed. DFS explores depth, BFS explores breadth. Master graphs, and you can model almost anything.

Key insight: **Most complex problems are graph problems in disguise. Learn to see the graph.**
