#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define cantidadLetrasMP 20
#define  cantidadLetrasMS 20

// ------variables globales----- 



char memoriaPrincipal[81][6][cantidadLetrasMP];
char memoriaSecundaria [162][6][cantidadLetrasMS];
char palabras[9360][20];
// ----------declarar metodos y funciones--------
void cargarPalabrasMemoriaPrincipal();
char* obtenerPalabra(int indexPalabra);
void cargarVectorPalabras();
void imprimirMemoriaSecundaria();



void cargarVectorPalabras(){
   FILE* archivo = fopen("palabras.txt", "r");
    if (!archivo) {
    printf("No se pudo abrir palabras.txt\n");
        exit(1);  // detener programa si archivo no existe
    }

    int cont=0;
    while(cont < 9360){
        fscanf(archivo, "%19s", palabras[cont]);//lee el archivo y lo separa por espacio
        cont ++;
    }
    fclose(archivo);
}
char* obtenerPalabra(int indexPalabra){
    return palabras[indexPalabra];
}
void cargarPalabrasMemoriaPrincipal(){
  
    int cont =0;
    for(int i=0;i<81;i++){
        for(int j=0;j<6;j++){//inicializar matriz 
                            //cada 3 lineas una letra nueva
            if(cont%18==0){
                 cont+=342;
            }
            strncpy(memoriaPrincipal[i][j], obtenerPalabra(cont),cantidadLetrasMP-1);
            memoriaPrincipal[i][j][cantidadLetrasMP - 1] = '\0';
            cont ++;
        }
    }
    
}

void cargarPalabrasMemoriaSecundaria(){
    
    int cont =0;
    for(int i=0;i<162;i++){
        for(int j=0;j<6;j++){//inicializar matriz 
                            //cada 6 lineas una letra nueva
            if(cont%36==0){
                cont+=324;
            }
            strncpy(memoriaSecundaria[i][j], obtenerPalabra(cont),cantidadLetrasMS-1);
            memoriaSecundaria[i][j][cantidadLetrasMS - 1] = '\0';
            cont ++;
        }
    }
}
void imprimirMemoriaPrincipal(){
    for(int i=0;i<81;i++){
        for(int j=0;j<6;j++){
            printf("%s   ",memoriaPrincipal[i][j]);
        }
        printf("\n");
    }
}
void imprimirMemoriaSecundaria(){
    for(int i=0;i<162;i++){
        for(int j=0;j<6;j++){

            printf("%s   ",memoriaSecundaria[i][j]);
        }
        printf("\n");
    }
}
void pedirFrase(){
    char frase[100]; // Buffer para la frase completa
    char palabras[10][20];
    printf("INGRESE UNA FRASE DE MINIMO 6 PALABRAS ");
    scanf("%d", &frase);
}
int main (void){

    cargarVectorPalabras();
    cargarPalabrasMemoriaPrincipal();
    cargarPalabrasMemoriaSecundaria();
    //imprimirMemoriaPrincipal();
    imprimirMemoriaSecundaria();
}
