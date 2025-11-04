#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM_MAX_PALABRA 64
#define PALABRAS_POR_PAGINA 6
#define PAGINAS_PRINCIPAL 81
#define PAGINAS_SECUNDARIA 162

typedef struct {
    char palabras[PALABRAS_POR_PAGINA][TAM_MAX_PALABRA];
    char letra_inicial;
    int R;
    int M;
    int enPrincipal;
    unsigned long referencias;
} Pagina;

Pagina memoriaPrincipal[PAGINAS_PRINCIPAL];
Pagina memoriaSecundaria[PAGINAS_SECUNDARIA];

unsigned long totalReferencias = 0;
unsigned long totalReemplazos = 0;
unsigned long fallosPagina = 0;

void iniciarMemorias() {
    for (int i = 0; i < PAGINAS_PRINCIPAL; i++) {
        memoriaPrincipal[i].R = 0;
        memoriaPrincipal[i].M = 0;
        memoriaPrincipal[i].enPrincipal = 1;
        memoriaPrincipal[i].referencias = 0;
        memoriaPrincipal[i].letra_inicial = '?';
        for (int j = 0; j < PALABRAS_POR_PAGINA; j++)
            memoriaPrincipal[i].palabras[j][0] = '\0';
    }
    for (int i = 0; i < PAGINAS_SECUNDARIA; i++) {
        memoriaSecundaria[i].R = 0;
        memoriaSecundaria[i].M = 0;
        memoriaSecundaria[i].enPrincipal = 0;
        memoriaSecundaria[i].referencias = 0;
        memoriaSecundaria[i].letra_inicial = '?';
        for (int j = 0; j < PALABRAS_POR_PAGINA; j++)
            memoriaSecundaria[i].palabras[j][0] = '\0';
    }
    printf("Memorias inicializadas correctamente.\n");
}

void cargarPalabrasDesdeArchivo(const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo %s\n", nombreArchivo);
        return;
    }

    char linea[100];
    int indice = 0;

    while (fgets(linea, sizeof(linea), archivo)) {
        linea[strcspn(linea, "\n")] = 0;
        if (strlen(linea) == 0) continue;

        int pagina = indice / PALABRAS_POR_PAGINA;
        int pos = indice % PALABRAS_POR_PAGINA;

        if (pagina < PAGINAS_PRINCIPAL) {
            strcpy(memoriaPrincipal[pagina].palabras[pos], linea);
            memoriaPrincipal[pagina].letra_inicial = tolower(linea[0]);
        } else if (pagina - PAGINAS_PRINCIPAL < PAGINAS_SECUNDARIA) {
            strcpy(memoriaSecundaria[pagina - PAGINAS_PRINCIPAL].palabras[pos], linea);
            memoriaSecundaria[pagina - PAGINAS_PRINCIPAL].letra_inicial = tolower(linea[0]);
        }
        indice++;
    }

    fclose(archivo);
    printf("Palabras cargadas desde %s\n", nombreArchivo);
}

void mostrarMemoriaPrincipal() {
    printf("\n=== Memoria Principal ===\n");
    for (int i = 0; i < PAGINAS_PRINCIPAL; i++) {
        printf("[%2d] ", i);
        for (int j = 0; j < PALABRAS_POR_PAGINA; j++)
            if (strlen(memoriaPrincipal[i].palabras[j]) > 0)
                printf("%s ", memoriaPrincipal[i].palabras[j]);
        printf("\n");
    }
}

void mostrarMemoriaSecundaria() {
    printf("\n=== Memoria Secundaria ===\n");
    for (int i = 0; i < PAGINAS_SECUNDARIA; i++) {
        printf("[%3d] ", i);
        for (int j = 0; j < PALABRAS_POR_PAGINA; j++)
            if (strlen(memoriaSecundaria[i].palabras[j]) > 0)
                printf("%s ", memoriaSecundaria[i].palabras[j]);
        printf("\n");
    }
}

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
        } else memoriaPrincipal[puntero].R = 0;
        puntero = (puntero + 1) % PAGINAS_PRINCIPAL;
    }
    if (!reemplazada)
        printf("No se encontró página con R=0\n");
}

void algoritmoNRU() {
    int candidato = -1;
    for (int i = 0; i < PAGINAS_PRINCIPAL; i++) {
        if (memoriaPrincipal[i].R == 0 && memoriaPrincipal[i].M == 0) { candidato = i; break; }
        else if (memoriaPrincipal[i].R == 0 && candidato == -1) candidato = i;
    }
    if (candidato != -1) {
        printf("Reemplazando página %d (NRU)\n", candidato);
        memoriaPrincipal[candidato].M = 0;
        memoriaPrincipal[candidato].R = 0;
        totalReemplazos++;
    } else printf("No se encontró página candidata.\n");
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

int main() {
    int opcion;
    char palabra[64];

    printf("===== Proyecto II - Administración de Memoria =====\n");
    iniciarMemorias();
    cargarPalabrasDesdeArchivo("palabras.txt");

    do {
        printf("\n1. Mostrar Memoria Principal\n2. Mostrar Memoria Secundaria\n3. Buscar Palabra\n4. Clock\n5. NRU\n6. Ver Estadísticas\n0. Salir\nOpción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: mostrarMemoriaPrincipal(); break;
            case 2: mostrarMemoriaSecundaria(); break;
            case 3:
                printf("Ingrese palabra: "); scanf("%s", palabra);
                if (buscarPalabraEnPrincipal(palabra))
                    printf("Palabra encontrada en memoria principal.\n");
                else if (buscarPalabraEnSecundaria(palabra)) {
                    printf("Palabra encontrada en memoria secundaria (fallo de página)\n");
                    fallosPagina++;
                    algoritmoClock();
                }
                else printf("Palabra no encontrada.\n");
                break;
            case 4: algoritmoClock(); break;
            case 5: algoritmoNRU(); break;
            case 6: mostrarEstadisticas(); break;
            case 0:
                mostrarEstadisticas();
                guardarEstadisticasEnArchivo();
                printf("Saliendo...\n");
                break;
            default: printf("Opción inválida.\n");
        }
    } while (opcion != 0);

    return 0;
}
