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

//Estos define que defino ahora luego se obtienen del config file
#define MARCOS 4
#define MARCO_SIZE 512



Lista listaMemoriaLibre = NULL;
pNodo p;

Lista listaMemoriaOcupada = NULL;
pNodo q;

void * memoria; //MARCOS * MARCOS_SIZE



struct heapMetadata{
    uint32_t size;
	bool isFree;
};

//Creo esta estructura para guardar los datos con su tamanio por que si no no se puede saber con un sizeof por ser void
struct datosStruct{
    void * dato;
    unsigned int tamDatos;
    int pid;

};

struct estructuraPaginacionInversa{
    int ** matriz;
    int filas;
    int columnas;

};
typedef struct estructuraPaginacionInversa tEstructuraPaginacionInversa;

tEstructuraPaginacionInversa EPI;

typedef struct heapMetadata metadata;

typedef struct datosStruct tDato;



void * nuevoBloqueDeMemoria();
metadata obtengoHeapMetadata(void * pagina,int posicionDeArranque);
void dividoMemoria(void * memoria);
bool puedoAlojarDatosEnUnaPagina(metadata memoria, int tamDatos);
void * agregarDatosABloqueDeMemoria(void * memoria, tDato datos);
tDato creoDato(void * dato,unsigned int tamDatos);
bool puedoAlojarDatos(void * memoria, int tamDatos);
bool consola();
tDato obtenerMemoria(void * memoria,int PID);
tDato obtenerMemoriaReducida(void * memoria,int PID,int posArranque,int desplazamiento);


int main(void)
{

    memoria = nuevoBloqueDeMemoria();



    pNodo n;

    tDato datito;
    tDato dato;

    void * datos0=malloc(498);
    datito.dato=datos0;
    datito.tamDatos=498;
    datito.pid=15;

    void * datos1="Hola";
    dato.dato=datos1;
    dato.tamDatos=5;
    dato.pid=3;

n= obtenerPrimerNodo(listaMemoriaLibre);
metadata metaData;

    metaData = obtengoHeapMetadata(memoria,n->posMemHeap);
    printf("Memoria libre en primer nodo libre: %d bytes \n\n",metaData.size);

    printf("memoria libre antes del copiado ");
    MostrarLista(listaMemoriaLibre,ASCENDENTE);
    printf("memoria ocupada antes del copiado ");
    MostrarLista(listaMemoriaOcupada,ASCENDENTE);





    memoria = agregarDatosABloqueDeMemoria(memoria,datito);
    memoria = agregarDatosABloqueDeMemoria(memoria,dato);
n= obtenerPrimerNodo(listaMemoriaLibre);
      metaData = obtengoHeapMetadata(memoria,n->posMemHeap);
    printf("Memoria libre en primer nodo libre luego de un copiado: %d bytes \n\n",metaData.size);
    n= obtenerPrimerNodo(listaMemoriaOcupada->siguiente);
      metaData = obtengoHeapMetadata(memoria,n->posMemHeap);
printf("memoria ocupada del primer nodo despue del primer copiado: %d \n",metaData.size);








    printf("memoria ocupada despues del copiado ");
    MostrarLista(listaMemoriaOcupada,ASCENDENTE);


    printf("memoria libre despues del copiado ");
    MostrarLista(listaMemoriaLibre,ASCENDENTE);

    pNodo m= obtenerPrimerNodo(listaMemoriaLibre);
    metaData = obtengoHeapMetadata(memoria,m->posMemHeap);
    printf("El primer nodo libre luego de la copia es: %d \n",m->posMemHeap);

 printf("\nMemoria libre en primer nodo libre despues de un copiado: %d bytes \n\n",metaData.size);

    m= obtenerPrimerNodo(listaMemoriaOcupada);
    metaData = obtengoHeapMetadata(memoria,m->siguiente->siguiente->posMemHeap);
    printf("\nMemoria Ocupada en tercer nodo ocupado despues de un copiado: %d bytes  \n",metaData.size);
    printf("ACLARACION: El primer nodo ocupado tiene 496 bytes, el segundo 0 y el tercero 2 bytes \n");
    printf("Pertenece al programa con PID: %d bytes  \n\n",m->siguiente->siguiente->pid);

    m= obtenerPrimerNodo(listaMemoriaOcupada);
    metaData = obtengoHeapMetadata(memoria,m->siguiente->siguiente->siguiente->posMemHeap);
    printf("\nMemoria Ocupada en cuarto nodo ocupado despues de un segundo copiado: %d bytes  \n",metaData.size);
    printf("ACLARACION: El primer nodo ocupado tiene 496 bytes, el segundo 0, el tercero 2 bytes y el cuarto 10 \n");
    printf("Pertenece al programa con PID: %d bytes  \n\n",m->siguiente->siguiente->siguiente->pid);

    int temp=0;
    for (temp;temp<EPI.filas;temp++){
     printf("EPI nFrame PID nPagina PosMem: %d,%d,%d,%d \n\n",EPI.matriz[temp][0],EPI.matriz[temp][1],EPI.matriz[temp][2],EPI.matriz[temp][3]);
    }
     //,EPI.matriz[0][1],EPI.matriz[0][2]

     dato=obtenerMemoria(memoria,3);
     printf("OBTENGO EL TAM PROGRAMA PARA PID 3 %d \n",dato.tamDatos);
    printf("OBTENGO EL CONTENIDO PROGRAMA PARA PID 3 %s \n",dato.dato);


    char * cadena= "2356";

    int a = convertirCharAInt(cadena,4);

    printf("El numero es %d \n",a);
}


tDato obtenerMemoria(void * memoria,int PID){
    int i=0;
    void *programa;
    metadata heap;
    int tamPrograma=0;
    int tamProgramaAnterior=0;
    tDato dato;
    for(i;i<EPI.filas;i++){
        if (EPI.matriz[i][1]==PID){

            heap = obtengoHeapMetadata(memoria,EPI.matriz[i][3]);

            tamProgramaAnterior=tamPrograma;
            tamPrograma+=heap.size;
            programa = (void*) realloc(programa,tamPrograma*sizeof(void*)*1);
            if(programa!='\0'){
           memmove((void *)(programa+tamProgramaAnterior),(void *)(EPI.matriz[i][3]+sizeof(heap)),heap.size);

            }
        }
    }
    dato.dato=programa; // aca hay que hacer un memcpy
    dato.tamDatos=tamPrograma;
    return dato;


}
tDato obtenerMemoriaReducida(void * memoria,int PID,int posArranque,int desplazamiento){
    int i=0;
    void *programa;
    metadata heap;

    tDato dato;
    for(i;i<EPI.filas;i++){
        if (EPI.matriz[i][1]==PID){

            heap = obtengoHeapMetadata(memoria,EPI.matriz[i][3]);



            programa = calloc(programa,desplazamiento*sizeof(void*));
            if(programa!='\0'){
           memmove((void *)(programa),(void *)(EPI.matriz[i][3]+sizeof(heap)+posArranque),desplazamiento);

            }
        }
    }
    dato.dato=programa; // aca hay que hacer un memcpy
    dato.tamDatos=desplazamiento;
    return dato;


}
void * nuevoBloqueDeMemoria()//Inicializo memora
{
	void * memoria = malloc(MARCOS*MARCO_SIZE);

    dividoMemoria(memoria); //Divido el bloque en paginas de 512 con indicadores de inicio y fin de pagina

    inicializoEPI();

	return memoria;
}

void * agregarDatosABloqueDeMemoria(void * memoria, tDato d)
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
        metaData = obtengoHeapMetadata(memoria,nodo->posMemHeap);
        tamMetaAntesModificacion = metaData.size;
        posMem=nodo->posMemHeap;


        if (puedoAlojarDatos(memoria,tamDatos) && tamDatos>0)
        {

            if(tamDatos<=metaData.size) cuantoPuedoCopiar =tamDatos;
            if(tamDatos>metaData.size) cuantoPuedoCopiar =metaData.size;

            metaData.isFree = FALSE;
            metaData.size = cuantoPuedoCopiar;
            memcpy(nodo->posMemHeap,&metaData,sizeof(metaData));

            memcpy((nodo->posMemHeap)+sizeof(metaData),datos,cuantoPuedoCopiar); //Copio los datos que me llegan

          int  nPagina=(((nodo->posMemHeap)+sizeof(metaData)- memoria)/512);
          if (nPagina==-1) nPagina+=1;


            agregaDatoEPI(d.pid,nPagina,nodo);

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

return memoria;

    }




metadata obtengoHeapMetadata(void * pagina,int posicionDeArranque){
   metadata b;

    void *p = (void *)posicionDeArranque;

    memcpy(&b,p,8);

    return b;
}


void dividoMemoria(void * memoria){

    int i=0;

    while (i<MARCOS){

	metadata inicio;
	inicio.size=MARCO_SIZE-(sizeof(metadata)*2);
	inicio.isFree=TRUE;

	memcpy(memoria + (i*MARCO_SIZE),&inicio,sizeof(metadata));
    Insertar(&listaMemoriaLibre,((memoria + (i*MARCO_SIZE))),0);//Agrega a listaDeMemoriaLibrea posiciones de los heaps con memoria libre

	metadata fin;
    fin.size=0;
    fin.isFree=TRUE;
    memcpy(memoria+(((i+1)*MARCO_SIZE)-sizeof(metadata)),&fin,sizeof(metadata));
    Insertar(&listaMemoriaOcupada,memoria+((i+1)*MARCO_SIZE)-sizeof(metadata),0);
    i++;
    }

}

bool puedoAlojarDatosEnUnaPagina(metadata memoria, int tamDatos){

    return memoria.isFree && memoria.size>=tamDatos;

}

bool puedoAlojarDatos(void * memoria, int tamDatos){

    pNodo n = obtenerPrimerNodo(listaMemoriaLibre);
    int sumador=0;
    metadata meta;
    while (n){
        meta = obtengoHeapMetadata(memoria,n->posMemHeap);
        sumador+=meta.size;
        n=n->siguiente;
    }
    if (sumador>=tamDatos+sizeof(meta)) return TRUE;

    return FALSE;
}
void inicializoEPI(){

EPI.filas=0;
EPI.columnas=3;


}
void agregaDatoEPI(int PID,int nPagina,pNodo nodo){

    int *aux;
    aux = (int**)realloc(EPI.matriz,sizeof(int)*(EPI.filas+1));

    if (aux!='\0'){
    //memcpy(EPI.matriz,&aux,sizeof(int)*(EPI.filas+1));
    aux[EPI.filas] =(int**) malloc(sizeof(int)*4);
    EPI.matriz=aux;
    memcpy(&EPI.matriz[EPI.filas][0],&EPI.filas,sizeof(EPI.filas));

    memcpy(&EPI.matriz[EPI.filas][1],&PID,sizeof(PID));
    memcpy(&EPI.matriz[EPI.filas][2],&nPagina,sizeof(nPagina));
    memcpy(&EPI.matriz[EPI.filas][3],&nodo->posMemHeap,sizeof(nodo->posMemHeap));

    EPI.filas+=1;
    }
}


tDato creoDato(void * dato,unsigned int tamDatos){
    tDato d;

    d.dato=dato;
    d.tamDatos=tamDatos;

    return d;
}

void dump(){
    BorrarLista(&listaMemoriaLibre);
    BorrarLista(&listaMemoriaOcupada);
    free(memoria);
}

bool consola(){
int salir = 0;
while (!salir) {
		//Mensaje
		char* mensaje = (char *) malloc(sizeof(char) * 1000);
		printf("> ");
		scanf("%[^\n]s", mensaje);
		char* comando = strtok(mensaje, " ");
		char* argumento = strtok(NULL, " ");

		 if (!strcmp(comando, "exit"))
			salir = 1;
		else if (!strcmp(comando, "clean"))
			system("clear");
        else if (!strcmp(comando, "dump")){
             dump();
			}
		else
			printf("Comando incorrecto. Pruebe run | stop | exit | clean\n");
		free(mensaje);
		limpiarBufferDeEntrada();
	}
}
void limpiarBufferDeEntrada() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}

int convertirCharAInt(char * numero,int tamChar){
		int i;
		int multiplicadorBase = 1;
		int numeroInt=0;
		tamChar -= 1;
		while (tamChar>=0)
		{
			numeroInt+= (numero[tamChar] - '0')*multiplicadorBase;
			tamChar--;
			multiplicadorBase*=10;
		}
		return numeroInt;
	}
