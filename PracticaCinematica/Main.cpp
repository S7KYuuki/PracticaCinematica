#include <iostream>
#include <Windows.h>
#include <string>
#include "Cohete.h"
#include "Misil.h"
#include "Constantes.h"

using std::cout;
using std::endl;
using std::cin;

bool colisiona(const Cohete &cohete, const Misil &misil);

void ClearScreen()
{
	if (system("CLS")) system("clear");
}

int main()
{
	float alturaCohete, velocidadCohete;
	float velocidadInicialMisil, anguloLanzamieto, tiempoLanzamiento;

	cout << "Altura del cohete a interceptar (m) = ";
	cin >> alturaCohete;

	cout << "Velocidad del cohete (m/s) = ";
	cin >> velocidadCohete;

	cout << "---------------------------------\n";

	cout << "Velocidad inicial del misil (m/s) = ";
	cin >> velocidadInicialMisil;

	cout << "Angulo de lanzamiento del misil (grados) = ";
	cin >> anguloLanzamieto;

	cout << "Instante de lanzamiento (s) (usar 0 si se quiere que el misil sea lanzado tan pronto se vea el cohete) = ";
	cin >> tiempoLanzamiento;

	Cohete *cohete = new Cohete(alturaCohete, velocidadCohete);
	Misil *misil = new Misil(velocidadInicialMisil, anguloLanzamieto, tiempoLanzamiento);
	printf("Colisiona = %d\n", colisiona(*cohete, *misil));
	system("pause");

	clock_t tiempoInicio = clock();
	while(!GetAsyncKeyState(VK_ESCAPE))
	{
		clock_t tiempoSimulacion = clock() - tiempoInicio;
		ClearScreen();

		printf("Esta simulacion es una aproximacion dado que se realiza en saltos de 1 aproximadamente segundo\nPulsa ESCAPE para omitir\n\n");
		cohete->setTiempo(tiempoSimulacion);
		misil->setTiempo(tiempoSimulacion);

		cohete->tick();
		misil->tick();

		printf("Tiempo transcurrido: %ldms (%ds)\n", tiempoSimulacion, tiempoSimulacion/1000);
		printf("COHETE\n");
		cohete->print();
		printf("---------------------------------------\n");
		printf("MISIL\n");
		misil->print();

		// TODO TEMP
		//if (misil->getPosicion()->getX() != 0 && misil->getPosicion()->getY() == 0 && tiempoSimulacion != 0) break;
		Sleep(1000);
	}
	printf("Hacer algo aqui :D\n");

	delete cohete;
	delete misil;
	return 0;
}

bool colisiona(const Cohete &cohete, const Misil &misil)
{
	// TODO a�adir la opcion del rozamiento..
	auto Vx = misil.getVelocidadInicial() * cos(misil.getAngulo() * constantes::PI / 180);
	auto Vy = misil.getVelocidadInicial() * sin(misil.getAngulo() * constantes::PI / 180);

	// 1� Calcular el tiempo en el que el misil alcanza la altura H del cohete
	// Como el movimiento del misil es una parabola, existen dos tiempos en los que la Y es igual a la altura. Cuando sube y cuando baja.
	auto tiempoColision1 = ((-Vy + sqrtf(powf(Vy, 2) - 2 * constantes::GRAVEDAD * cohete.getPosicion()->getY())) / -constantes::GRAVEDAD) + misil.getTiempoLanzamiento();
	auto tiempoColision2 = ((-Vy - sqrtf(powf(Vy, 2) - 2 * constantes::GRAVEDAD * cohete.getPosicion()->getY())) / -constantes::GRAVEDAD) + misil.getTiempoLanzamiento();

	printf("TIEMPO DE COLISION = %f\n", tiempoColision1);
	printf("TIEMPO DE COLISION = %f\n", tiempoColision2);

	// 2� Comprobar que las X son iguales
	int xCohete = cohete.getVelocidad() * tiempoColision1;
	int xMisil = Vx * (tiempoColision1 - misil.getTiempoLanzamiento());

	printf("Xc = %d\nXm = %d\n", xCohete, xMisil);
	// Supongamos un margen de error de 1 metro..
	if (abs(xCohete - xMisil) <= 1) return true;

	// Puede ser que el impacto se realice cuando el misil cae
	xCohete = cohete.getVelocidad() * tiempoColision2;
	xMisil = Vx * (tiempoColision2 - misil.getTiempoLanzamiento());
	printf("Xc = %d\nXm = %d\n", xCohete, xMisil);

	if (abs(xCohete - xMisil) <= 1) return true;

	// Se supone que no colisiona
	return false;
}