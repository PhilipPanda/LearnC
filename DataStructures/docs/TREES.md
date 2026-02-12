# Binary Trees and Binary Search Trees

## The Big Picture

Lists are linear - one element after another. Trees branch. Each node can have multiple children. This structure matches many real-world relationships: file systems, organization charts, decision trees.

The most common tree is the **binary tree** - each node has at most two children (left and right). When ordered a specific way, it becomes a **binary search tree (BST)** - enabling fast search, insert, and delete.

## Tree Terminology

```
        50
       /  \
      30   70
     / \   / \
    20 40 60 80
```

- **Root:** Top node (50)
- **Parent:** Node with children (30 is parent of 20 and 40)
- **Child:** Node below another (20 and 40 are children of 30)
- **Leaf:** Node with no children (20, 40, 60, 80)
- **Height:** Longest path from root to leaf (2 in this case)
- **Depth:** Distance from root to node
- **Subtree:** Tree formed by node and all descendants

## Binary Tree Structure

```c
typedef struct TreeNode {
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode* create_node(int data) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}
```

## Binary Search Tree (BST)

A binary tree with an ordering property:
- **Left subtree** contains only nodes with values **less than** parent
- **Right subtree** contains only nodes with values **greater than** parent
- Both subtrees are also BSTs

Example:
```
        50
       /  \
      30   70     Valid BST
     / \   / \    
    20 40 60 80   All left < parent < all right
```

This property enables binary search on a tree!

## BST Operations

### Search

```c
TreeNode* search(TreeNode* root, int target) {
    if (root == NULL || root->data == target) {
        return root;
    }
    
    if (target < root->data) {
        return search(root->left, target);  // Go left
    } else {
        return search(root->right, target); // Go right
    }
}
```

Like binary search: eliminate half the tree each step.  
**Complexity:** O(h) where h is height  
- Balanced: O(log n)
- Skewed: O(n)

### Insert

```c
TreeNode* insert(TreeNode* root, int data) {
    if (root == NULL) {
        return create_node(data);
    }
    
    if (data < root->data) {
        root->left = insert(root->left, data);
    } else if (data > root->data) {
        root->right = insert(root->right, data);
    }
    // If equal, don't insert duplicate
    
    return root;
}
```

Walk down until you find NULL spot, create node there.

Example: Insert 35 into tree above
```
        50
       /  \
      30   70
     / \   / \
    20 40 60 80
      /
     35  <- New node
```

### Delete

Most complex operation. Three cases:

**Case 1: Leaf node**
Just remove it.

**Case 2: One child**
Replace node with its child.

**Case 3: Two children**
Replace with inorder successor (smallest in right subtree) or predecessor.

```c
TreeNode* find_min(TreeNode* root) {
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

TreeNode* delete_node(TreeNode* root, int data) {
    if (root == NULL) return NULL;
    
    if (data < root->data) {
        root->left = delete_node(root->left, data);
    } else if (data > root->data) {
        root->right = delete_node(root->right, data);
    } else {
        // Found node to delete
        
        // Case 1: Leaf or Case 2: One child
        if (root->left == NULL) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }
        
        // Case 3: Two children
        TreeNode* temp = find_min(root->right);  // Inorder successor
        root->data = temp->data;
        root->right = delete_node(root->right, temp->data);
    }
    
    return root;
}
```

## Tree Traversals

Visit every node in specific order.

### Inorder (Left, Root, Right)

```c
void inorder(TreeNode* root) {
    if (root == NULL) return;
    
    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}
```

For BST, prints in **sorted order**!
```
Tree:    50
        /  \
       30   70
      / \   / \
     20 40 60 80

Inorder: 20 30 40 50 60 70 80
```

### Preorder (Root, Left, Right)

```c
void preorder(TreeNode* root) {
    if (root == NULL) return;
    
    printf("%d ", root->data);
    preorder(root->left);
    preorder(root->right);
}
```

Output: `50 30 20 40 70 60 80`

Useful for copying tree, prefix expression.

### Postorder (Left, Right, Root)

```c
void postorder(TreeNode* root) {
    if (root == NULL) return;
    
    postorder(root->left);
    postorder(root->right);
    printf("%d ", root->data);
}
```

Output: `20 40 30 60 80 70 50`

Useful for deleting tree, postfix expression.

### Level-order (Breadth-first)

```c
void level_order(TreeNode* root) {
    if (root == NULL) return;
    
    Queue* queue = create_queue(100);
    enqueue(queue, root);
    
    while (!is_empty(queue)) {
        TreeNode* node = dequeue(queue);
        printf("%d ", node->data);
        
        if (node->left) enqueue(queue, node->left);
        if (node->right) enqueue(queue, node->right);
    }
}
```

Output: `50 30 70 20 40 60 80`

Visits level by level.

## Tree Properties

### Height

```c
int height(TreeNode* root) {
    if (root == NULL) return -1;  // Or 0, depending on definition
    
    int left_height = height(root->left);
    int right_height = height(root->right);
    
    return 1 + (left_height > right_height ? left_height : right_height);
}
```

### Size (Node count)

```c
int size(TreeNode* root) {
    if (root == NULL) return 0;
    return 1 + size(root->left) + size(root->right);
}
```

### Is BST?

```c
int is_bst_helper(TreeNode* root, int min, int max) {
    if (root == NULL) return 1;
    
    if (root->data <= min || root->data >= max) {
        return 0;
    }
    
    return is_bst_helper(root->left, min, root->data) &&
           is_bst_helper(root->right, root->data, max);
}

int is_bst(TreeNode* root) {
    return is_bst_helper(root, INT_MIN, INT_MAX);
}
```

## Balanced Trees

BST can become skewed (like a linked list):

```
    50      Insert 10, 20, 30, 40, 50
      \
       40
        \
         30
          \
           20
            \
             10
```

This is O(n) search - not good!

**Balanced trees** maintain O(log n) height:
- **AVL trees:** Strict balancing (height difference ≤ 1)
- **Red-Black trees:** Looser balancing, faster inserts
- **B-trees:** Multiple children, used in databases

## Complexity Summary

| Operation | Average | Worst (Skewed) | Balanced |
|-----------|---------|----------------|----------|
| Search    | O(log n) | O(n) | O(log n) |
| Insert    | O(log n) | O(n) | O(log n) |
| Delete    | O(log n) | O(n) | O(log n) |
| Space     | O(n) | O(n) | O(n) |

## When to Use BST

**Use BST when:**
- Need sorted data with dynamic inserts/deletes
- Range queries (find all in range [a, b])
- Fast search in changing dataset
- Priority queue alternative

**Don't use when:**
- Need guaranteed O(log n) (use balanced tree)
- Frequently access by index (use array)
- Only need min/max (use heap)

## Common Problems

**Lowest Common Ancestor:**
```c
TreeNode* lca(TreeNode* root, int n1, int n2) {
    if (root == NULL) return NULL;
    
    if (root->data > n1 && root->data > n2) {
        return lca(root->left, n1, n2);
    }
    
    if (root->data < n1 && root->data < n2) {
        return lca(root->right, n1, n2);
    }
    
    return root;  // Split point is LCA
}
```

**Mirror tree:**
```c
void mirror(TreeNode* root) {
    if (root == NULL) return;
    
    TreeNode* temp = root->left;
    root->left = root->right;
    root->right = temp;
    
    mirror(root->left);
    mirror(root->right);
}
```

## Summary

Trees organize data hierarchically. BSTs add ordering for fast search. Recursion is natural for tree operations. Balance is crucial for performance.

Key insight: **Binary search isn't just for arrays - trees enable binary search on dynamic data.**
