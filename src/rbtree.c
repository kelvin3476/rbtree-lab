#include "rbtree.h"

#include <stdlib.h>

void traverse_delete(rbtree *t, node_t *node);
void rbtree_insert_fixup(rbtree *t, node_t *z);
void rbtree_left_rotate(rbtree *t, node_t *x);
void rbtree_right_rotate(rbtree *t, node_t *y);
void rb_transplant(rbtree *t, node_t *u, node_t *v);
node_t *successor(rbtree *t, node_t *cur);
int inorder(node_t *cur, key_t *arr, const rbtree *t, int i, size_t n);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;

  p->root = nil;
  p->nil = nil;

  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  node_t *node = t->root;
  if (node != t->nil)
    traverse_delete(t, node);

  // memory of nil node and rbtree struct return
  free(t->nil);
  free(t);
}

void traverse_delete(rbtree *t, node_t *node) {
  if (node->left != t->nil)
    traverse_delete(t, node->left);
  if (node->right != t->nil)
    traverse_delete(t, node->right);

  free(node);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *z = (node_t *)malloc(sizeof(node_t));

  // new node z initialize
  z->key = key;
  z->parent = NULL;
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;

  // find location to insert
  node_t *y = t->nil;
  node_t *x = t->root;
  while (x != t->nil) {
    y = x;
    if (key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }

  // insert z
  z->parent = y;
  if (y == t->nil) {
    t->root = z;
  } else if (key < y->key) {
    y->left = z;
  } else {
    y->right = z;
  }

  // perform rotation and color change after insertion in order to maintain the properties of rb tree
  rbtree_insert_fixup(t, z);
 
  return t->root;
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  while (z->parent && z->parent->color == RBTREE_RED) {
    if (z->parent == z->parent->parent->left) { // case 1: parent is left child of grandparent
      node_t *y = z->parent->parent->right;
      if (y && y->color == RBTREE_RED) { // case 1.1: uncle is red
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else { // case 1.2: uncle is black
        if (z == z->parent->right) { // case 1.2.1: z is a right child
          z = z->parent;
          rbtree_left_rotate(t, z);
        }
        // case 1.2.2: z is a left child
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        rbtree_right_rotate(t, z->parent->parent);
      }
    } else { // case 2: parent is right child of grandparent
      node_t *y = z->parent->parent->left;
      if (y && y->color == RBTREE_RED) { // case 2.1: uncle is red
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else { // case 2.2: uncle is black
        if (z == z->parent->left) { // case 2.2.1: z is a left child
          z = z->parent;
          rbtree_right_rotate(t, z);
        }
        // case 2.2.
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        rbtree_left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

void rbtree_left_rotate(rbtree *t, node_t *tmp) {
  node_t *x = tmp->right;
  tmp->right = x->left;
  if (x->left != t->nil) {
    x->left->parent = tmp;
  }
  x->parent = tmp->parent;
  if (tmp->parent == t->nil) {
    t->root = x;
  } else if (tmp == tmp->parent->left) {
    tmp->parent->left = x;
  } else {
    tmp->parent->right = x;
  }
  x->left = tmp;
  tmp->parent = x;
}

void rbtree_right_rotate(rbtree *t, node_t *tmp) {
  node_t *x = tmp->left;
  tmp->left = x->right;
  if (x->right != t->nil) {
    x->right->parent = tmp;
  }
  x->parent = tmp->parent;
  if (tmp->parent == t->nil) {
    t->root = x;
  } else if (tmp == tmp->parent->right) {
    tmp->parent->right = x;
  } else {
    tmp->parent->left = x;
  }
  x->right = tmp;
  tmp->parent = x;
}


node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *tmp = t->root;
  while (tmp != t->nil) {
    if (tmp->key == key) {
      return tmp;
    } else if (tmp->key > key) {
      tmp = tmp->left;
    } else {
      tmp = tmp->right;
    }
  }
  if (tmp == t->nil) {
    return NULL;
  }
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
    node_t *current = t->root;
    while (current->left != t->nil)
      current = current->left;
    return current;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *current = t->root;
  while (current->right != t->nil)
    current = current->right;
  return current;
}

//RB Tree에서 지정된 노드p를 제거
int rbtree_erase(rbtree *t, node_t *p) {
  //p가 없는 노드이면 삭제 작업 안함
  if (p == NULL) {
    return 0;
  }

  // y : 삭제할 노드, x : y의 원래의 위치로 이동할 노드
  node_t *y = p;
  color_t y_original_color = y->color;
  node_t *x;

  //p가 오른쪽 자식만 가질 경우
  if (p->left == t->nil) {
    x = p->right;
    rb_transplant(t, p, p->right);
  } //p가 왼쪽 자식만 가질 경우
  else if (p->right == t->nil) {
    x = p->left;
    rb_transplant(t, p, p->left);
  } //양쪽 자식 모두 가질 경우
  else {
    //오른쪽 서브트리에서 가장 작은 수 반환
    //successor 를 찾는다.
    y = successor(t,p->right);
    y_original_color = y->color;
    x = y->right;
    if (y->parent == p) {
      x->parent = y;
    }
    else {
      rb_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rb_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }

  //RB-erase-Fixup------------------------------
  if (y_original_color == RBTREE_BLACK) {
    while (x != t->root && x->color == RBTREE_BLACK){
      if(x == x->parent->left) {
        node_t *w = x->parent->right;
        if(w->color == RBTREE_RED) {
          w->color = RBTREE_BLACK;
          x->parent->color = RBTREE_RED;
          rbtree_left_rotate(t, x->parent);
          w = x->parent->right;
        }
        if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
          w->color = RBTREE_RED;
          x = x->parent;
        }
        else {
          if (w->right->color == RBTREE_BLACK) {
            w->left->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            rbtree_right_rotate(t, w);
            w = x->parent->right;
          }
          w->color = x->parent->color;
          x->parent->color = RBTREE_BLACK;
          w->right->color = RBTREE_BLACK;
          rbtree_left_rotate(t, x->parent);
          x = t->root;
        }
      }
      else {
        node_t *w = x->parent->left;
        if(w->color == RBTREE_RED) {
          w->color = RBTREE_BLACK;
          x->parent->color = RBTREE_RED;
          rbtree_right_rotate(t, x->parent);
          w = x->parent->left;
        }
        if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
          w->color = RBTREE_RED;
          x = x->parent;
        }
        else {
          if (w->left->color == RBTREE_BLACK) {
            w->right->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            rbtree_left_rotate(t, w);
            w = x->parent->left;
          }
          w->color = x->parent->color;
          x->parent->color = RBTREE_BLACK;
          w->left->color = RBTREE_BLACK;
          rbtree_right_rotate(t, x->parent);
          x = t->root;
        }
      }
    }
    x->color = RBTREE_BLACK;
  }
  free(p);
  return 0;
}

//u : 삭제할 노드, v : u의 자식 노드 - u의 부모 노드와 연결되어야 한다.
void rb_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) {
    t->root = v;
  }
  else if (u == u->parent->left) {
    u->parent->left = v;
  }
  else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

node_t *successor(rbtree *t, node_t *cur) {
  node_t *n = cur;
  while(n->left != t->nil) {
    n = n->left;
  }
  return n;
}

int inorder(node_t *cur, key_t *arr, const rbtree *t, int i, size_t n) {
  if (i < n) {
    if (cur->left != t->nil) {
      i = inorder(cur->left, arr, t, i, n);
    }
    arr[i++] = cur->key;
    if (cur->right != t->nil) {
      i = inorder(cur->right, arr, t, i, n);
    }
  }
  return i;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  if (!inorder(t->root, arr, t, 0, n)) {
    return 1;
  }
  return 0;
}