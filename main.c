#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Constantes
#define PALABRAS_POR_PAGINA 6
#define PAGINAS_PRINCIPAL 81
#define PAGINAS_SECUNDARIA 162
#define TOTAL_PALABRAS 9360

// ------variables globales----- 
struct Pagina {
    char palabras[PALABRAS_POR_PAGINA][20];
    int R;  // Bit de referencia
    int M;  // Bit de modificación
    unsigned long referencias;
};

struct Pagina memoriaPrincipal[PAGINAS_PRINCIPAL];
struct Pagina memoriaSecundaria[PAGINAS_SECUNDARIA];
char palabras[TOTAL_PALABRAS][20];

// Variables para estadísticas
unsigned long totalReferencias = 0;
unsigned long totalReemplazos = 0;
unsigned long fallosPagina = 0;

// ----------declarar metodos y funciones--------
void cargarVectorPalabras();
char* obtenerPalabra(int indexPalabra);
void cargarPalabrasMemoriaPrincipal();
void cargarPalabrasMemoriaSecundaria();
void imprimirMemoriaPrincipal();
void imprimirMemoriaSecundaria();
void pedirFrase();
int buscarPalabraEnPrincipal(char *palabra);
int buscarPalabraEnSecundaria(char *palabra);
void algoritmoClock();
void algoritmoNRU();
void mostrarEstadisticas();
void guardarEstadisticasEnArchivo();

struct Letra {
    char letra;
    int cantidad;
};
struct Letra iniciales[26];

void cargarVectorPalabras(){
   FILE* archivo = fopen("palabras.txt", "r");
    if (!archivo) {
    printf("No se pudo abrir palabras.txt\n");
        exit(1);
    }

    int cont=0;
    while(cont < TOTAL_PALABRAS){
        fscanf(archivo, "%19s", palabras[cont]);
        cont ++;
    }
    fclose(archivo);
}

char* obtenerPalabra(int indexPalabra){
    return palabras[indexPalabra];
}

void cargarPalabrasMemoriaPrincipal(){
    int cont = 0;
    for(int i=0; i<PAGINAS_PRINCIPAL; i++){
        for(int j=0; j<PALABRAS_POR_PAGINA; j++){
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
    for(int i=0; i<PAGINAS_SECUNDARIA; i++){
        for(int j=0; j<PALABRAS_POR_PAGINA; j++){
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
    for(int i=0; i<PAGINAS_PRINCIPAL; i++){
        printf("Pagina %d (R=%d, M=%d): ", i, memoriaPrincipal[i].R, memoriaPrincipal[i].M);
        for(int j=0; j<PALABRAS_POR_PAGINA; j++){
            printf("%s   ", memoriaPrincipal[i].palabras[j]);
        }
        printf("\n");
    }
}

void imprimirMemoriaSecundaria(){
    for(int i=0; i<PAGINAS_SECUNDARIA; i++){
        printf("Pagina %d: ", i);
        for(int j=0; j<PALABRAS_POR_PAGINA; j++){
            printf("%s   ", memoriaSecundaria[i].palabras[j]);
        }
        printf("\n");
    }
}

// Funciones de tu compañero adaptadas
int buscarPalabraEnPrincipal(char *palabra) {
    for (int i = 0; i < PAGINAS_PRINCIPAL; i++)
        for (int j = 0; j < PALABRAS_POR_PAGINA; j++)
            if (strcmp(memoriaPrincipal[i].palabras[j], palabra) == 0) {
                memoriaPrincipal[i].R = 1;
                memoriaPrincipal[i].referencias++;
                totalReferencias++;
                return 1;
            }
    return 0;
}

int buscarPalabraEnSecundaria(char *palabra) {
    for (int i = 0; i < PAGINAS_SECUNDARIA; i++)
        for (int j = 0; j < PALABRAS_POR_PAGINA; j++)
            if (strcmp(memoriaSecundaria[i].palabras[j], palabra) == 0) {
                memoriaSecundaria[i].referencias++;
                totalReferencias++;
                return 1;
            }
    return 0;
}

void algoritmoClock() {
    static int puntero = 0;
    int reemplazada = 0;
    for (int i = 0; i < PAGINAS_PRINCIPAL * 2 && !reemplazada; i++) {
        if (memoriaPrincipal[puntero].R == 0) {
            printf("Reemplazando página %d (Clock)\n", puntero);
            memoriaPrincipal[puntero].M = 0;
            memoriaPrincipal[puntero].R = 0;
            totalReemplazos++;
            reemplazada = 1;
        } else {
            memoriaPrincipal[puntero].R = 0;
        }
        puntero = (puntero + 1) % PAGINAS_PRINCIPAL;
    }
    if (!reemplazada)
        printf("No se encontró página con R=0\n");
}

void algoritmoNRU() {
    int candidato = -1;
    for (int i = 0; i < PAGINAS_PRINCIPAL; i++) {
        if (memoriaPrincipal[i].R == 0 && memoriaPrincipal[i].M == 0) { 
            candidato = i; 
            break; 
        }
        else if (memoriaPrincipal[i].R == 0 && candidato == -1) 
            candidato = i;
    }
    if (candidato != -1) {
        printf("Reemplazando página %d (NRU)\n", candidato);
        memoriaPrincipal[candidato].M = 0;
        memoriaPrincipal[candidato].R = 0;
        totalReemplazos++;
    } else {
        printf("No se encontró página candidata.\n");
    }
}

void mostrarEstadisticas() {
    printf("\n=== ESTADISTICAS DEL SISTEMA ===\n");
    printf("Total de referencias: %lu\n", totalReferencias);
    printf("Total de reemplazos: %lu\n", totalReemplazos);
    printf("Total de fallos de página: %lu\n", fallosPagina);
    unsigned long totalUsos = 0;
    for (int i = 0; i < PAGINAS_PRINCIPAL; i++)
        totalUsos += memoriaPrincipal[i].referencias;
    printf("Promedio de referencias por página: %.2f\n",
           (float) totalUsos / PAGINAS_PRINCIPAL);
}

void guardarEstadisticasEnArchivo() {
    FILE *f = fopen("resultados.txt", "a");
    if (!f) {
        printf("No se pudo crear resultados.txt\n");
        return;
    }
    fprintf(f, "------------------------------\n");
    fprintf(f, "Estadísticas de ejecución:\n");
    fprintf(f, "Total de referencias: %lu\n", totalReferencias);
    fprintf(f, "Total de reemplazos: %lu\n", totalReemplazos);
    fprintf(f, "Total de fallos de página: %lu\n", fallosPagina);
    unsigned long totalUsos = 0;
    for (int i = 0; i < PAGINAS_PRINCIPAL; i++)
        totalUsos += memoriaPrincipal[i].referencias;
    fprintf(f, "Promedio de referencias por página: %.2f\n",
            (float) totalUsos / PAGINAS_PRINCIPAL);
    fprintf(f, "------------------------------\n\n");
    fclose(f);
    printf("Estadísticas guardadas en resultados.txt\n");
}

// Tu función pedirFrase modificada para usar las nuevas funciones
void pedirFrase() {
    char frase[200];
    char palabrasFrase[10][20]; 
    int contPalabras = 0;
    int contCol = 0;
   
    printf("INGRESE UNA FRASE: ");
    fgets(frase, sizeof(frase), stdin);
    
    // Eliminar salto de línea
    size_t len = strlen(frase);
    if (len > 0 && frase[len-1] == '\n') {
        frase[len-1] = '\0';
    }
    
    printf("Frase ingresada: %s\n", frase);

    // Reinicializar contadores
    for (int i = 0; i < 26; i++) {
        iniciales[i].letra = 'a' + i;
        iniciales[i].cantidad = 0;
    }
    
    len = strlen(frase);
    char inicial = ' ';
    int leyendoPalabra = 0;

    for (int i = 0; i <= len; i++) {
        char c = frase[i];

        if (c != ' ' && c != '\0') {
            if (leyendoPalabra == 0) {
                leyendoPalabra = 1;
                inicial = c;
                if (contPalabras >= 10) break;
                contCol = 0;

                for (int j = 0; j < 26; j++) {
                    if (iniciales[j].letra == inicial) {
                        iniciales[j].cantidad++;
                        break;
                    }
                }
            }
            palabrasFrase[contPalabras][contCol++] = c;
        } 
        else {
            if (leyendoPalabra) {
                palabrasFrase[contPalabras][contCol] = '\0';
                
                // BUSCAR PALABRA EN MEMORIA
                if (!buscarPalabraEnPrincipal(palabrasFrase[contPalabras])) {
                    if (!buscarPalabraEnSecundaria(palabrasFrase[contPalabras])) {
                        printf("Palabra no encontrada: %s\n", palabrasFrase[contPalabras]);
                        fallosPagina++;
                    }
                }
                
                contPalabras++;
                leyendoPalabra = 0;
            }
        }
    }

    // Validaciones
    if (contPalabras < 6) {
        printf("Error: se ingresaron %d palabras. El mínimo son 6.\n", contPalabras);
        return;
    }

    int LimiteIniciales = 0;
    for (int i = 0; i < 26; i++) {
        if (iniciales[i].cantidad > 2) {
            LimiteIniciales = 1;
            printf("Error: más de 2 palabras con la letra inicial '%c'\n", iniciales[i].letra);
        }
    }

    if (LimiteIniciales == 1) {
        printf("Por favor ingrese una frase donde ninguna letra inicial se repita más de 2 veces.\n");
        return;
    }

    printf("\nFrase válida (%d palabras):\n", contPalabras);
    for (int i = 0; i < contPalabras; i++) {
        printf("%s\n", palabrasFrase[i]);
    }
    
    // Ejecutar algoritmo de reemplazo (puedes elegir Clock o NRU)
    algoritmoClock();
    // algoritmoNRU();
    
    mostrarEstadisticas();
}

int main(void){
    printf("Iniciando sistema de memoria...\n");
    cargarVectorPalabras();
    cargarPalabrasMemoriaPrincipal();
    cargarPalabrasMemoriaSecundaria();
    
    // Bucle principal para múltiples frases
    while(1) {
        pedirFrase();
        
        printf("\n¿Desea ingresar otra frase? (s/n): ");
        char respuesta[10];
        fgets(respuesta, sizeof(respuesta), stdin);
        if (respuesta[0] != 's' && respuesta[0] != 'S') {
            break;
        }
    }
    
    guardarEstadisticasEnArchivo();
    return 0;
}