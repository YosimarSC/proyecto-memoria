#include "memoria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Pagina memoriaPrincipal[PAGINAS_PRINCIPAL];
Pagina memoriaSecundaria[PAGINAS_SECUNDARIA];


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
    printf(" Memorias inicializadas correctamente.\n");
}


void cargarPalabrasDesdeArchivo(const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        printf(" No se pudo abrir el archivo %s\n", nombreArchivo);
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
    printf(" Palabras cargadas desde %s\n", nombreArchivo);
}

// muestra la memoria principal
void mostrarMemoriaPrincipal() {
    printf("\n===  Memoria Principal ===\n");
    for (int i = 0; i < PAGINAS_PRINCIPAL; i++) {
        printf("[%2d] ", i);
        for (int j = 0; j < PALABRAS_POR_PAGINA; j++)
            if (strlen(memoriaPrincipal[i].palabras[j]) > 0)
                printf("%s ", memoriaPrincipal[i].palabras[j]);
        printf("\n");
    }
}

// muestra la memoria secundaria
void mostrarMemoriaSecundaria() {
    printf("\n=== 💾 Memoria Secundaria ===\n");
    for (int i = 0; i < PAGINAS_SECUNDARIA; i++) {
        printf("[%3d] ", i);
        for (int j = 0; j < PALABRAS_POR_PAGINA; j++)
            if (strlen(memoriaSecundaria[i].palabras[j]) > 0)
                printf("%s ", memoriaSecundaria[i].palabras[j]);
        printf("\n");
    }
}

// busca palabra en memoria principal
int buscarPalabraEnPrincipal(char *palabra) {
    for (int i = 0; i < PAGINAS_PRINCIPAL; i++)
        for (int j = 0; j < PALABRAS_POR_PAGINA; j++)
            if (strcmp(memoriaPrincipal[i].palabras[j], palabra) == 0) {
                memoriaPrincipal[i].referencias++;
                return 1;
            }
    return 0;
}

// busca palabra en memoria secundaria
int buscarPalabraEnSecundaria(char *palabra) {
    for (int i = 0; i < PAGINAS_SECUNDARIA; i++)
        for (int j = 0; j < PALABRAS_POR_PAGINA; j++)
            if (strcmp(memoriaSecundaria[i].palabras[j], palabra) == 0) {
                memoriaSecundaria[i].referencias++;
                return 1;
            }
    return 0;
}
