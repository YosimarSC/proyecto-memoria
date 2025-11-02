#ifndef MEMORIA_H
#define MEMORIA_H
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



void inicializarMemorias();

void mostrarMemoriaPrincipal();

void mostrarMemoriaSecundaria();

int buscarPalabraEnPrincipal(char *palabra);

int buscarPalabraEnSecundaria(char *palabra);

void cargarPalabrasDesdeArchivo(const char *nombreArchivo);

#endif

