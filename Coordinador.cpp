#ifndef __SIMPLESEMAPHORE_H_
#include <SimpleSemaphore.h>
#define __SIMPLESEMAPHORE_H_
#endif

#ifndef __SHAREDMEMORY_H_
#include <SharedMemory.h>
#define __SHAREDMEMORY_H_
#endif

#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char *argv[]) 
{
	SimpleSemaphore Turno("/sem_turno",0);
  	
	SimpleSemaphore Jugador1("/sem_jugador1",0);
	SimpleSemaphore Jugador2("/sem_jugador2",0);

	SharedMemory<int> sm("/mem");
	SharedMemory<int> sm1("/mem1");
	SharedMemory<int> sm2("/men2");
	SharedMemory<int> sm3("/men3");

  	int &CartaJugador1 = sm();
	int &CartaJugador2 = sm1();
	int &numJugadores = sm2(); 
	int &turno = sm3();

	
	
	int valorCarta1=0;
	int valorCarta2=0;
	int Puntaje1 = 0;
	int Puntaje2 = 0;
	int PuntajeAcum = 0;
	turno=0;
	
	cout<<"Esperando a los jugadores"<<endl;
	
	
	Jugador1.Wait();
	cout<<"Jugador 1 registrado"<<endl;
	numJugadores++;
	Jugador2.Wait();
	cout<<"Jugador 2 registrado"<<endl<<"Todos los jugadores registrados"<<endl;
	numJugadores++;
	
	for(int i=0;i<10;i++)
	{
	//if(turno==0)
	//{
		Jugador1.Signal();
		cout<<"Esperando la carta del jugador 1"<<endl;
		valorCarta1 = CartaJugador1;
		//Turno.Signal();
		
		
		sleep(3);
	//	turno++;
	//}else
	//if(turno==1)
	//{
		Jugador2.Signal();
		cout<<"Esperando la carta del jugador 2"<<endl;
		valorCarta2 = CartaJugador2;
		//Turno.Signal();
		
		sleep(3);
	//	turno--;
	//}
		
		cout<<"Carta jugador 1: "<<CartaJugador1<<" -- Carta jugador 2: "<<CartaJugador2<<endl;
		if(CartaJugador1<CartaJugador2)
		{
			cout<<"Jugador 2 carta mayor"<<endl;
			Puntaje1 = Puntaje1 + PuntajeAcum + 1;
			PuntajeAcum = 0 ;
		}else if(CartaJugador1>CartaJugador2)
		{	
			cout<<"Jugador 1 carta mayor"<<endl;
			Puntaje2 = Puntaje2 + PuntajeAcum + 1;
			PuntajeAcum = 0 ;
		}else
		{	
			cout<<"Empate"<<endl;
			PuntajeAcum = PuntajeAcum + 1; 
		}
	}
	
	if( Puntaje1 > Puntaje2){
		cout<<"Jugador 1 gana la partida con "<<Puntaje1<<endl;
		}else 	if( Puntaje2 > Puntaje1){
			cout<<"Jugador 2 gana la partida con "<<Puntaje2<<endl;
				}else {
					cout<<"La partida quedo empatada a "<<Puntaje1<<endl;
			}
	
	return 0;
}
