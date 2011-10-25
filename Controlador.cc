/*
 * =====================================================================================
 *
 *       Filename:  Controlador.cc
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  13/05/11 12:31:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __SIMPLESEMAPHORE_H_
#include <SimpleSemaphore.h>
#define __SIMPLESEMAPHORE_H_
#endif

#ifndef __SHAREDMEMORY_H_
#include <SharedMemory.h>
#define __SHAREDMEMORY_H_
#endif

#include <iostream>

using namespace std;



struct Juego {
	int disponibilidad_jugador1;
	int disponibilidad_jugador2;
	int puntaje_jugador1;
	int puntaje_jugador2;
	int pinta_jugador1;
	int pinta_jugador2;
	int valor_jugador1;
	int valor_jugador2;


};


int main()
{
	SharedMemory<Juego>memoria("/operativos");
	Juego &juego = memoria();
	bool salir = false;


	cout<<"Entro Controlador"<<endl;
	SimpleSemaphore sem_jugar2("/parciall",0);
	SimpleSemaphore sem_jugar("/parcial",0);
	SimpleSemaphore sem_registro("/parcial2",0);
	SimpleSemaphore sem_empezar("/parcial3",0);
	SimpleSemaphore sem_juego("/parcial4",0);

	do {

			if (juego.disponibilidad_jugador1==1 && juego.disponibilidad_jugador2==1)
			{
				cout<<"Los Jugadores Ya estan Completos"<<endl;
				sleep(1);
			//	for (int i =0;i<2;i++){
				sem_jugar.Signal();
			//	sem_jugar.Wait();
			//	sem_jugar2.Signal();
			//	sem_jugar2.Wait();
				if(juego.valor_jugador1 == juego.valor_jugador2 )
				{
					sleep(1);
					cout<<"Empate"<<endl;
					cout<<"Valor Jugador 1: "<<juego.valor_jugador1 <<endl;
					cout<<"Valor jugador 2: "<<juego.valor_jugador2 <<endl;
					cout<<"Pinta jugador 1: "<<juego.pinta_jugador1 <<endl;
					cout<<"Pinta Jugador 2: "<<juego.pinta_jugador2 <<endl;
					sem_jugar.Signal();





				}else if (juego.valor_jugador1 < juego.valor_jugador2 )
				{
					sleep(0.5);
					cout <<"Gano Jugador 2"<<endl;
					juego.puntaje_jugador2++; 
					sem_jugar.Signal();


				}else if(juego.valor_jugador1 > juego.valor_jugador2)
				{
					sleep(0.5);
					cout <<"Gano Jugador 1"<<endl;
					juego.puntaje_jugador1++; 
					sem_jugar.Signal();


				}
			//	sem_jugar.Signal();
			//	salir = true;
			//	juego.disponibilidad_jugador1==0;

			}else{

			sem_registro.Wait();
			cout<<"Jugador 1 Registrado"<<endl;
			juego.disponibilidad_jugador1 = 1;
			sem_juego.Signal();

		 		if (juego.disponibilidad_jugador1==1){
					sem_registro.Wait();
					cout<<"Jugador 2 Registrado"<<endl;
					juego.disponibilidad_jugador2 = 1;
					sem_juego.Signal();
				}
			}
	
	//salir = true;

	}while(true);
	
	
	return 0;

	}

