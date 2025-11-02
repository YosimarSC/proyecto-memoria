#include <stdio.h>


#define cantidadLetrasMP 20
#define  cantidadLetrasMS 20

// ------variables globales----- 



char memoriaPrincipal[81][6][cantidadLetrasMP];
char memoriaSecundaria [162][6][cantidadLetrasMS];


void cargarPalabrasMemoriaPrincipal(){
    
    for(int i=0;i<81;i++){
        for(int j=0;j<6;j++){
            strcpy(memoriaPrincipal[i][j], "");  
        }
    }
}

void cargarPalabrasMemoriaSecundaria(){
    
    for(int i=0;i<162;i++){
        for(int j=0;j<6;j++){
            strcpy(memoriaSecundaria[i][j], "");
        }
    }
}


int main (void){



}
