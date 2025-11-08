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

// Estructura de una página
struct Pagina {
    char palabras[PalabrasPorPagina][20];
    int R;
    int M;
    unsigned long referencias;
};

// Memorias
struct Pagina memoriaPrincipal[PaginasPrincipal];
struct Pagina memoriaSecundaria[PaginasSecundaria];
char palabrasArchivo[TotalPalabras][20];

// Variables globales
unsigned long totalReferencias = 0;
unsigned long totalReemplazos = 0;
unsigned long fallosPagina = 0;
int paginasConBitCambiado[20];
int contadorBitsCambiados = 0;
int algoritmoSeleccionado = 0;
static bool usado[TotalPalabras];

FILE *salida;

// Prototipos
void cargarVectorPalabras();
void cargarPalabrasMemoriaPrincipal();
void cargarPalabrasMemoriaSecundaria();
void imprimirMemoriaPrincipal();
void imprimirMemoriaSecundaria();
int buscarPalabraEnPrincipal(char *palabra);
int buscarPalabraEnSecundaria(char *palabra);
int buscarPalabraEnArchivo(char *palabra);
int algoritmoClock();
int algoritmoNRU();
void cambiarBits();
void cargarPaginaMemoriaPrincipal(int paginaSecundaria);
void mostrarPaginasMasReferenciadas();
void mostrarPaginasQueCambiaronBit();
void mostrarEstadisticas();
int validarSoloLetras(char *frase);
void menuSeleccionAlgoritmo();
void pedirFrase();
void menuImpresionMemoria();

// Carga las palabras desde el archivo
void cargarVectorPalabras() {
    FILE* archivo = fopen("palabras.txt", "r");
    if (!archivo) {
        printf("No se pudo abrir palabras.txt\n");
        exit(1);
    }
    int cont = 0;
    while (cont < TotalPalabras && fscanf(archivo, "%19s", palabrasArchivo[cont]) == 1)
        cont++;
    fclose(archivo);
    printf("Archivo cargado con %d palabras.\n", cont);
}

// Carga la memoria secundaria
void cargarPalabrasMemoriaSecundaria() {
    int cont = 0;
    for (int i = 0; i < TotalPalabras; i++) usado[i] = false;
    for (int i = 0; i < PaginasSecundaria; i++) {
        memoriaSecundaria[i].R = 0;
        memoriaSecundaria[i].M = 0;
        memoriaSecundaria[i].referencias = 0;
        for (int j = 0; j < PalabrasPorPagina; j++) {
            while (usado[cont]) cont = (cont + 1) % TotalPalabras;
            strcpy(memoriaSecundaria[i].palabras[j], palabrasArchivo[cont]);
            usado[cont] = true;
            cont = (cont + 1) % TotalPalabras;
        }
    }
}

// Carga la memoria principal
void cargarPalabrasMemoriaPrincipal() {
    int cont = 0;
    for (int i = 0; i < PaginasPrincipal; i++) {
        memoriaPrincipal[i].R = 0;
        memoriaPrincipal[i].M = 0;
        memoriaPrincipal[i].referencias = 0;
        for (int j = 0; j < PalabrasPorPagina; j++) {
            while (usado[cont]) cont = (cont + 1) % TotalPalabras;
            strcpy(memoriaPrincipal[i].palabras[j], palabrasArchivo[cont]);
            usado[cont] = true;
            cont = (cont + 1) % TotalPalabras;
        }
    }
}

// Imprime la memoria principal
void imprimirMemoriaPrincipal() {
    printf("\nMEMORIA PRINCIPAL:\n");
    for (int i = 0; i < PaginasPrincipal; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < PalabrasPorPagina; j++)
            printf("%s ", memoriaPrincipal[i].palabras[j]);
        printf("\n");
    }
}

// Imprime la memoria secundaria
void imprimirMemoriaSecundaria() {
    printf("\nMEMORIA SECUNDARIA:\n");
    for (int i = 0; i < PaginasSecundaria; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < PalabrasPorPagina; j++)
            printf("%s ", memoriaSecundaria[i].palabras[j]);
        printf("\n");
    }
}

// Busca palabra en la principal
int buscarPalabraEnPrincipal(char *palabra) {
    for (int i = 0; i < PaginasPrincipal; i++) {
        for (int j = 0; j < PalabrasPorPagina; j++) {
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

// Busca palabra en la secundaria
int buscarPalabraEnSecundaria(char *palabra) {
    for (int i = 0; i < PaginasSecundaria; i++) {
        for (int j = 0; j < PalabrasPorPagina; j++) {
            if (strcmp(memoriaSecundaria[i].palabras[j], palabra) == 0) {
                memoriaSecundaria[i].referencias++;
                totalReferencias++;
                return i;
            }
        }
    }
    return -1;
}

// Busca palabra en el archivo base
int buscarPalabraEnArchivo(char *palabra) {
    for (int i = 0; i < TotalPalabras; i++)
        if (strcmp(palabrasArchivo[i], palabra) == 0)
            return i;
    return -1;
}

// Algoritmo CLOCK
int algoritmoClock() {
    static int puntero = 0;
    for (int i = 0; i < PaginasPrincipal * 2; i++) {
        if (memoriaPrincipal[puntero].R == 0) {
            totalReemplazos++;
            int r = puntero;
            puntero = (puntero + 1) % PaginasPrincipal;
            return r;
        }
        memoriaPrincipal[puntero].R = 0;
        puntero = (puntero + 1) % PaginasPrincipal;
    }
    return 0;
}

// Algoritmo NRU
int algoritmoNRU() {
    for (int clase = 0; clase < 4; clase++) {
        for (int i = 0; i < PaginasPrincipal; i++) {
            int R = memoriaPrincipal[i].R;
            int M = memoriaPrincipal[i].M;
            if ((clase == 0 && R == 0 && M == 0) ||
                (clase == 1 && R == 0 && M == 1) ||
                (clase == 2 && R == 1 && M == 0) ||
                (clase == 3 && R == 1 && M == 1)) {
                totalReemplazos++;
                return i;
            }
        }
    }
    return 0;
}

// Carga página desde secundaria
void cargarPaginaMemoriaPrincipal(int paginaSecundaria) {
    int num = (algoritmoSeleccionado == 0) ? algoritmoClock() : algoritmoNRU();
    for (int i = 0; i < PalabrasPorPagina; i++)
        strcpy(memoriaPrincipal[num].palabras[i], memoriaSecundaria[paginaSecundaria].palabras[i]);
    memoriaPrincipal[num].R = 1;
    memoriaPrincipal[num].M = 0;
    memoriaPrincipal[num].referencias = 0;
    fallosPagina++;
    fprintf(salida, "Página secundaria %d cargada en principal %d\n", paginaSecundaria, num);
}

// Cambia bits R y M aleatoriamente
void cambiarBits() {
    contadorBitsCambiados = 0;
    for (int i = 0; i < 10; i++) {
        int p = rand() % PaginasPrincipal;
        memoriaPrincipal[p].R = !memoriaPrincipal[p].R;
        memoriaPrincipal[p].M = !memoriaPrincipal[p].M;
        paginasConBitCambiado[contadorBitsCambiados++] = p;
    }
}

// Muestra páginas más referenciadas
void mostrarPaginasMasReferenciadas() {
    int indices[PaginasPrincipal];
    for (int i = 0; i < PaginasPrincipal; i++) indices[i] = i;
    for (int i = 0; i < PaginasPrincipal - 1; i++) {
        for (int j = 0; j < PaginasPrincipal - i - 1; j++) {
            if (memoriaPrincipal[indices[j]].referencias < memoriaPrincipal[indices[j+1]].referencias) {
                int temp = indices[j];
                indices[j] = indices[j+1];
                indices[j+1] = temp;
            }
        }
    }
    printf("\n10 páginas más referenciadas:\n");
    for (int i = 0; i < 10; i++) {
        printf("P%d - %lu referencias\n", indices[i], memoriaPrincipal[indices[i]].referencias);
        fprintf(salida, "P%d - %lu referencias\n", indices[i], memoriaPrincipal[indices[i]].referencias);
    }
}

// Muestra las páginas que cambiaron bit
void mostrarPaginasQueCambiaronBit() {
    printf("\nPáginas que cambiaron bit:\n");
    for (int i = 0; i < contadorBitsCambiados; i++) {
        int p = paginasConBitCambiado[i];
        printf("P%d -> R=%d, M=%d\n", p, memoriaPrincipal[p].R, memoriaPrincipal[p].M);
        fprintf(salida, "P%d -> R=%d, M=%d\n", p, memoriaPrincipal[p].R, memoriaPrincipal[p].M);
    }
}

// Muestra estadísticas
void mostrarEstadisticas() {
    printf("\nEstadísticas:\n");
    printf("Fallos de página: %lu\n", fallosPagina);
    fprintf(salida, "Fallos de página: %lu\n", fallosPagina);
}

// Valida letras y espacios
int validarSoloLetras(char *frase) {
    for (int i = 0; frase[i]; i++)
        if (!isalpha(frase[i]) && frase[i] != ' ') return 0;
    return 1;
}

// Permite elegir algoritmo
void menuSeleccionAlgoritmo() {
    printf("\nSeleccione algoritmo (0=CLOCK, 1=NRU): ");
    scanf("%d", &algoritmoSeleccionado);
    while (getchar() != '\n');
}

// Permite elegir qué memoria imprimir
void menuImpresionMemoria() {
    int op;
    printf("\nSeleccione memoria a imprimir (1=Principal, 2=Secundaria): ");
    scanf("%d", &op);
    while (getchar() != '\n');
    if (op == 1) imprimirMemoriaPrincipal();
    else imprimirMemoriaSecundaria();
}

// Pide una frase al usuario
void pedirFrase() {
    char frase[200];
    char palabrasNoEncontradas[20][20];
    int noEncontradas = 0;
    printf("\nIngrese una frase: ");
    fgets(frase, sizeof(frase), stdin);
    frase[strcspn(frase, "\n")] = '\0';
    if (!validarSoloLetras(frase)) {
        printf("Solo se permiten letras y espacios.\n");
        return;
    }

    fprintf(salida, "\nFrase ingresada: %s\n", frase);

    char *token = strtok(frase, " ");
    while (token) {
        int pag = buscarPalabraEnPrincipal(token);
        if (pag != -1) {
            printf("'%s' encontrada en principal\n", token);
            fprintf(salida, "'%s' encontrada en principal\n", token);
        } else {
            int sec = buscarPalabraEnSecundaria(token);
            if (sec != -1) {
                printf("'%s' encontrada en secundaria\n", token);
                fprintf(salida, "'%s' encontrada en secundaria\n", token);
                cargarPaginaMemoriaPrincipal(sec);
            } else {
                printf("'%s' no encontrada\n", token);
                fprintf(salida, "'%s' no encontrada\n", token);
                strcpy(palabrasNoEncontradas[noEncontradas++], token);
                fallosPagina++;
            }
        }
        token = strtok(NULL, " ");
    }

    cambiarBits();
    mostrarEstadisticas();
    printf("\nPalabras no encontradas:\n");
    for (int i = 0; i < noEncontradas; i++) {
        printf("%s\n", palabrasNoEncontradas[i]);
        fprintf(salida, "%s\n", palabrasNoEncontradas[i]);
    }
    mostrarPaginasMasReferenciadas();
    mostrarPaginasQueCambiaronBit();
    menuImpresionMemoria();
}

// Programa principal
int main(void) {
    srand(time(NULL));
    salida = fopen("resultados.txt", "w");
    cargarVectorPalabras();
    cargarPalabrasMemoriaSecundaria();
    cargarPalabrasMemoriaPrincipal();
    menuSeleccionAlgoritmo();

    while (1) {
        pedirFrase();
        printf("\n¿Desea ingresar otra frase? (s/n): ");
        char resp[10];
        fgets(resp, sizeof(resp), stdin);
        if (tolower(resp[0]) != 's') break;
    }

    fclose(salida);
    printf("\nSimulación finalizada. Resultados en resultados.txt\n");
    return 0;
}
