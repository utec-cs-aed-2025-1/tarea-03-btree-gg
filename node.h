#ifndef NODE_H
#define NODE_H

using namespace std;

template <typename TK>
struct Node {
  // array de keys
  TK* keys;
  // array de punteros a hijos
  Node** children;
  // cantidad de keys
  int count;
  // indicador de nodo hoja
  bool leaf;

  Node() : keys(nullptr), children(nullptr), count(0) {}
  Node(int M) {
    keys = new TK[M - 1];
    children = new Node<TK>*[M];
    count = 0;
    leaf = true;
  }

  void killSelf() {
    // delete del array de keys
    if (keys) {
      delete[] keys;
      keys = nullptr;
    }
    
    // para el ptr array
    if (children != nullptr){
      // si no es hoja se borran los children 
      if (!leaf){
        for (int i=0; i<=count; i++){
          if(children[i] != nullptr){
            children[i]->killSelf();
            delete children[i];
            children[i]=nullptr;
          }
        }
      }
      
      delete[] children;
      children=nullptr;
    }

    // reset
    count = 0;
    leaf = true;
  }
};

#endif