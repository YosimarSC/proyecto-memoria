#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define cantidadLetrasMP 20
#define  cantidadLetrasMS 20

// ------variables globales----- 



char memoriaPrincipal[81][6][cantidadLetrasMP];
char memoriaSecundaria [162][6][cantidadLetrasMS];
char palabras[9360][20];
char palabrasCargadas [9360][15];
// ----------declarar metodos y funciones--------
void cargarPalabrasMemoriaPrincipal();
char* obtenerPalabra(int indexPalabra);
void cargarVectorPalabras();
void imprimirMemoriaSecundaria();
void pedirFrase();


struct Letra {
    char letra;
    int cantidad;
};
struct Letra iniciales[26];

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
    //int random=rand()*9360; cargar palabras random

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




void pedirFrase() {
    char frase[200];
    char palabrasFrase[10][20]; 
    int contPalabras = 0;
    int contCol = 0;
   
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf("INGRESE UNA FRASE: ");
    fgets(frase, sizeof(frase), stdin);
    printf("%s", frase);

    for (int i = 0; i < 26; i++) {
        iniciales[i].letra = 'a' + i;
        iniciales[i].cantidad = 0;
    }
    // 
    size_t len = strlen(frase);

    char inicial=' ';
    int leyendoPalabra = 0; // indica si se esta procesando la palabra

    for (int i = 0; i <= len; i++) {
        char c = frase[i];

        // Si no es espacio ni fin de cadena entonces es parte de una palabra
        if (c != ' ' ) {
            if (leyendoPalabra==0) {
                leyendoPalabra = 1; // empieza una nueva palabra
                inicial=c;
                if (contPalabras >= 10) break;
                contCol = 0;

               for (int j = 0; j < 26; j++) {//contar la inicial
                    if (iniciales[j].letra == inicial) {
                        iniciales[j].cantidad++;
                        break;
                    }
                }
            }
             palabrasFrase[contPalabras][contCol++] = c;
          
            
        } 
        // Si hay espacio o ficerramos la palabra
        else {
            if (leyendoPalabra) {
                contPalabras++;
                leyendoPalabra = 0;
            }
        }
    }

    // Validar minimo de palabras
    if (contPalabras < 6) {
        printf("Error: se ingresaron %d palabras el minimo son 6.\n", contPalabras);
        return;
    }

    // Validar que no haya mas de 2 con la misma letra inicial
    int LimiteIniciales = 0;
    for (int i = 0; i < 26; i++) {
        if (iniciales[i].cantidad > 2) {
           LimiteIniciales=1;
            printf("Error: más de 2 palabras con la misma letra inicial: ");
        }
    }

    if (LimiteIniciales==1) {
        printf("\nPor favor ingrese una frase donde ninguna letra inicial se repita mas de 2 veces.\n");
        return;
    }

    // Mostrar las palabras
    printf("\nFrase válida. Palabras extraídas (%d):\n", contPalabras);
    for (int i = 0; i < contPalabras; i++) {
        printf("%s\n", palabrasFrase[i]);
    }
}


int main (void){
    
    cargarVectorPalabras();
    cargarPalabrasMemoriaPrincipal();
    cargarPalabrasMemoriaSecundaria();
    //imprimirMemoriaPrincipal();
    //imprimirMemoriaSecundaria();
    pedirFrase();
}
