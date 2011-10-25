#ifndef __SIMPLESEMAPHORE_H_
#include <SimpleSemaphore.h>
#define __SIMPLESEMAPHORE_H_
#endif

#ifndef __SHAREDMEMORY_H_
#include <SharedMemory.h>
#define __SHAREDMEMORY_H_
#endif

#include <iostream>
#include <time.h>
#include <stdlib.h>

using std::cout;
using std::endl;

int main(int argc, char *argv[]) 
{
	SimpleSemaphore Turno("/sem_turno",0);
  	//SimpleSemaphore Carta("/sem_res",52);
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
	turno = 0; 
	if(numJugadores==0)
	{	
		cout<<"Me estoy registrando, jugador 1"<<endl;
		Jugador1.Signal();
	}else if(numJugadores=1)
	{
		cout<<"Me estoy registrando, jugador 2"<<endl;
		Jugador2.Signal();
	}
	
	for(int i=0;i<10;i++)
	{	
		sleep(5);
		if(turno==0)
		{
			Jugador1.Wait();
			//CartaJugador1=10;
			srand(time(NULL));    
		        CartaJugador1 = rand()%14;
			//Turno.Wait();
			cout<<"Soy 1 y mi carta es: "<<CartaJugador1<<endl;
			turno=1;
		}else 
		{
			Jugador2.Wait();
                        srand(time(NULL));    
			CartaJugador2 = rand()%14;
			//Turno.Wait();
			cout<<"Soy 2 y mi carta es: "<<CartaJugador2<<endl;
			turno = 0;
		}
	}
	
	return 0;
}
