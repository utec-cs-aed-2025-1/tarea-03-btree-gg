#ifndef BTree_H
#define BTree_H
#include <iostream>
#include "node.h"
#include <vector>

using namespace std;

template <typename TK>
class BTree {
 private:
  Node<TK>* root;
  int M;  // grado u orden del arbol
  int n; // total de elementos en el arbol 

  // funcion split para el insert
  void split(Node<TK>* parent, int i){
    Node<TK>* y = parent->children[i];
    int maxKeys = M - 1;
    int median = maxKeys / 2;

    Node<TK>* z = new Node<TK>(M);
    z->leaf = y->leaf;

    int j = 0;
    for (int k = median+1; k < y->count; k++) {
      z->keys[j++] = y->keys[k];
    }
    z->count = j;

    // si no es hoja mover los children
    if (!y->leaf) {
      int cj = 0;
      for (int k = median + 1; k <= y->count; k++) {
        z->children[cj++] = y->children[k];
        y->children[k] = nullptr;
      }
    }

    y->count = median;

    // mover children del parent
    for (int k = parent->count; k >= i + 1; k--) {
      parent->children[k + 1] = parent->children[k];
    }
    parent->children[i + 1] = z;

    // desplazar keys del parent y meter la mediana
    for (int k = parent->count - 1; k >= i; k--) {
      parent->keys[k + 1] = parent->keys[k];
    }
    parent->keys[i] = y->keys[median];
    parent->count += 1;
  }

 public:
  BTree(int _M) : root(nullptr), M(_M), n(0){}

  bool search(TK key){
    Node<TK>* temp=root;
    while (temp != nullptr){
      int i=0;
      while(i<temp->count && key>temp->keys[i]){
        i++;
      }
      // para el nodo actual
      if(i<temp->count && key==temp->keys[i]){ 
        return true;
      }
      if(temp->leaf){
        return false;
      }

      temp=temp->children[i];
    }
    return false; // ya revisamos todo
  }//indica si se encuentra o no un elemento

  void insert(TK key){
    // si el árbol está vacío se crea root
    if(root == nullptr){
      root=new Node<TK>(M);
      root->keys[0]=key;
      root->count=1;
      root->leaf=true;
      n=1;
      return;
    }

    Node<TK>* current = root;
    Node<TK>* parent = nullptr;
    int parentIndex = 0;
    while (true){
      int i = current->count-1;
      
      if(current->leaf){
        // desplazar keys hacia la derecha para hacer espacio
        while(i >= 0 && key < current->keys[i]){
          current->keys[i + 1] = current->keys[i];
          i--;
        }
        current->keys[i+1] = key;
        current->count += 1;
        n++;

        // split si el nodo sobrepasa el tamaño
        if(current->count > M-1){
          if(current == root){
            Node<TK>* s = new Node<TK>(M);
            s->leaf = false;
            s->children[0] = root;
            split(s, 0);
            root = s;
          } else {
            split(parent, parentIndex);
          }
        }
        return;
      } 
      else{
        i = current->count-1;
        // se bja hasta el hijo que cumpla
        while (i>=0 && key<current->keys[i]){
          i--;
        }
        i++;
        
        // guardar el padre
        parent = current;
        parentIndex = i;
        current = current->children[i];
      }
    }
  }//inserta un elemento

  void remove(TK key);//elimina un elemento
  int height(){
    if (!root) return 0;
    int h = 1;
    Node<TK>* temp = root;
    while (!temp->leaf) {
      temp = temp->children[0];
      h++;
    }
    return h;
  };
//altura del arbol. Considerar altura 0 para arbol vacio
  string toString(const string& sep);  // recorrido inorder
  vector<TK> rangeSearch(TK begin, TK end);

  TK minKey();  // minimo valor de la llave en el arbol
  TK maxKey();  // maximo valor de la llave en el arbol
  void clear(); // eliminar todos lo elementos del arbol
  int size(); // retorna el total de elementos insertados  
  
  // Construya un árbol B a partir de un vector de elementos ordenados
  template<typename T>
  static BTree* build_from_ordered_vector(vector<T> elements, int x){};
  // Verifique las propiedades de un árbol B
  bool check_properties();

  ~BTree();     // liberar memoria
};

#endif
