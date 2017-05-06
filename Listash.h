#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define ASCENDENTE 1
#define DESCENDENTE 0


typedef struct _nodo {
 void * posMemHeap;
 struct _nodo *siguiente;
 struct _nodo *anterior;
 int pid;
} tipoNodo;

typedef tipoNodo *pNodo;
typedef tipoNodo *Lista;

/* Funciones con listas: */
void Insertar(Lista *l, void * v,int pid);
void Borrar(Lista *l,void * v);
void BorrarLista(Lista *);
void MostrarLista(Lista l, int orden);
Lista  obtenerPrimerNodo(Lista lista);
int existeNodo(Lista *lista, void * v);
