#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>
#define tamanioMatriz 500
#define cantEjecuciones 5
#define cantSemanas 1200

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

int tamanioFinal;
int filasXproceso;
int fila =-1;
clock_t tiempoInicial, tiempoFinal, tiempototal;


int main(int argc, char **argv) {

    int proceso, cantProcesos;
    tamanioFinal = tamanioMatriz * tamanioMatriz;
	MPI_Init(&argc, &argv);
    MPI_Status status; //Para usar en el "recibir" del mensaje y en el test
    MPI_Request request; //Para usar en el MPI_Test
    MPI_Comm_size(MPI_COMM_WORLD,&cantProcesos);// Obtener la cantidad de procesos
    MPI_Comm_rank(MPI_COMM_WORLD,&proceso);// Obtener el número de proceso
    
    int tamReparto = (tamanioMatriz*2);
	arbol *matrizCampo = NULL;

	/// matrices donde reciben su parte los procesos
	arbol *matrizLocal = NULL;
	arbol *matrizLocalAux = NULL;

	/// arreglos auxiliares para recibir filas
	arbol *arregloArriba = NULL;
    arbol *arregloAbajo = NULL;
    arbol *arregloAuxA=NULL;
    arbol *arregloAuxB=NULL;

    filasXproceso = (tamanioFinal/cantProcesos);
    matrizLocal = (arbol*)malloc(filasXproceso*sizeof(arbol));
    matrizLocalAux= (arbol*)malloc(filasXproceso*sizeof(arbol));
    arregloArriba = (arbol*)malloc(tamReparto*sizeof(arbol));
    arregloAbajo = (arbol*)malloc(tamReparto*sizeof(arbol));
    arregloAuxA = (arbol*)malloc(tamReparto*sizeof(arbol));
    arregloAuxB = (arbol*)malloc(tamReparto*sizeof(arbol));

    for(int ejecuciones = 0; ejecuciones < cantEjecuciones; ejecuciones++){
	tiempoInicial = clock();
	if(proceso == 0){
		matrizCampo = (arbol*)malloc(tamanioFinal*sizeof(arbol)); /// doy memoria a la matriz original
		/// inicializo la matriz original
        int random;
        fila =-1;
   	 	for(int i=0; i<tamanioFinal; i++){

        	random = rand() % 101;
        	matrizCampo[i].semanasInfectado=0;
      
        	matrizCampo[i].semanasTotales=0;

        	matrizCampo[i].semanasPodado=0;
        
        	///asignacion de colores para los arboles
        	if(random<65){
            	matrizCampo[i].color = 5; ///Arbol Sano (verde)
            	
        	}else{
            	if(random>=65 && random<70){
                		matrizCampo[i].color = 3;///Arbol con sintomas (rojo)
                	
            	}else{
                		if(random>=70 && random<80){
                    		matrizCampo[i].color = 4;///Arbol enfermo sin sintomas (naranja)
                   		
               	 }else{
                    	if(random>=80 && random<=100){
                        	matrizCampo[i].color = 2; ///Arbol con tratamiento (azul)
                        	
                    		}
                		}
            	}
       	 }
		///asignacion de edad de los arboles
        	if(random<30){
            	matrizCampo[i].edad = 52; ///Arbol joven
            	
        	}
        	else{
            	if(random>=30 && random<80){
            	matrizCampo[i].edad = 157; ///Arbol adulto
           		
            	}else{
                		if(random>=80 && random<=100){
                    	matrizCampo[i].edad = 1821; ///Arbol viejo)
                    	
               	 	}
            	}
       	 }
        	if((i%tamanioMatriz) == 0){
            fila++;
        	}
        	matrizCampo[i].fila = fila;
        	
        	matrizCampo[i].heridas=0;
        	
    		} /// fin for inicializacion del campo de arboles

///fin if del proceso 0
}


MPI_Scatter(matrizCampo,(filasXproceso*sizeof(arbol)),MPI_BYTE, matrizLocal,filasXproceso*sizeof(arbol), MPI_BYTE, 0, MPI_COMM_WORLD);


    int filasAbajo = (filasXproceso) - (2*tamanioMatriz);
    int arbolesContagiando;
    int vecinosVisitados;
    int indiceAbajo= 0;
    int indiceAbajoB=0;
    int cambioEstado;
    int probHeridas;
    int tratamiento;
    int cambio;
    int indice = 0;
    int randomParaSano;
    float susceptibilidad;
    float probabilidadContagio;
    float porcentajeEnfermos;

    


    if(proceso == 0){
        for(int h=0; h<(tamanioMatriz*2); h++){
                arregloArriba[h].color = 10;
                arregloArriba[h].fila = -5;

    		}
    }
    if(proceso== cantProcesos-1){
        for(int i = 0; i< (tamanioMatriz*2); i++){
                arregloAbajo[i].color = 10;
                arregloAbajo[i].fila = tamanioMatriz+5;
        }

    }
 

    for(int semana =0; semana < cantSemanas; semana++){
        indiceAbajo=0;
        indiceAbajoB=0;
        filasAbajo = (filasXproceso) - (2*tamanioMatriz);
        if(proceso!=0){

        for(int i = 0; i<(tamanioMatriz*2); i++){
            arregloAuxA[i].color = matrizLocal[i].color;
            arregloAuxA[i].edad = matrizLocal[i].edad;
            arregloAuxA[i].fila = matrizLocal[i].fila;
            arregloAuxA[i].heridas = matrizLocal[i].heridas;
            arregloAuxA[i].semanasInfectado = matrizLocal[i].semanasInfectado;
            arregloAuxA[i].semanasPodado = matrizLocal[i].semanasPodado;
            arregloAuxA[i].semanasTotales = matrizLocal[i].semanasTotales;
            }
        }
      
       

        if(proceso != cantProcesos-1){
        for(int j= 0; j<(tamanioMatriz*2); j++){
            arregloAuxB[j].color = matrizLocal[filasAbajo].color;
            arregloAuxB[j].edad = matrizLocal[filasAbajo].edad;
            arregloAuxB[j].fila = matrizLocal[filasAbajo].fila;
            arregloAuxB[j].heridas = matrizLocal[filasAbajo].heridas;
            arregloAuxB[j].semanasInfectado = matrizLocal[filasAbajo].semanasInfectado;
            arregloAuxB[j].semanasPodado = matrizLocal[filasAbajo].semanasPodado;
            arregloAuxB[j].semanasTotales = matrizLocal[filasAbajo].semanasTotales;
            filasAbajo++;
        }
        }
  
        
        if(proceso != 0){
            MPI_Isend(arregloAuxA, (tamReparto*sizeof(arbol)), MPI_BYTE, proceso-1, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
        }
        
        if(proceso != cantProcesos-1){
            MPI_Irecv(arregloAbajo,(tamReparto*sizeof(arbol)), MPI_BYTE, proceso+1, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
        }

        if(proceso != cantProcesos-1){
            MPI_Isend(arregloAuxB, (tamReparto*sizeof(arbol)), MPI_BYTE, proceso+1, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
            
        }

        if(proceso != 0){
            MPI_Irecv(arregloArriba,(tamReparto*sizeof(arbol)), MPI_BYTE, proceso-1, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
        }
        

        ///comienzo para logica de los vecinos
        
        for(indice =0; indice< filasXproceso; indice++){
            arbolesContagiando=0;
            vecinosVisitados=0;

            if(indice>0){            
            if((matrizLocal[indice-1].fila == matrizLocal[indice].fila)){
                if(matrizLocal[indice-1].color == 3){
                    arbolesContagiando+=1;
                }
                if(matrizLocal[indice-1].fila >= 0){
                    vecinosVisitados+=1;
                }

            }
        }
    
            if(indice>1){
            if((matrizLocal[indice-2].fila == matrizLocal[indice].fila)){
                if(matrizLocal[indice-2].color == 3){
                    arbolesContagiando+=1;
                }
                if(matrizLocal[indice-2].fila >= 0){
                    vecinosVisitados+=1;}
            }
            }

            if(indice<filasXproceso-1){
            if((matrizLocal[indice+1].fila == matrizLocal[indice].fila)){
                if( matrizLocal[indice+1].color == 3){
                    arbolesContagiando+=1;}
                if(matrizLocal[indice+1].fila < tamanioMatriz){
                    vecinosVisitados+=1;
            }

             }
        }

            if(indice<filasXproceso-2){
           if((matrizLocal[indice+2].fila == matrizLocal[indice].fila) ){
                if(matrizLocal[indice+2].color == 3){
                    arbolesContagiando+=1;
               }
                if(matrizLocal[indice+2].fila < tamanioMatriz){
                    vecinosVisitados+=1;
               }

            }
        }

        if(indice<tamanioMatriz){ /// caso comunicarse con las filas recibidas si es la primera fila
            if(matrizLocal[indice].fila-1 == arregloArriba[indice+tamanioMatriz].fila){
                if(arregloArriba[indice+tamanioMatriz].color == 3){
                    arbolesContagiando++;
                }
                if(arregloArriba[indice+tamanioMatriz].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-1 == arregloArriba[indice+(tamanioMatriz+1)].fila) && indice<tamanioMatriz-1){
                if(arregloArriba[indice+(tamanioMatriz+1)].color == 3){
                    arbolesContagiando++;
                }
                if(arregloArriba[indice+(tamanioMatriz+1)].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-1 == arregloArriba[indice+(tamanioMatriz-1)].fila)){
                if(arregloArriba[indice+(tamanioMatriz-1)].color == 3){
                    arbolesContagiando++;
                }
                if(arregloArriba[indice+(tamanioMatriz-1)].fila >= 0){
                    vecinosVisitados++;
                }
            }

             if((matrizLocal[indice].fila-2 == arregloArriba[indice].fila)){
                if(arregloArriba[indice].color == 3){
                    arbolesContagiando++;
                }
                if(arregloArriba[indice].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila+1 == matrizLocal[indice + tamanioMatriz].fila)){
                if(matrizLocal[indice + tamanioMatriz].color == 3){
                    arbolesContagiando++;
                }
                if(matrizLocal[indice + tamanioMatriz].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila+1 == matrizLocal[indice + (tamanioMatriz-1)].fila)){
                if(matrizLocal[indice + (tamanioMatriz-1)].color == 3){
                    arbolesContagiando++;
                }
                if(matrizLocal[indice + (tamanioMatriz-1)].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila+1 == matrizLocal[indice + (tamanioMatriz+1)].fila)){
                if(matrizLocal[indice + (tamanioMatriz+1)].color == 3){
                    arbolesContagiando++;
                }
                if(matrizLocal[indice + (tamanioMatriz+1)].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila+2 == matrizLocal[indice + (2*tamanioMatriz)].fila)){
                if(matrizLocal[indice + (2*tamanioMatriz)].color == 3){
                    arbolesContagiando++;
                }
                if(matrizLocal[indice + (2*tamanioMatriz)].fila >= 0){
                    vecinosVisitados++;
                }
            }


        }

        if(indice > tamanioMatriz-1 && indice < tamanioMatriz*2){ ///caso comunicacion si es la segunda fila
		if(matrizLocal[indice].fila -2 == arregloArriba[indice].fila){
                if(arregloArriba[indice].color == 3){
                    arbolesContagiando++;
                }
                if(arregloArriba[indice].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila+1 == matrizLocal[indice + tamanioMatriz].fila)){
                if(matrizLocal[indice + tamanioMatriz].color == 3){
                    arbolesContagiando++;
                }
                if(matrizLocal[indice + tamanioMatriz].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila+1 == matrizLocal[indice + (tamanioMatriz-1)].fila)){
                if(matrizLocal[indice + (tamanioMatriz-1)].color == 3){
                    arbolesContagiando++;
                }
                if(matrizLocal[indice + (tamanioMatriz-1)].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila+1 == matrizLocal[indice + (tamanioMatriz+1)].fila)){
                if(matrizLocal[indice + (tamanioMatriz+1)].color == 3){
                    arbolesContagiando++;
                }
                if(matrizLocal[indice + (tamanioMatriz+1)].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-1 == matrizLocal[indice - tamanioMatriz].fila)){
                if(matrizLocal[indice - tamanioMatriz].color == 3){
                    arbolesContagiando++;
                }
                if(matrizLocal[indice - tamanioMatriz].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-1 == matrizLocal[indice - (tamanioMatriz-1)].fila)){
                if(matrizLocal[indice - (tamanioMatriz-1)].color == 3){
                    arbolesContagiando++;
                }
                if(matrizLocal[indice - (tamanioMatriz-1)].fila >= 0){
                    vecinosVisitados++;
                }
            }

            if((indice-(tamanioMatriz+1))>= 0 && (matrizLocal[indice].fila-1 == matrizLocal[indice - (tamanioMatriz+1)].fila)){
                if(matrizLocal[indice - (tamanioMatriz+1)].color == 3){
                    arbolesContagiando++;
                }
                if(matrizLocal[indice - (tamanioMatriz+1)].fila >= 0){
                    vecinosVisitados++;
                }
            }

        }

        /// casos generales
        if((indice >= tamanioMatriz*2) && (indice< filasXproceso-(2*tamanioMatriz)) ){
        if(matrizLocal[indice].fila +1 == matrizLocal[indice + tamanioMatriz].fila){                
                if(matrizLocal[indice + tamanioMatriz].color == 3){
                    arbolesContagiando++;
                }
                if(matrizLocal[indice + tamanioMatriz].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila+1 == matrizLocal[indice + (tamanioMatriz +1)].fila) ){               
                if(matrizLocal[indice + (tamanioMatriz +1)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice + (tamanioMatriz +1)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila+1 == matrizLocal[indice + (tamanioMatriz -1)].fila)){
                if(matrizLocal[indice + (tamanioMatriz -1)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice + (tamanioMatriz -1)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila+2 == matrizLocal[indice + (2*tamanioMatriz)].fila)){
                if(matrizLocal[indice + (2*tamanioMatriz)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice + (2*tamanioMatriz)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-1 == matrizLocal[indice-tamanioMatriz].fila)){
                if(matrizLocal[indice-tamanioMatriz].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice-tamanioMatriz].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-1 == matrizLocal[indice-(tamanioMatriz+1)].fila)){
                if(matrizLocal[indice-(tamanioMatriz+1)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice-(tamanioMatriz+1)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-1 == matrizLocal[indice-(tamanioMatriz-1)].fila)){
                if(matrizLocal[indice-(tamanioMatriz-1)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice-(tamanioMatriz-1)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-2 == matrizLocal[indice-(2*tamanioMatriz)].fila)){
                if(matrizLocal[indice-(2*tamanioMatriz)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice-(2*tamanioMatriz)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }
        
        }

        if(indice >= (filasXproceso - tamanioMatriz*2) && indice < (filasXproceso - tamanioMatriz)){ ///caso ante ultima fila
		if(matrizLocal[indice].fila+2 == arregloAbajo[indiceAbajo].fila){
                if(arregloAbajo[indiceAbajo].color == 3){
                    arbolesContagiando++;
                }
                if(arregloAbajo[indiceAbajo].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if(matrizLocal[indice].fila +1 == matrizLocal[indice + tamanioMatriz].fila){
                if(matrizLocal[indice + tamanioMatriz].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice + tamanioMatriz].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }
    
            if((matrizLocal[indice].fila +1 == matrizLocal[indice + (tamanioMatriz+1)].fila)){
                if(matrizLocal[indice + tamanioMatriz].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice + tamanioMatriz].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila +1 == matrizLocal[indice + (tamanioMatriz-1)].fila)){
                if(matrizLocal[indice + (tamanioMatriz-1)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice + (tamanioMatriz-1)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila -1 == matrizLocal[indice - (tamanioMatriz)].fila)){
                if(matrizLocal[indice - (tamanioMatriz)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice - (tamanioMatriz)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila -1 == matrizLocal[indice - (tamanioMatriz+1)].fila)){
                if(matrizLocal[indice - (tamanioMatriz+1)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice - (tamanioMatriz+1)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila -1 == matrizLocal[indice - (tamanioMatriz-1)].fila)){
                if(matrizLocal[indice - (tamanioMatriz-1)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice - (tamanioMatriz-1)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila -2 == matrizLocal[indice - (tamanioMatriz*2)].fila)){
                if(matrizLocal[indice - (tamanioMatriz*2)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice - (tamanioMatriz*2)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }
        indiceAbajoB++;
        }
        /// caso ultima fila
        if(indice >= (filasXproceso  - tamanioMatriz) && indice < filasXproceso ){
            if(indiceAbajoB == tamanioMatriz){
                indiceAbajoB=0;
            }

            if(matrizLocal[indice].fila +1 == arregloAbajo[indiceAbajoB].fila){               
                if(arregloAbajo[indiceAbajoB].color == 3){
                    arbolesContagiando++;
                }
                if(arregloAbajo[indiceAbajoB].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if(indiceAbajoB>0){
            if((matrizLocal[indice].fila+1 == arregloAbajo[indiceAbajoB-1].fila)){
                if(arregloAbajo[indiceAbajoB-1].color == 3){
                    arbolesContagiando++;
                }

                if(arregloAbajo[indiceAbajoB-1].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
                }
            }

            if((matrizLocal[indice].fila+1 == arregloAbajo[indiceAbajoB+1].fila)){
                if(arregloAbajo[indiceAbajoB+1].color == 3){
                    arbolesContagiando++;
                }

                if(arregloAbajo[indiceAbajoB+1].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila+2 == arregloAbajo[indiceAbajoB+tamanioMatriz].fila)){
                if(arregloAbajo[indiceAbajoB+tamanioMatriz].color == 3){
                    arbolesContagiando++;
                }

                if(arregloAbajo[indiceAbajoB+tamanioMatriz].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-1 == matrizLocal[indice-tamanioMatriz].fila)){
                if(matrizLocal[indice-tamanioMatriz].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice-tamanioMatriz].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-1 == matrizLocal[indice-(tamanioMatriz+1)].fila)){
                if(matrizLocal[indice-(tamanioMatriz+1)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice-(tamanioMatriz+1)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-1 == matrizLocal[indice-(tamanioMatriz-1)].fila)){
                if(matrizLocal[indice-(tamanioMatriz-1)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice-(tamanioMatriz-1)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

            if((matrizLocal[indice].fila-2 == matrizLocal[indice-(2*tamanioMatriz)].fila)){
                if(matrizLocal[indice-(2*tamanioMatriz)].color == 3){
                    arbolesContagiando++;
                }

                if(matrizLocal[indice-(2*tamanioMatriz)].fila < tamanioMatriz){
                    vecinosVisitados++;
                }
            }

        indiceAbajoB++;
        }

        ///calculo de susceptibilidad
    if(matrizLocal[indice].edad < 157 ){
        if(matrizLocal[indice].heridas ==1){
        susceptibilidad =  0.35 + 0.15;
    }else{
        susceptibilidad = 0.35;

     }
    }

    if((matrizLocal[indice].edad >= 157 && matrizLocal[indice].edad < 1821)){
        if(matrizLocal[indice].heridas ==1){
        susceptibilidad =  0.17 + 0.15;

    }else{
        susceptibilidad = 0.17;

    }
    }

    if((matrizLocal[indice].edad >= 1821)){
        if(matrizLocal[indice].heridas ==1){
        susceptibilidad= 0.63 + 0.15;
    }else{
        susceptibilidad = 0.63;


    }
    }

    ///fin calculo de susceptibilidad

    ///aumento edad del arbol
    matrizLocalAux[indice].edad+=1;

    ///aumento la cantidad de semanas de infectado de los arboles
    if(matrizLocal[indice].color == 4){
        matrizLocalAux[indice].semanasInfectado+=1;
        matrizLocalAux[indice].semanasTotales++;
    }
    ///cambio de estado de enfermo sin sintomas a enfermo con sintomas

    if(matrizLocal[indice].semanasInfectado == 3){
        matrizLocalAux[indice].color =3;  /// el arbol luego de 3 semanas pasa a contagiar
    }
    ///cambio de estado de enfermo con sintomas a arbol con tratamiento
    if(matrizLocal[indice].color == 3){
        matrizLocalAux[indice].semanasTotales++;
        if((rand()%100)<=85){
            matrizLocalAux[indice].color =2;
            matrizLocalAux[indice].semanasInfectado=0;
            matrizLocalAux[indice].semanasTotales+=1;
        }
    }

    ///cambio de estado para arboles con tratamiento

    if(matrizLocal[indice].color == 2){
        if(matrizLocal[indice].semanasTotales <= 8){
            matrizLocalAux[indice].semanasTotales++;
        }else{

            if(matrizLocal[indice].edad < 157){
               if(rand()%100 > 3){
                matrizLocalAux[indice].color = 5;
                matrizLocalAux[indice].semanasTotales=0;
            }else{
                matrizLocalAux[indice].color = 1;
                matrizLocalAux[indice].semanasTotales=0;
            }
            }

            if((matrizLocal[indice].edad >= 157 && matrizLocal[indice].edad <1821)){
                if(rand()%100 > 15){
                matrizLocalAux[indice].color = 5;
                matrizLocalAux[indice].semanasTotales=0;
            }else{
                matrizLocalAux[indice].color = 1;
                matrizLocalAux[indice].semanasTotales=0;
            }
            }

            if(matrizLocal[indice].edad >= 1821){
                if(rand()%100 > 53){
                matrizLocalAux[indice].color = 5;
                matrizLocalAux[indice].semanasTotales=0;
            }else{
                matrizLocalAux[indice].color = 5;
                matrizLocalAux[indice].semanasTotales =0;
                matrizLocalAux[indice].edad=57;
            }
            }


        }
    }

    /// cambio de estado de podado a sano

    if(matrizLocal[indice].color == 1){
        if(matrizLocal[indice].semanasPodado<=12){
            matrizLocalAux[indice].semanasPodado++;
        }else{
            matrizLocalAux[indice].color = 5;
            matrizLocalAux[indice].semanasPodado=0;

        }
    }


    ///calculo cambio de estado
    if(matrizLocal[indice].color == 5){
         porcentajeEnfermos = (float)(arbolesContagiando/vecinosVisitados);
         probabilidadContagio = (float)(porcentajeEnfermos + susceptibilidad) *0.60 +0.07;
         if(probabilidadContagio < ((rand()%1001)/1000.f)){
            matrizLocalAux[indice].color=4;
         }
    }


    if(semana > 0){
        probHeridas = rand() % 101;

        if((matrizLocal[indice].edad < 157) && probHeridas <23){
            matrizLocalAux[indice].heridas = 1;
        }else{
            matrizLocalAux[indice].heridas =0;
        }

        if((matrizLocal[indice].edad >= 157 && matrizLocal[indice].edad < 1821) && (probHeridas <8 )){
            matrizLocalAux[indice].heridas = 1;
        }else{
            matrizLocalAux[indice].heridas =0;
        }

        if((matrizLocal[indice].edad >= 1821) && (probHeridas <37 )){
            matrizLocalAux[indice].heridas = 1;
        }else{
            matrizLocalAux[indice].heridas =0;
        }



    } /// fin para el random de heridas abiertas




} /// fin for para explorar vecinos

MPI_Barrier(MPI_COMM_WORLD);
MPI_Gather(matrizLocalAux,(filasXproceso*sizeof(arbol)),MPI_BYTE, matrizCampo,(filasXproceso*sizeof(arbol)),MPI_BYTE,0,MPI_COMM_WORLD );

}/// fin for semanas


tiempoFinal = clock();
double segundos = (double)(tiempoFinal-tiempoInicial) / CLOCKS_PER_SEC;
printf("EL TIEMPO DE EJECUCION DE LA VUELTA  %d FUE: %f\n",ejecuciones,segundos);
tiempototal += (tiempoFinal-tiempoInicial);
fila =-1;
free((void*)matrizCampo);
}
printf("El tiempo promedio total fue: %f\n", (double)(tiempototal/5)/CLOCKS_PER_SEC);

free((void*)matrizLocal);
free((void*)matrizLocalAux);
free((void*)arregloAbajo);
free((void*)arregloArriba);
free((void*)arregloAuxA);
free((void*)arregloAuxB);
MPI_Finalize();


}/// fin main









