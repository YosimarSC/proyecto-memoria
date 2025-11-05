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

// Variables para estadisticas
unsigned long totalReferencias = 0;
unsigned long totalReemplazos = 0;
unsigned long fallosPagina = 0;
int paginasConBitCambiado[20];
int contadorBitsCambiados = 0;

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

char* obtenerPalabra(int indexPalabra){
    return palabrasArchivo[indexPalabra];
}

void cargarPalabrasMemoriaPrincipal(){
    int cont = 0;
    for(int i=0; i<PaginasPrincipal; i++){
        for(int j=0; j<PalabrasPorPagina; j++){
            if(cont % 18 == 0){
                 cont += 342;
            }
            strcpy(memoriaPrincipal[i].palabras[j], obtenerPalabra(cont));
            memoriaPrincipal[i].R = 0;
            memoriaPrincipal[i].M = 0;
            memoriaPrincipal[i].referencias = 0;
            cont++;
        }
    }
}

void cargarPalabrasMemoriaSecundaria(){
    int cont = 0;
    for(int i=0; i<PaginasSecundaria; i++){
        for(int j=0; j<PalabrasPorPagina; j++){
            if(cont % 36 == 0){
                cont += 324;
            }
            strcpy(memoriaSecundaria[i].palabras[j], obtenerPalabra(cont));
            memoriaSecundaria[i].referencias = 0;
            cont++;
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
  
    fprintf(f, "--Estadisticas--:\n");
    fprintf(f, "Total de referencias: %lu\n", totalReferencias);
    fprintf(f, "Total de reemplazos: %lu\n", totalReemplazos);
    fprintf(f, "Total de fallos de pagina: %lu\n", fallosPagina);
    unsigned long totalUsos = 0;
    for (int i = 0; i < PaginasPrincipal; i++)
        totalUsos += memoriaPrincipal[i].referencias;
    fprintf(f, "Promedio de referencias por paginas: %.2f\n",
            (float) totalUsos / PaginasPrincipal);
    fprintf(f, "------------------------------\n\n");
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
    int numPaginaReemplazar = algoritmoClock();

    bool duplicado = true;
    for (int i = 0; i < PalabrasPorPagina; i++) {
        if (strcmp(memoriaPrincipal[numPaginaReemplazar].palabras[i], 
                   memoriaSecundaria[paginaSecundaria].palabras[i]) != 0) {
            duplicado = false;
            break;
        }
    }

    if (duplicado) {
        printf("La página %d ya contiene las mismas palabras, no se reemplaza.\n", numPaginaReemplazar);
        return;
    }

    FILE *resultados = fopen("resultados.txt", "a");
    if (resultados) {
        fprintf(resultados, "Reemplazo: Pagina %d reemplazada por pagina secundaria %d\n", 
                numPaginaReemplazar, paginaSecundaria);
        fprintf(resultados, "Palabras copiadas:\n");
    }

    for(int i = 0; i < PalabrasPorPagina; i++) {
        strcpy(memoriaPrincipal[numPaginaReemplazar].palabras[i], 
               memoriaSecundaria[paginaSecundaria].palabras[i]);
        if (resultados) fprintf(resultados, "%s ", memoriaPrincipal[numPaginaReemplazar].palabras[i]);
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
}

int main(void){
    srand(time(NULL));
    cargarVectorPalabras();
    cargarPalabrasMemoriaPrincipal();
    cargarPalabrasMemoriaSecundaria();
    
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
