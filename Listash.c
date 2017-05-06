
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "Listash.h"

#define ASCENDENTE 1
#define DESCENDENTE 0
#define TRUE 1
#define FALSE 0

void Insertar(Lista *lista, void * v,int pid)
{
 pNodo nuevo, actual;
 /* Crear un nodo nuevo */
 nuevo = (pNodo)malloc(sizeof(tipoNodo));

 nuevo->posMemHeap = v;
 nuevo->pid=pid;
 /* Colocamos actual en la primera posición de la lista */
 actual = *lista;
 if(actual) while(actual->anterior) actual = actual->anterior;
 /* Si la lista está vacía o el primer miembro es mayor que el nuevo */
 if(!actual || actual->posMemHeap > v) {
 /* Añadimos la lista a continuación del nuevo nodo */
 nuevo->siguiente = actual;
 nuevo->anterior = NULL;
 if(actual) actual->anterior = nuevo;
 if(!*lista) *lista = nuevo;
 }
 else {
 /* Avanzamos hasta el último elemento o hasta que el siguiente tenga
 un valor mayor que v */
 while(actual->siguiente &&actual->siguiente->posMemHeap <= v)
 actual = actual->siguiente;
 /* Insertamos el nuevo nodo después del nodo anterior */
 nuevo->siguiente = actual->siguiente;
 actual->siguiente = nuevo;
 nuevo->anterior = actual;
 if(nuevo->siguiente) nuevo->siguiente->anterior = nuevo;
 }
}
void Borrar(Lista *lista, void * v)
{
pNodo nodo;
/* Buscar el nodo de valor v */
nodo = *lista;
while(nodo && nodo->posMemHeap < v) nodo = nodo->siguiente;
while(nodo && nodo->posMemHeap > v) nodo = nodo->anterior;
/* El valor v no está en la lista */
if(!nodo || nodo->posMemHeap != v) return;
/* Borrar el nodo */
/* Si lista apunta al nodo que queremos borrar, apuntar a otro */
if(nodo == *lista)
if(nodo->anterior) *lista = nodo->anterior;
else *lista = nodo->siguiente;
if(nodo->anterior) /* no es el primer elemento */
nodo->anterior->siguiente = nodo->siguiente;
if(nodo->siguiente) /* no es el último nodo */
nodo->siguiente->anterior = nodo->anterior;
free(nodo);
}
int existeNodo(Lista *lista, void * v)
{
pNodo nodo;
/* Buscar el nodo de valor v */
nodo = *lista;
while(nodo->anterior) nodo = nodo->anterior;
while(nodo && nodo->posMemHeap < v) nodo = nodo->siguiente;
while(nodo && nodo->posMemHeap > v) nodo = nodo->anterior;
/* El valor v no está en la lista */
if(!nodo || nodo->posMemHeap != v) return 0;
else return 1;

}
void BorrarLista(Lista *lista)
{
 pNodo nodo, actual;
 actual = *lista;
 while(actual->anterior) actual = actual->anterior;

 while(actual) {
 nodo = actual;
 actual = actual->siguiente;
 free(nodo);
 }
 *lista = NULL;
}
void MostrarLista(Lista lista, int orden)
{
 pNodo nodo = lista;
 if(!lista) printf("Lista vacía");
 nodo = lista;
 if(orden == ASCENDENTE) {
 while(nodo->anterior) nodo = nodo->anterior;
 printf("Orden ascendente: ");
 while(nodo) {
 printf("%d -> ", nodo->posMemHeap);
 nodo = nodo->siguiente;
 }
 }
 else {
 while(nodo->siguiente) nodo = nodo->siguiente;
 printf("Orden descendente: ");
 while(nodo) {
 printf("%d -> ", nodo->posMemHeap);
 nodo = nodo->anterior;
 }
 }
 printf("\n");
}

Lista obtenerPrimerNodo(Lista lista){

 pNodo nodo;
 nodo=lista;
 while(nodo->anterior) nodo = nodo->anterior;

return nodo;
}
