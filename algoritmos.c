#include "memoria.h"
#include <stdio.h>

void algoritmoClock() {
    static int puntero = 0;
    int reemplazada = 0;
    for (int i = 0; i < PAGINAS_PRINCIPAL * 2 && !reemplazada; i++) {
        if (memoriaPrincipal[puntero].R == 0) {
            printf("Reemplazando página %d (Clock)\n", puntero);
            memoriaPrincipal[puntero].M = 0;
            memoriaPrincipal[puntero].R = 0;
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
    } else printf("No se encontró página candidata.\n");
}
