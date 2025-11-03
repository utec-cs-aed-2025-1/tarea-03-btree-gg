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

  void stringInorder(Node<TK>* node, string& result, const string& sep){
    if (node == nullptr) return;
    
    for (int i = 0; i < node->count; i++) {
      // visitar hijo izquierdo
      if (!node->leaf) {
        stringInorder(node->children[i], result, sep);
      }
      
      // agregar clave actual
      if (result.length() > 0) {
        result += sep;
      }
      result += to_string(node->keys[i]);
    }
    
    // visitar último hijo
    if (!node->leaf) {
      stringInorder(node->children[node->count], result, sep);
    }
  }

  void rangeSearchH(Node<TK>* nodo, TK inicio, TK fin, vector<TK>& elementos){
    if (nodo == nullptr) return;
    int i = 0;
    for (i = 0; i < nodo->count; i++) {
      if (!nodo->leaf) {
        rangeSearchH(nodo->children[i], inicio, fin, elementos);
      }
      // si la clave esta en el rango se agrega
      if (nodo->keys[i] >= inicio && nodo->keys[i] <= fin) {
        elementos.push_back(nodo->keys[i]);
      }
      else if (nodo->keys[i] > fin) {
        return;
      }
    }
    
    // visitar ultimo hijo
    if (!nodo->leaf) {
      rangeSearchH(nodo->children[i], inicio, fin, elementos);
    }
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

  // obtener predecesor (max del subarbol izquierdo)
  TK getPredecessor(Node<TK>* node, int idx){
    Node<TK>* temp = node->children[idx];
    while (!temp->leaf) {
      temp = temp->children[temp->count];
    }
    return temp->keys[temp->count - 1];
  }

  // obtener sucesor (min del subarbol derecho)
  TK getSuccessor(Node<TK>* node, int idx){
    Node<TK>* temp = node->children[idx + 1];
    while (!temp->leaf) {
      temp = temp->children[0];
    }
    return temp->keys[0];
  }

  // fusionar hijo en idx con su hermano derecho
  void merge(Node<TK>* node, int idx){
    Node<TK>* child = node->children[idx];
    Node<TK>* sibling = node->children[idx + 1];

    // bajar la clave del padre al hijo
    child->keys[child->count] = node->keys[idx];

    // copiar claves del hermano al hijo
    for (int i = 0; i < sibling->count; i++) {
      child->keys[child->count + 1 + i] = sibling->keys[i];
    }

    // copiar punteros a hijos si no es hoja
    if (!child->leaf) {
      for (int i = 0; i <= sibling->count; i++) {
        child->children[child->count + 1 + i] = sibling->children[i];
      }
    }

    // actualizar count del hijo
    child->count += sibling->count + 1;

    // mover keys del padre hacia la izquierda
    for (int i = idx + 1; i < node->count; i++) {
      node->keys[i - 1] = node->keys[i];
    }

    // mover punteros a hijos hacia la izquierda
    for (int i = idx + 2; i <= node->count; i++) {
      node->children[i - 1] = node->children[i];
    }

    node->count--;
    
    // liberar memoria del hermano sin llamar killSelf
    delete[] sibling->keys;
    delete[] sibling->children;
    delete sibling;
  }

  // tomar una clave del hermano izquierdo
  void borrowFromPrev(Node<TK>* node, int idx){
    Node<TK>* child = node->children[idx];
    Node<TK>* sibling = node->children[idx - 1];

    // mover keys del hijo hacia la derecha
    for (int i = child->count - 1; i >= 0; i--) {
      child->keys[i + 1] = child->keys[i];
    }

    // mover children si no es hoja
    if (!child->leaf) {
      for (int i = child->count; i >= 0; i--) {
        child->children[i + 1] = child->children[i];
      }
    }

    // bajar clave del padre al hijo
    child->keys[0] = node->keys[idx - 1];

    // mover puntero si no es hoja
    if (!child->leaf) {
      child->children[0] = sibling->children[sibling->count];
    }

    // subir clave del hermano al padre
    node->keys[idx - 1] = sibling->keys[sibling->count - 1];

    child->count++;
    sibling->count--;
  }

  // tomar una clave del hermano derecho
  void borrowFromNext(Node<TK>* node, int idx){
    Node<TK>* child = node->children[idx];
    Node<TK>* sibling = node->children[idx + 1];

    // bajar clave del padre al hijo
    child->keys[child->count] = node->keys[idx];

    // mover primer puntero del hermano si no es hoja
    if (!child->leaf) {
      child->children[child->count + 1] = sibling->children[0];
    }

    // subir primera clave del hermano al padre
    node->keys[idx] = sibling->keys[0];

    // mover keys del hermano hacia la izquierda
    for (int i = 1; i < sibling->count; i++) {
      sibling->keys[i - 1] = sibling->keys[i];
    }

    // mover children del hermano si no es hoja
    if (!sibling->leaf) {
      for (int i = 1; i <= sibling->count; i++) {
        sibling->children[i - 1] = sibling->children[i];
      }
    }

    child->count++;
    sibling->count--;
  }

  // llenar hijo que tiene menos del minimo de claves
  void fill(Node<TK>* node, int idx){
    int t = (M + 1) / 2;

    // si el hermano izquierdo tiene mas del minimo
    if (idx != 0 && node->children[idx - 1]->count >= t) {
      borrowFromPrev(node, idx);
    }
    // si el hermano derecho tiene mas del minimo
    else if (idx != node->count && node->children[idx + 1]->count >= t) {
      borrowFromNext(node, idx);
    }
    // fusionar con hermano
    else {
      if (idx != node->count) {
        merge(node, idx);
      } else {
        merge(node, idx - 1);
      }
    }
  }

  // eliminar clave de nodo hoja
  void removeFromLeaf(Node<TK>* node, int idx){
    // desplazar claves hacia la izquierda
    for (int i = idx + 1; i < node->count; i++) {
      node->keys[i - 1] = node->keys[i];
    }
    node->count--;
  }

  // eliminar clave de nodo interno
  void removeFromNonLeaf(Node<TK>* node, int idx){
    TK key = node->keys[idx];
    int t = (M + 1) / 2;

    // si el hijo izquierdo tiene suficientes claves
    if (node->children[idx]->count >= t) {
      TK pred = getPredecessor(node, idx);
      node->keys[idx] = pred;
      removeHelper(node->children[idx], pred);
    }
    // si el hijo derecho tiene suficientes claves
    else if (node->children[idx + 1]->count >= t) {
      TK succ = getSuccessor(node, idx);
      node->keys[idx] = succ;
      removeHelper(node->children[idx + 1], succ);
    }
    // fusionar hijos
    else {
      merge(node, idx);
      removeHelper(node->children[idx], key);
    }
  }

  // funcion auxiliar recursiva para eliminar
  bool removeHelper(Node<TK>* node, TK key){
    int idx = 0;
    int t = (M + 1) / 2;

    // buscar la posicion de la clave
    while (idx < node->count && node->keys[idx] < key) {
      idx++;
    }

    // si la clave esta en este nodo
    if (idx < node->count && node->keys[idx] == key) {
      if (node->leaf) {
        removeFromLeaf(node, idx);
      } else {
        removeFromNonLeaf(node, idx);
      }
      return true;
    }
    // si la clave no esta en este nodo
    else {
      if (node->leaf) {
        return false; // la clave no existe
      }

      // verificar si la clave esta en el subarbol
      bool isInSubtree = (idx == node->count);

      // si el hijo tiene menos del minimo de claves
      if (node->children[idx]->count < t) {
        fill(node, idx);
      }

      // despues de fill la clave podria moverse
      if (isInSubtree && idx > node->count) {
        return removeHelper(node->children[idx - 1], key);
      } else {
        // buscar nuevamente la posicion correcta
        idx = 0;
        while (idx < node->count && node->keys[idx] < key) {
          idx++;
        }
        return removeHelper(node->children[idx], key);
      }
    }
  }

  void remove(TK key){
    if (!root) return;

    bool found = removeHelper(root, key);
    
    // solo decrementar si se encontro la clave
    if (found) {
      n--;
    }

    // si root queda vacia se actualiza
    if (root->count == 0) {
      Node<TK>* temp = root;
      if (root->leaf) {
        root = nullptr;
        delete[] temp->keys;
        delete[] temp->children;
        delete temp;
      } else {
        root = root->children[0];
        delete[] temp->keys;
        delete[] temp->children;
        delete temp;
      }
    }
  }

  int height(){
    if (!root) return 0;
    int h = 1;
    Node<TK>* temp = root;
    while (!temp->leaf) {
      temp = temp->children[0];
      h++;
    }
    return h;
  }//altura del arbol. Considerar altura 0 para arbol vacio

  string toString(const string& sep){
    string c = "";
    stringInorder(root, c, sep);
    return c;
  }

  vector<TK> rangeSearch(TK begin, TK end){
    vector<TK> elementos = {};
    rangeSearchH(root, begin, end, elementos);
    return elementos;
  }

  TK minKey(){
   if (!root) {
    throw runtime_error("El árbol está vacío");
  }
   Node<TK>* temp = root;
   while (!temp->leaf) {
    temp = temp->children[0];  
  }
   return temp->keys[0];
  }// minimo valor de la llave en el arbol

  TK maxKey(){
   if (!root) {
    throw runtime_error("El árbol está vacío");
  }
   Node<TK>* temp = root;
   while (!temp->leaf) {
    temp = temp->children[temp->count];  
  }
   return temp->keys[temp->count-1];
  }// maximo valor de la llave en el arbol

  void clear(){
    if(root != nullptr){
      root->killSelf(); // metodo del node
      delete root;
      root=nullptr;
    }
    n = 0;
  }// eliminar todos lo elementos del arbol
  
  int size(){
    return n;
  }// retorna el total de elementos insertados  
  
  // Construya un árbol B a partir de un vector de elementos ordenados
  template<typename T>
  static BTree* build_from_ordered_vector(vector<T> elements, int x){
    BTree* arbol = new BTree(x);
    for(int i=0; i < elements.size(); i++){
      arbol->insert(elements[i]);
    }
    return arbol;
  }

  // Verifique las propiedades de un árbol B
  bool check_properties(){
    if(root == nullptr) return true;
    if(n == 0) return true;
    
    return true; // Por ahora solo retorna true
  }

  ~BTree(){
    clear();
  }// liberar memoria
};

#endif
