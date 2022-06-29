#include <stdio.h>
#include <stdlib.h>
#include<time.h>

typedef struct celda
{
    int color; /// 1-Blanco 2-Azul 3-Rojo 4-Naranja 5-verde
    int edad; ///
    int heridas; /// 0-NO 1-SI
    int fila; /// fila a la que pertenece cada celda
    int semanasInfectado; /// cantidad de semanas infectado
    int semanasTotales;
    int semanasPodado;

} arbol;

int probContagio;
float numeroRandom;

int main()
{
    clock_t tiempoInicial, tiempoFinal;
    int tamanioMatriz = 500;
    int cantidadSemanas = 1200;
    float porcentajeEnfermos;

    int tamanioFinal = (tamanioMatriz*tamanioMatriz) + 4*tamanioMatriz;
    arbol *matrizCampo = NULL;
    matrizCampo=(arbol*)malloc(tamanioFinal* sizeof(arbol));
    arbol *matrizAux = NULL;
    matrizAux=(arbol*)malloc(tamanioFinal* sizeof(arbol));


///Inicializar la matriz a partir de las diferentes probabilidades dadas

    int cantEjecuciones;
    clock_t tiempototal;
    int i;
    int random;
    int fila=-1;
    int semana;
    int indice;
    float  arbolesContagiando;
    float vecinosVisitados;
    int cambioEstado;
    int probHeridas;
    int tratamiento;
    int cambio;
    int randomParaSano;
    float susceptibilidad;
    float probabilidadContagio;
 
    for(cantEjecuciones=0; cantEjecuciones<5; cantEjecuciones++){

    #pragma omp parallel for private(i) num_threads(10)
    for(i=0; i<(tamanioMatriz*2); i++){
        matrizCampo[i].fila = -5;
        matrizAux[i].fila = -5;
        matrizAux[i].color = 10;
        matrizCampo[i].color = 10;
    }
    #pragma omp parallel for  private(i,random) num_threads(10)
    for(i=tamanioMatriz*2; i<tamanioFinal-2*tamanioMatriz; i++){
        tiempoInicial = clock();
        random = rand() % 101;
        matrizCampo[i].semanasInfectado=0;
        matrizAux[i].semanasInfectado=0;
        matrizCampo[i].semanasTotales=0;
        matrizAux[i].semanasTotales=0;
        matrizCampo[i].semanasPodado=0;
        matrizAux[i].semanasPodado=0;
        ///asignacion de colores para los arboles
        if(random<65){
            matrizCampo[i].color = 5; ///Arbol Sano (verde)
            matrizAux[i].color = 5;
        }else{
            if(random>=65 && random<70){
                matrizCampo[i].color = 3;///Arbol con sintomas (rojo)
                matrizAux[i].color = 3;
            }else{
                if(random>=70 && random<80){
                    matrizCampo[i].color = 4;///Arbol enfermo sin sintomas (naranja)
                    matrizAux[i].color = 4;
                }else{
                    if(random>=80 && random<=100){
                        matrizCampo[i].color = 2; ///Arbol con tratamiento (azul)
                        matrizAux[i].color = 2;
                    }
                }
            }
        }
///asignacion de edad de los arboles
        if(random<30){
            matrizCampo[i].edad = 52; ///Arbol joven
            matrizAux[i].edad = 52;
        }
        else{
            if(random>=30 && random<80){
            matrizCampo[i].edad = 157; ///Arbol adulto
            matrizAux[i].edad = 157;
            }else{
                if(random>=80 && random<=100){
                    matrizCampo[i].edad = 1821; ///Arbol viejo)
                    matrizAux[i].edad = 1821;
                }
            }
        }
        if((i%tamanioMatriz) == 0){
            fila++;
        }
        matrizCampo[i].fila = fila;
        matrizAux[i].fila = fila;
        matrizCampo[i].heridas=0;
        matrizAux[i].heridas=0;
    }
    #pragma omp parallel for private(i) num_threads(10)
    for(i=(tamanioFinal-2*tamanioMatriz); i<tamanioFinal; i++){
        matrizCampo[i].fila = tamanioMatriz+5;
        matrizAux[i].fila = tamanioMatriz+5;
        matrizAux[i].color = 10;
        matrizCampo[i].color = 10;
    }


    /*int j;
    for(j =0; j<tamanioFinal; j++){

    printf("posicion : %d\n", j);
    printf("fila al que pertenece : %d\n", matrizCampo[j].fila);
    printf("color celda : %d\n", matrizCampo[j].color);
    printf("-----------------------------------------------------\n");
    }*/

///Modelado de actualizacion de la grilla a medida que pasa el tiempo

for(semana = 0; semana < cantidadSemanas; semana++ ){
///Recoleccion de informacion de los vecinos
#pragma omp parallel for  private(indice, susceptibilidad, porcentajeEnfermos, probabilidadContagio, probHeridas, arbolesContagiando, vecinosVisitados) num_threads(10)
for(indice =tamanioMatriz*2; indice< (tamanioFinal-2*tamanioMatriz); indice++){
    arbolesContagiando=0;
    vecinosVisitados=0;
    if((matrizCampo[indice-1].fila == matrizCampo[indice].fila)){
        if(matrizCampo[indice-1].color == 3){
        arbolesContagiando+=1;
        }
        if(matrizCampo[indice-1].fila >= 0){
        vecinosVisitados+=1;
        }

    }

    if((matrizCampo[indice-2].fila == matrizCampo[indice].fila)){
        if(matrizCampo[indice-2].color == 3){
        arbolesContagiando+=1;
        }
        if(matrizCampo[indice-2].fila >= 0){
        vecinosVisitados+=1;}
    }

    if((matrizCampo[indice+1].fila == matrizCampo[indice].fila)){
        if( matrizCampo[indice+1].color == 3){
        arbolesContagiando+=1;}
        if(matrizCampo[indice+1].fila < tamanioMatriz){
        vecinosVisitados+=1;
        }

    }
    if((matrizCampo[indice+2].fila == matrizCampo[indice].fila)){
        if(matrizCampo[indice+2].color == 3){
        arbolesContagiando+=1;
        }
        if(matrizCampo[indice+2].fila < tamanioMatriz){
        vecinosVisitados+=1;
        }

    }

    if((((matrizCampo[indice].fila) -1) == matrizCampo[indice-tamanioMatriz].fila)){
        if(matrizCampo[indice-tamanioMatriz].color == 3){
        arbolesContagiando+=1;}
        if(matrizCampo[indice-tamanioMatriz].fila >= 0){
        vecinosVisitados+=1;
        ;}

    }

     if((((matrizCampo[indice].fila) -1) == matrizCampo[indice-(tamanioMatriz-1)].fila) ){
        if(matrizCampo[indice-(tamanioMatriz-1)].color == 3 ){
        arbolesContagiando+=1;}
        if(matrizCampo[indice-(tamanioMatriz-1)].fila >= 0){
        vecinosVisitados+=1;
        }

    }

     if(((matrizCampo[indice].fila -1) == matrizCampo[indice-(tamanioMatriz+1)].fila)){
        if(matrizCampo[indice-tamanioMatriz+1].color == 3){
        arbolesContagiando+=1;}
        if(matrizCampo[indice-tamanioMatriz+1].fila >= 0){
        vecinosVisitados+=1;
        }
     }


     if(((matrizCampo[indice].fila -2) == matrizCampo[indice-(2*tamanioMatriz)].fila)){
        if(matrizCampo[indice-(2*tamanioMatriz)].color == 3 ){
        arbolesContagiando+=1;}
        if(matrizCampo[indice-(2*tamanioMatriz)].fila >= 0){
        vecinosVisitados+=1;
        }

    }

     if(((matrizCampo[indice].fila +1) == matrizCampo[indice+tamanioMatriz].fila) ){
        if(matrizCampo[indice+tamanioMatriz].color == 3){
        arbolesContagiando+=1;}
        if(matrizCampo[indice+tamanioMatriz].fila < tamanioMatriz){
        vecinosVisitados+=1;
        }

    }

     if(((matrizCampo[indice].fila +1) == matrizCampo[indice+(tamanioMatriz-1)].fila)){
        if(matrizCampo[indice+(tamanioMatriz-1)].color == 3 ){
        arbolesContagiando+=1;}
        if(matrizCampo[indice+(tamanioMatriz-1)].fila < tamanioMatriz){
        vecinosVisitados+=1;
        }
    }

     if((matrizCampo[indice].fila +1) == matrizCampo[indice+(tamanioMatriz+1)].fila){
        if(matrizCampo[indice+(tamanioMatriz+1)].color == 3 ){
        arbolesContagiando+=1;
        }
        if(matrizCampo[indice+(tamanioMatriz+1)].fila < tamanioMatriz){
        vecinosVisitados+=1;
        }

    }

     if(((matrizCampo[indice].fila +2) == matrizCampo[indice+(2*tamanioMatriz)].fila)){
        if(matrizCampo[indice+(2*tamanioMatriz)].color == 3){
        arbolesContagiando+=1;}
        if(matrizCampo[indice+(2*tamanioMatriz)].fila < tamanioMatriz){
        vecinosVisitados+=1;
        }
    }

    //printf("cantidad de arboles contagiando %d\n", arbolesContagiando);
    //printf("cantidad de vecinos visitados %d\n", vecinosVisitados);
    //printf("estado del arbol: %d\n", matrizCampo[indice].color);

    ///fin recoleccion informacion de los vecinos

    ///calculo de susceptibilidad
    if(matrizCampo[indice].edad < 157 ){
        if(matrizCampo[indice].heridas ==1){
        susceptibilidad =  0.35 + 0.15;
    }else{
        susceptibilidad = 0.35;

     }
    }

    if((matrizCampo[indice].edad >= 157 && matrizCampo[indice].edad < 1821)){
        if(matrizCampo[indice].heridas ==1){
        susceptibilidad =  0.17 + 0.15;

    }else{
        susceptibilidad = 0.17;

    }
    }

    if((matrizCampo[indice].edad >= 1821)){
        if(matrizCampo[indice].heridas ==1){
        susceptibilidad= 0.63 + 0.15;
    }else{
        susceptibilidad = 0.63;


    }
    }

    ///fin calculo de susceptibilidad

    ///aumento edad del arbol
    matrizAux[indice].edad+=1;

    ///aumento la cantidad de semanas de infectado de los arboles
    if(matrizCampo[indice].color == 4){
        matrizAux[indice].semanasInfectado+=1;
        matrizAux[indice].semanasTotales++;
    }
    ///cambio de estado de enfermo sin sintomas a enfermo con sintomas

    if(matrizCampo[indice].semanasInfectado == 3){
        matrizAux[indice].color =3;  /// el arbol luego de 3 semanas pasa a contagiar
    }
    ///cambio de estado de enfermo con sintomas a arbol con tratamiento
    if(matrizCampo[indice].color == 3){
        matrizAux[indice].semanasTotales++;
        if((rand()%100)<=85){
            matrizAux[indice].color =2;
            matrizAux[indice].semanasInfectado=0;
            matrizAux[indice].semanasTotales+=1;
        }
    }

    ///cambio de estado para arboles con tratamiento

    if(matrizCampo[indice].color == 2){
        if(matrizCampo[indice].semanasTotales <= 8){
            matrizAux[indice].semanasTotales++;
        }else{

            if(matrizCampo[indice].edad < 157){
               if(rand()%100 > 3){
                matrizAux[indice].color = 5;
                matrizAux[indice].semanasTotales=0;
            }else{
                matrizAux[indice].color = 1;
                matrizAux[indice].semanasTotales=0;
            }
            }

            if((matrizCampo[indice].edad >= 157 && matrizCampo[indice].edad <1821)){
                if(rand()%100 > 15){
                matrizAux[indice].color = 5;
                matrizAux[indice].semanasTotales=0;
            }else{
                matrizAux[indice].color = 1;
                matrizAux[indice].semanasTotales=0;
            }
            }

            if(matrizCampo[indice].edad >= 1821){
                if(rand()%100 > 53){
                matrizAux[indice].color = 5;
                matrizAux[indice].semanasTotales=0;
            }else{
                matrizAux[indice].color = 5;
                matrizAux[indice].semanasTotales =0;
                matrizAux[indice].edad=57;
            }
            }


        }
    }

    /// cambio de estado de podado a sano

    if(matrizCampo[indice].color == 1){
        if(matrizCampo[indice].semanasPodado<=12){
            matrizAux[indice].semanasPodado++;
        }else{
            matrizAux[indice].color = 5;
            matrizAux[indice].semanasPodado=0;

        }
    }


    ///calculo cambio de estado
    if(matrizCampo[indice].color == 5){
         porcentajeEnfermos = (arbolesContagiando/vecinosVisitados);
         probabilidadContagio = (porcentajeEnfermos + susceptibilidad) *0.60 +0.07;
         if(probabilidadContagio < ((rand()%1001)/1000.f)){
            matrizAux[indice].color=4;
         }
    }


    if(semana > 0){
        probHeridas = rand() % 101;

        if((matrizCampo[indice].edad < 157) && probHeridas <23){
            matrizAux[indice].heridas = 1;
        }else{
            matrizAux[indice].heridas =0;
        }

        if((matrizCampo[indice].edad >= 157 && matrizCampo[indice].edad < 1821) && (probHeridas <8 )){
            matrizAux[indice].heridas = 1;
        }else{
            matrizAux[indice].heridas =0;
        }

        if((matrizCampo[indice].edad >= 1821) && (probHeridas <37 )){
            matrizAux[indice].heridas = 1;
        }else{
            matrizAux[indice].heridas =0;
        }



    } /// fin para el random de heridas abiertas



} /// fin for para explorar vecinos

        ///copio de la matriz auxiliar a la matriz original
        for(cambio =tamanioMatriz*2; cambio< (tamanioFinal-2*tamanioMatriz); cambio++){
        matrizCampo[cambio].color = matrizAux[cambio].color;
        matrizCampo[cambio].edad = matrizAux[cambio].edad;
        matrizCampo[cambio].heridas = matrizAux[cambio].heridas;
        matrizCampo[cambio].semanasInfectado= matrizAux[cambio].semanasInfectado;
        matrizCampo[cambio].semanasTotales = matrizAux[cambio].semanasTotales;
        matrizCampo[cambio].semanasPodado = matrizAux[cambio].semanasPodado;

        }


}/// fin for cantidad de semanas

tiempoFinal = clock();
double segundos = (double)(tiempoFinal-tiempoInicial) / CLOCKS_PER_SEC;
printf("EL TIEMPO DE EJECUCION DE UNA VUELTA FUE: %f\n", segundos);
tiempototal += (tiempoFinal-tiempoInicial);
fila =-1;


}
printf("El tiempo promedio total fue: %f\n", (double)(tiempototal/5)/CLOCKS_PER_SEC);

}


