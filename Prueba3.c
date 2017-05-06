#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "Listash.c"
#define TAM_BLOQUE 2048
#define TAM_PAGINA 512
#define CANTCONECIONES 10 	// Si quiero el maximo de conexiones posibles en el sockect reemplazar por 'SOMAXCONN'
#define TRUE 1
#define FALSE 0


Lista listaMemoriaLibre = NULL;
pNodo p;

Lista listaMemoriaOcupada = NULL;
pNodo q;

struct heapMetadata{
    uint32_t size;
	bool isFree;
};

struct heapMetadataID{
    uint32_t address;   //Direccion de memoria del HeapMetaData
    uint32_t PID;       //PID del programa que luego del HeapMetadata
    uint32_t PSize;     //Tamanio total del programa
};
//Creo esta estructura para guardar los datos con su tamanio por que si no no se puede saber con un sizeof por ser void
struct datosStruct{
    void * dato;
    unsigned int tamDatos;
    int pid;
};

struct heapMetadatasContainer{
    unsigned long int cantidad;
    void * heapMetadatas;
};


typedef struct heapMetadata metadata;

typedef struct heapMetadataID metadataID;

typedef struct datosStruct tDato;

void * nuevoBloqueDeMemoria();
metadata obtengoHeapMetadata(void * pagina,int posicionDeArranque);
void dividoMemoria(void * memoria);
bool puedoAlojarDatosEnUnaPagina(metadata a, int tamDatos);
void * agregarDatosABloqueDeMemoria(void * bloque, tDato datos);
tDato creoDato(void * dato,unsigned int tamDatos);
bool puedoAlojarDatos(void * bloque, int tamDatos);


int main(void)
{

    void * a = nuevoBloqueDeMemoria();

    pNodo n;

    tDato datito;
    tDato dato;

    void * datos0=malloc(498);
    datito.dato=datos0;
    datito.tamDatos=498;
    datito.pid=15;

    void * datos1=malloc(10);
    dato.dato=datos1;
    dato.tamDatos=10;
    dato.pid=3;

n= obtenerPrimerNodo(listaMemoriaLibre);
metadata metaData;

    metaData = obtengoHeapMetadata(a,n->posMemHeap);
    printf("Memoria libre en primer nodo libre: %d bytes \n\n",metaData.size);

    printf("memoria libre antes del copiado ");
    MostrarLista(listaMemoriaLibre,ASCENDENTE);
    printf("memoria ocupada antes del copiado ");
    MostrarLista(listaMemoriaOcupada,ASCENDENTE);





    a = agregarDatosABloqueDeMemoria(a,datito);
    a = agregarDatosABloqueDeMemoria(a,dato);
n= obtenerPrimerNodo(listaMemoriaLibre);
      metaData = obtengoHeapMetadata(a,n->posMemHeap);
    printf("Memoria libre en primer nodo libre luego de un copiado: %d bytes \n\n",metaData.size);
    n= obtenerPrimerNodo(listaMemoriaOcupada->siguiente);
      metaData = obtengoHeapMetadata(a,n->posMemHeap);
printf("memoria ocupada del primer nodo despue del primer copiado: %d \n",metaData.size);








    printf("memoria ocupada despues del copiado ");
    MostrarLista(listaMemoriaOcupada,ASCENDENTE);


    printf("memoria libre despues del copiado ");
    MostrarLista(listaMemoriaLibre,ASCENDENTE);

    pNodo m= obtenerPrimerNodo(listaMemoriaLibre);
    metaData = obtengoHeapMetadata(a,m->posMemHeap);
    printf("El primer nodo libre luego de la copia es: %d \n",m->posMemHeap);

 printf("\nMemoria libre en primer nodo libre despues de un copiado: %d bytes \n\n",metaData.size);

    m= obtenerPrimerNodo(listaMemoriaOcupada);
    metaData = obtengoHeapMetadata(a,m->siguiente->siguiente->posMemHeap);
    printf("\nMemoria Ocupada en tercer nodo ocupado despues de un copiado: %d bytes  \n",metaData.size);
    printf("ACLARACION: El primer nodo ocupado tiene 496 bytes, el segundo 0 y el tercero 2 bytes \n");
    printf("Pertenece al programa con PID: %d bytes  \n\n",m->siguiente->siguiente->pid);

    m= obtenerPrimerNodo(listaMemoriaOcupada);
    metaData = obtengoHeapMetadata(a,m->siguiente->siguiente->siguiente->posMemHeap);
    printf("\nMemoria Ocupada en cuarto nodo ocupado despues de un segundo copiado: %d bytes  \n",metaData.size);
    printf("ACLARACION: El primer nodo ocupado tiene 496 bytes, el segundo 0, el tercero 2 bytes y el cuarto 10 \n");
    printf("Pertenece al programa con PID: %d bytes  \n\n",m->siguiente->siguiente->siguiente->pid);

}




void * nuevoBloqueDeMemoria()
{
	void * memoria = malloc(TAM_BLOQUE);

    dividoMemoria(memoria); //Divido el bloque en paginas de 512 con indicadores de inicio y fin de pagina

	return memoria;
}

void * agregarDatosABloqueDeMemoria(void * bloque, tDato d)
{
	/*Verifico condiciones:: 1 - Existe bloque que puede alojar todo

                             2 - Se puede alojar todo en diferentes bloques

                             3 - No hay suficiente espacio */


    int i = 0;
    int j;
    int tamDatos = d.tamDatos;

    void * datos = d.dato;

    int dirEspacioDisponible;
    int cuantoPuedoCopiar;
    int espacioDisponible;

    int tamMetaAntesModificacion;
    metadata metaData;

    int posMem;
    pNodo nodo;

    int yaCopie=FALSE;
    int yaBorre=FALSE;
    tamDatos = d.tamDatos;
    datos = d.dato;
    while (nodo && tamDatos>0)//avanzo segun la lista de memoria disponible
    {

        nodo = obtenerPrimerNodo(listaMemoriaLibre);
        metaData = obtengoHeapMetadata(bloque,nodo->posMemHeap);
        tamMetaAntesModificacion = metaData.size;
        posMem=nodo->posMemHeap;


        if (puedoAlojarDatos(bloque,tamDatos) && tamDatos>0)
        {

            if(tamDatos<=metaData.size) cuantoPuedoCopiar =tamDatos;
            if(tamDatos>metaData.size) cuantoPuedoCopiar =metaData.size;

            metaData.isFree = FALSE;
            metaData.size = cuantoPuedoCopiar;
            memcpy(nodo->posMemHeap,&metaData,sizeof(metaData));

            memcpy((nodo->posMemHeap)+sizeof(metaData),datos,cuantoPuedoCopiar); //Copio los datos que me llegan



             //Actualizo el metadata de memoria a ocupado y los bytes ocuados
            Insertar(&listaMemoriaOcupada,nodo->posMemHeap,d.pid);
            Borrar(&listaMemoriaLibre,nodo->posMemHeap);



           for (j=0;j<7 && yaBorre==FALSE;j++)//validacion para que no se pise con otros heapMetadatas. siguiente al copiado de datos
            {

                if (existeNodo(&listaMemoriaOcupada,j+posMem+cuantoPuedoCopiar+sizeof(metaData))) //si esxiste algun heap desde donde copio los datos en los proximos 8 bytes no agrega ningun heap ya que pisaria el otro. Queda memoria basura
                {
                    yaBorre=TRUE;
                }
            }

            if (yaBorre==FALSE)
            {
                metaData.isFree=TRUE;
                metaData.size = tamMetaAntesModificacion - cuantoPuedoCopiar - sizeof(metaData);
                memcpy(posMem+sizeof(metaData)+cuantoPuedoCopiar,&metaData,sizeof(metaData));


            Insertar(&listaMemoriaLibre,posMem+sizeof(metaData)+cuantoPuedoCopiar,0);


            yaBorre=TRUE;

            }



            }

        datos=datos + cuantoPuedoCopiar;
        tamDatos=tamDatos - cuantoPuedoCopiar;
        yaCopie=FALSE;
        yaBorre=FALSE;

    }

return bloque;

    }



metadata obtengoHeapMetadata(void * pagina,int posicionDeArranque){
   metadata b;

    void *p = (void *)posicionDeArranque;

    memcpy(&b,p,8);

    return b;
}

void dividoMemoria(void * memoria){

    int i=0;

    while (i<(TAM_BLOQUE/TAM_PAGINA)){

	metadata inicio;
	inicio.size=496;
	inicio.isFree=TRUE;

	memcpy(memoria + (i*TAM_PAGINA),&inicio,sizeof(metadata));
    Insertar(&listaMemoriaLibre,((memoria + (i*TAM_PAGINA))),0);//Agrega a listaDeMemoriaLibrea posiciones de los heaps con memoria libre

	metadata fin;
    fin.size=0;
    fin.isFree=TRUE;
    memcpy(memoria+(((i+1)*TAM_PAGINA)-sizeof(metadata)),&fin,sizeof(metadata));
    Insertar(&listaMemoriaOcupada,memoria+((i+1)*TAM_PAGINA)-sizeof(metadata),0);
    i++;
    }

}

bool puedoAlojarDatosEnUnaPagina(metadata a, int tamDatos){

    return a.isFree && a.size>=tamDatos;

}

bool puedoAlojarDatos(void * bloque, int tamDatos){

    pNodo n = obtenerPrimerNodo(listaMemoriaLibre);
    int sumador=0;
    metadata meta;
    while (n){
        meta = obtengoHeapMetadata(bloque,n->posMemHeap);
        sumador+=meta.size;
        n=n->siguiente;
    }
    if (sumador>=tamDatos+sizeof(meta)) return TRUE;

    return FALSE;
}

tDato creoDato(void * dato,unsigned int tamDatos){
    tDato d;

    d.dato=dato;
    d.tamDatos=tamDatos;

    return d;
}

