/*
 * =====================================================================================
 *
 *       Filename:  Jugador.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  13/05/11 12:30:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Carvajal 
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
#include <stdio.h> 
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

using namespace std;
int main()
{
	double turno;

	SharedMemory<Juego>memoria("/operativos");
	Juego &juego = memoria();

//	int cartas1[2][13]={{4,4,4,4,4,4,4,4,4,4,4,4,4}
				//	,{1,2,3,4,5,6,7,8,9,10,11,12,13}};

	int cartas2[2][13]={{4,4,4,4,4,4,4,4,4,4,4,4,4}
					,{13,12,11,10,9,8,7,6,5,4,3,2,1}};

	int cartas1[2][13]={{4,4,4,4,4,4,4,4,4,4,4,4,4}
					,{13,12,11,10,9,8,7,6,5,4,3,2,1}};

	SimpleSemaphore sem_jugar2("/parciall",0);
	SimpleSemaphore sem_jugar("/parcial",0);
	SimpleSemaphore sem_registro("/parcial2",0);
	SimpleSemaphore sem_empezar("/parcial3",0);
	SimpleSemaphore sem_juego("/parcial4",0);

	
	cout<<"matriz"<<endl;
	cout<<cartas1[1][1]<<endl;
	sem_registro.Signal();
	sem_juego.Wait();
	cout<<"Jugador  Vinculado"<<endl;

	//Empieza El Juego
		for (int i =0;i<52;i++)	{
			for(int b =0;b<4;b++){
	
			sem_jugar.Wait();
			cout<<"Jugador 1"<<endl;
			juego.pinta_jugador1 = cartas1[0][i];
			juego.valor_jugador1 = cartas1[1][i];
			

			sem_jugar2.Signal();
			sleep(0.2);

			sem_jugar2.Wait();
			cout<<"Jugador 2"<<endl;
			juego.pinta_jugador2 = cartas2[0][i];
			juego.valor_jugador2 = cartas2[1][i];
			if ((juego.pinta_jugador2 ==1 ||juego.pinta_jugador2 ==1) && juego.valor_jugador2 ==1 )
			{
				cin>>turno;
			}

			cartas2[0][i]--;
			cartas1[0][i]--;
		//	sem_jugar.Signal();
			sleep(0.2);
			}
		}
		
		//scanf(%d,turno);







return 0;
}
