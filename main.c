#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#define PalabrasPorPagina 6
#define PaginasPrincipal 81
#define PaginasSecundaria 162
#define TotalPalabras 9360

// variables globales
struct Pagina {
    char palabras[PalabrasPorPagina][20];
    int R;  
    int M;  
    unsigned long referencias;
};

struct Pagina memoriaPrincipal[PaginasPrincipal];
struct Pagina memoriaSecundaria[PaginasSecundaria];
char palabrasArchivo[TotalPalabras][20];

// Variables
unsigned long totalReferencias = 0;
unsigned long totalReemplazos = 0;
unsigned long fallosPagina = 0;
int paginasConBitCambiado[20];
int contadorBitsCambiados = 0;
int algoritmoSeleccionado = 0;
static bool usado[TotalPalabras];

struct Letra {
    char letra;
    int cantidad;
};
struct Letra iniciales[26];

//  metodos y funciones
void cargarVectorPalabras();
char* obtenerPalabra(int indexPalabra);
void cargarPalabrasMemoriaPrincipal();
void cargarPalabrasMemoriaSecundaria();
void imprimirMemoriaPrincipal();
void imprimirMemoriaSecundaria();
void pedirFrase();
int buscarPalabraEnPrincipal(char *palabra);
int buscarPalabraEnSecundaria(char *palabra);
int buscarPalabraEnArchivo(char *palabra);
int algoritmoClock();
int algoritmoNRU();
void mostrarEstadisticas();
void guardarEstadisticasEnArchivo();
void cambiarBits();
void cargarPaginaMemoriaPrincipal(int paginaSecundaria);
void menuImpresionMemoria();
void mostrarPaginasMasReferenciadas();
void mostrarPaginasQueCambiaronBit();
int validarSoloLetras(char *frase);
void menuSeleccionAlgoritmo();
void contarPorInicialesArchivo();
void cargarVectorPalabras(){
   FILE* archivo = fopen("palabras.txt", "r");
    if (!archivo) {
    printf("No se pudo abrir palabras.txt\n");
        exit(1);
    }

    int cont=0;
    while(cont < TotalPalabras){
        fscanf(archivo, "%19s", palabrasArchivo[cont]);
        cont ++;
    }
    fclose(archivo);
}
void contarPorInicialesArchivo() {
    int counts[26] = {0};
    for (int i = 0; i < TotalPalabras; ++i) {
        if (palabrasArchivo[i][0] == '\0') continue;
        char c = tolower((unsigned char)palabrasArchivo[i][0]);
        if (c >= 'a' && c <= 'z') counts[c - 'a']++;
    }
    printf("Conteo por inicial en palabrasArchivo (a..z):\n");
    for (int i = 0; i < 26; ++i) {
        printf("%c:%d ", 'a' + i, counts[i]);
        if ((i+1) % 6 == 0) printf("\n");
    }
    printf("\n");
}
char* obtenerPalabra(int indexPalabra){
    return palabrasArchivo[indexPalabra];
}



void cargarPalabrasMemoriaSecundaria() {
    int cont = 0;
    // inicializar usados
    for (int i = 0; i < TotalPalabras; ++i){
        usado[i] = false;
    } 

    for (int i = 0; i < PaginasSecundaria; ++i) {
        memoriaSecundaria[i].referencias = 0;
        for (int j = 0; j < PalabrasPorPagina; ++j) {
            
            if (cont % 36 == 0) {
                cont = (cont + 324) % TotalPalabras;
            }
            
            int intentos = 0;
            while (intentos < TotalPalabras && usado[cont]) {
                cont = (cont + 1) % TotalPalabras;
                intentos++;
            }
           
            strcpy(memoriaSecundaria[i].palabras[j], obtenerPalabra(cont));
            usado[cont] = true;
            cont = (cont + 1) % TotalPalabras;
            
        }
    }
}

void cargarPalabrasMemoriaPrincipal() {
    int cont = 0;
    for (int i = 0; i < PaginasPrincipal; ++i) {
        memoriaPrincipal[i].R = 0;
        memoriaPrincipal[i].M = 0;
        memoriaPrincipal[i].referencias = 0;
        for (int j = 0; j < PalabrasPorPagina; ++j) {
            
            if (cont % 18 == 0) {//saltar a la otra letra
                cont = (cont + 342) % TotalPalabras;
            }
            
            int intentos = 0;
            while (intentos < TotalPalabras && usado[cont]) {
                cont = (cont + 1) % TotalPalabras;
                intentos++;
            }
           
            strcpy(memoriaPrincipal[i].palabras[j], obtenerPalabra(cont));
            usado[cont] = true;
            cont = (cont + 1) % TotalPalabras;
            
        }
    }
}


void imprimirMemoriaPrincipal(){
    for(int i=0;i<81;i++){
        for(int j=0;j<6;j++){
            printf("%s   ",memoriaPrincipal[i].palabras[j]);
        }
        printf("\n");
    }
}
void imprimirMemoriaSecundaria(){
    for(int i=0;i<162;i++){
        for(int j=0;j<6;j++){

            printf("%s   ",memoriaSecundaria[i].palabras[j]);
        }
        printf("\n");
    }
}

int buscarPalabraEnPrincipal(char *palabra) {
    for (int i = 0; i < PaginasPrincipal; i++){
        for (int j = 0; j < PalabrasPorPagina; j++){
            if (strcmp(memoriaPrincipal[i].palabras[j], palabra) == 0) {
                memoriaPrincipal[i].R = 1;
                memoriaPrincipal[i].referencias++;
                totalReferencias++;
                return i;
            }
        }
    }
    return -1;
}

int buscarPalabraEnSecundaria(char *palabra) {

    for (int i = 0; i < PaginasSecundaria; i++){
        for (int j = 0; j < PalabrasPorPagina; j++){
            if (strcmp(memoriaSecundaria[i].palabras[j], palabra) == 0){// si la palabra coincide con alguna de memoria princ{
                memoriaSecundaria[i].referencias++;
                totalReferencias++;
                return i;
            }
        }
    }
    return -1;
}

int buscarPalabraEnArchivo(char *palabra) {
    for (int i = 0; i < TotalPalabras; i++) {
        if (strcmp(palabrasArchivo[i], palabra) == 0) {
            return i;
        }
    }
    return -1;
}

int algoritmoClock() {
    static int puntero = 0;
    int paginaReemplazo = -1;
    
    for (int i = 0; i < PaginasPrincipal * 2 && paginaReemplazo == -1; i++) {
        if (memoriaPrincipal[puntero].R == 0) {
            paginaReemplazo = puntero;
            printf("Reemplazando pagina %d (Clock)\n", puntero);
            totalReemplazos++;
        } else {
            memoriaPrincipal[puntero].R = 0;
        }
        puntero = (puntero + 1) % PaginasPrincipal;
    }
    
    if (paginaReemplazo == -1) {
        printf("No se encontro pagina con R=0\n");
        paginaReemplazo = 0;
    }
    
    return paginaReemplazo;
}
int algoritmoNRU() {
    int paginaReemplazo = -1;
    
    // Buscar en las 4 categorias del NRU 
    //R=0, M=0
    for (int i = 0; i < PaginasPrincipal; i++) {
        if (memoriaPrincipal[i].R == 0 && memoriaPrincipal[i].M == 0) { 
            paginaReemplazo = i; 
            break; 
        }
    }
    
    //R=0 M=1
    if (paginaReemplazo == -1) {
        for (int i = 0; i < PaginasPrincipal; i++) {
            if (memoriaPrincipal[i].R == 0 && memoriaPrincipal[i].M == 1) { 
                paginaReemplazo = i; 
                break; 
            }
        }
    }
    
    //R=1 M=0
    if (paginaReemplazo == -1) {
        for (int i = 0; i < PaginasPrincipal; i++) {
            if (memoriaPrincipal[i].R == 1 && memoriaPrincipal[i].M == 0) { 
                paginaReemplazo = i; 
                break; 
            }
        }
    }
    
    //R=1 M=1 
    if (paginaReemplazo == -1) {
        for (int i = 0; i < PaginasPrincipal; i++) {
            if (memoriaPrincipal[i].R == 1 && memoriaPrincipal[i].M == 1) { 
                paginaReemplazo = i; 
                break; 
            }
        }
    }
    
    // Si no encontro ninguna se usa la primera 
    if (paginaReemplazo == -1) {
        paginaReemplazo = 0;
        printf("No se encontro pagina candidata usando la pagina %d\n", paginaReemplazo);
    } else {
        printf("Reemplazando pagina %d \n", paginaReemplazo);
    }
    
    totalReemplazos++;
    return paginaReemplazo;  //return numero de pagina
}
void mostrarEstadisticas() {
    printf("\nESTADISTICAS \n");
    printf("Total de referencias: %lu\n", totalReferencias);
    printf("Total de reemplazos: %lu\n", totalReemplazos);
    printf("Total de fallos de pagina: %lu\n", fallosPagina);
    unsigned long totalUsos = 0;
    for (int i = 0; i < PaginasPrincipal; i++)
        totalUsos += memoriaPrincipal[i].referencias;
    printf("Promedio de referencias por pagina: %.2f\n",
           (float) totalUsos / PaginasPrincipal);
}

void guardarEstadisticasEnArchivo() {
    FILE *f = fopen("resultados.txt", "a");
  
    fprintf(f, "Estadisticas:\n");
    fprintf(f, "Total de referencias: %lu\n", totalReferencias);
    fprintf(f, "Total de reemplazos: %lu\n", totalReemplazos);
    fprintf(f, "Total de fallos de pagina: %lu\n", fallosPagina);
    unsigned long totalUsos = 0;
    for (int i = 0; i < PaginasPrincipal; i++)
        totalUsos += memoriaPrincipal[i].referencias;
    fprintf(f, "Promedio de referencias por paginas: %.2f\n",
            (float) totalUsos / PaginasPrincipal);
   
    fclose(f);
    printf("Estadisticas guardadas en resultados.txt\n");
}

int validarSoloLetras(char *frase) {
    for (int i = 0; frase[i] != '\0'; i++) {
        if (!isalpha(frase[i]) && frase[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

void cambiarBits() {
    contadorBitsCambiados = 0;
    printf("Modificando bits R de 10 paginas aleatorias:\n");
    for(int i = 0; i < 10; i++) {
        int NumeroPag = rand() % PaginasPrincipal;
        memoriaPrincipal[NumeroPag].R = !memoriaPrincipal[NumeroPag].R;
        paginasConBitCambiado[contadorBitsCambiados++] = NumeroPag;
        printf("pagina %d: R cambio a %d\n", NumeroPag, memoriaPrincipal[NumeroPag].R);
    }
    
    printf("Modificando bits M de 10 paginas aleatorias:\n");
    for(int i = 0; i < 10; i++) {
        int NumeroPag = rand() % PaginasPrincipal;
        memoriaPrincipal[NumeroPag].M = !memoriaPrincipal[NumeroPag].M;
        paginasConBitCambiado[contadorBitsCambiados++] = NumeroPag;
        printf("pagina %d: M cambio a %d\n", NumeroPag, memoriaPrincipal[NumeroPag].M);
    }
}


void cargarPaginaMemoriaPrincipal(int paginaSecundaria) {
    int numPaginaReemplazar = 0;
    if(algoritmoSeleccionado==0){
        numPaginaReemplazar=algoritmoClock();       
    }else{
        numPaginaReemplazar=algoritmoNRU();
    }


    FILE *resultados = fopen("resultados.txt", "a");
    
    fprintf(resultados, "Reemplazo: Pagina %d reemplazada por pagina secundaria %d\n", numPaginaReemplazar, paginaSecundaria);
    fprintf(resultados, "Palabras copiadas:\n");

    for(int i = 0; i < PalabrasPorPagina; i++) {
        strcpy(memoriaPrincipal[numPaginaReemplazar].palabras[i],memoriaSecundaria[paginaSecundaria].palabras[i]);

        fprintf(resultados, "%s ", memoriaPrincipal[numPaginaReemplazar].palabras[i]);
        
    }

    memoriaPrincipal[numPaginaReemplazar].R = 1;
    memoriaPrincipal[numPaginaReemplazar].M = 0;
    memoriaPrincipal[numPaginaReemplazar].referencias = 0;

    
    printf("pagina %d de secundaria cargada en posicion %d de memoria principal\n", paginaSecundaria, numPaginaReemplazar);
}

void pedirFrase() {
    char frase[200];
    char palabrasFrase[10][20]; 
    int contPalabras = 0;
    int contCol = 0;
   

    printf("INGRESE UNA FRASE: ");
    fgets(frase, sizeof(frase), stdin);
    
    size_t len = strlen(frase);
    if (len > 0 && frase[len-1] == '\n') {
        frase[len-1] = '\0';
        len--;
    }
    
    if (!validarSoloLetras(frase)) {
        printf("Error: La frase solo puede contener letras y espacios.\n");
        return;
    }
    
    for (int i = 0; i < 26; i++) {
        iniciales[i].letra = 'a' + i;
        iniciales[i].cantidad = 0;
    }
    
    char inicial=' ';
    int leyendoPalabra = 0; // indica si se esta procesando la palabra

    for (int i = 0; i <= len; i++) {
        char c = frase[i];

        if (c != ' ' && c != '\0') {
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
            if (contCol < 19) {
                palabrasFrase[contPalabras][contCol++] = c;
            }
        } else {
            if (leyendoPalabra) {
                palabrasFrase[contPalabras][contCol] = '\0';
                
                int encontrada = 0;
                int paginaPrincipal = buscarPalabraEnPrincipal(palabrasFrase[contPalabras]);
                
                if (paginaPrincipal != -1) {
                    encontrada = 1;
                    printf("Palabra '%s' encontrada en principal pagina %d\n", 
                           palabrasFrase[contPalabras], paginaPrincipal);
                } else {
                    fallosPagina++; 
                    int paginaSecundaria = buscarPalabraEnSecundaria(palabrasFrase[contPalabras]);
                    if (paginaSecundaria != -1) {
                        encontrada = 1;
                        printf("Palabra '%s' encontrada en secundaria pagina %d se cargara en principal \n", 
                               palabrasFrase[contPalabras], paginaSecundaria);
                        cargarPaginaMemoriaPrincipal(paginaSecundaria);
                    } else {
                        int posArchivo = buscarPalabraEnArchivo(palabrasFrase[contPalabras]);
                        if (posArchivo != -1) {
                            encontrada = 1;
                            printf("Palabra '%s' encontrada en archivo\n", palabrasFrase[contPalabras]);
                        } else {
                            printf("Palabra no encontrada: %s\n", palabrasFrase[contPalabras]);
                        }
                    }
                }
                
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

    int LimiteIniciales = 0;
    for (int i = 0; i < 26; i++) {
        if (iniciales[i].cantidad > 2) {
            LimiteIniciales = 1;
            printf("Error: mas de 2 palabras con la letra inicial '%c'\n", iniciales[i].letra);
        }
    }

    if (LimiteIniciales == 1) {
        printf("Por favor ingrese una frase donde ninguna letra inicial se repita mas de 2 veces.\n");
        return;
    }
    
    printf("\nMODIFICACION DE BITS\n");
    cambiarBits();
    
    /*for (int i = 0; i < contPalabras; i++) { para imprimir las palabras de la frase
        printf("%s\n", palabrasFrase[i]);
    }*/
    
    mostrarEstadisticas();
    mostrarPaginasMasReferenciadas();
    mostrarPaginasQueCambiaronBit();
    menuImpresionMemoria();
}
void menuSeleccionAlgoritmo() {
    int opcion=0;
    int c;
    printf("INGRESE EL ALGORITMO QUE DESEA IMPLEMENTAR (0-CLOCK) (1- NRU)");

    scanf("%d", &opcion);
    while ((c = getchar()) != '\n' && c != EOF);

    if(opcion==0){
        algoritmoSeleccionado=0;
    }else{
         algoritmoSeleccionado=1;
    }

}
void mostrarPaginasMasReferenciadas(){
    // En lugar de copiar las páginas, copiar ÍNDICES
    int indicesPrincipal[PaginasPrincipal];
    int indicesSecundaria[PaginasSecundaria];
    
    // Inicializar índices
    for(int i = 0; i < PaginasPrincipal; i++) {
        indicesPrincipal[i] = i;  // guardar los indices originales 
    }
    for(int i = 0; i < PaginasSecundaria; i++) {
        indicesSecundaria[i] = i; 
    }
    // burble sort
    for(int i = 0; i < PaginasPrincipal - 1; i++) {
        for(int j = 0; j < PaginasPrincipal - i - 1; j++) {
            if(memoriaPrincipal[indicesPrincipal[j]].referencias < 
               memoriaPrincipal[indicesPrincipal[j+1]].referencias) {
        
                int temp = indicesPrincipal[j];
                indicesPrincipal[j] = indicesPrincipal[j+1];
                indicesPrincipal[j+1] = temp;
            }
        }
    }
    
    // Hacer lo mismo para secundaria
    for(int i = 0; i < PaginasSecundaria - 1; i++) {
        for(int j = 0; j < PaginasSecundaria - i - 1; j++) {
            if(memoriaSecundaria[indicesSecundaria[j]].referencias < 
               memoriaSecundaria[indicesSecundaria[j+1]].referencias) {
        
                int temp = indicesSecundaria[j];
                indicesSecundaria[j] = indicesSecundaria[j+1];
                indicesSecundaria[j+1] = temp;
            }
        }
    }
    
    printf("\n 10 PAGINAS MAS REFERENCIADAS \n");
    
    printf("\nMEMORIA PRINCIPAL:\n");
    for(int i = 0; i < 10 && i < PaginasPrincipal; i++) {
        int numPagina = indicesPrincipal[i];
        printf("%d pagina %d - %lu referencias\n", 
               i+1, numPagina, memoriaPrincipal[numPagina].referencias);
    }
    
    printf("\nMEMORIA SECUNDARIA:\n");
    for(int i = 0; i < 10 && i < PaginasSecundaria; i++) {
        int numPagina = indicesSecundaria[i];
        printf("%d- pagina %d - %lu referencias\n", i+1, numPagina, memoriaSecundaria[numPagina].referencias);
    }
}
void mostrarPaginasQueCambiaronBit() {
    printf("\n PAGINAS QUE CAMBIARON BIT \n");
    if (contadorBitsCambiados == 0) {
        printf("No hubo cambios de bits en esta iteracion.\n");
        return;
    }
    
    for (int i = 0; i < contadorBitsCambiados; i++) {
        int numPagina = paginasConBitCambiado[i];
        printf("pagina %d - R=%d, M=%d\n", numPagina, memoriaPrincipal[numPagina].R, memoriaPrincipal[numPagina].M);
    }
}

void menuImpresionMemoria() {
    int opcion;
    printf("\n¿Que memoria desea imprimir?\n");
    printf("1. Memoria Principal\n");
    printf("2. Memoria Secundaria\n");
    printf("Opcion: ");
   
    scanf("%d", &opcion);
    
    
    if(opcion == 1) {
        imprimirMemoriaPrincipal();
    } else if(opcion == 2) {
        imprimirMemoriaSecundaria();
    }
}
int main(void){
    for(int i=0;i<TotalPalabras;i++){//inicializar vector de usados
        usado[i]=false;
    }
    srand(time(NULL));
    cargarVectorPalabras();
    cargarPalabrasMemoriaSecundaria();
    cargarPalabrasMemoriaPrincipal();
     contarPorInicialesArchivo();
    menuSeleccionAlgoritmo();
    while(1) {

        pedirFrase();
        
        printf("\n¿Desea ingresar otra frase? s - n: ");
        char respuesta[10];
        fgets(respuesta, sizeof(respuesta), stdin);
        if (respuesta[0] != 's' && respuesta[0] != 'S') {
            break;
        }
    }
   
    guardarEstadisticasEnArchivo();
    return 0;
}
