#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdint.h>
#include <x86intrin.h>

using namespace std;

const double FRECUENCIA_CPU_MHZ = 3900.0;

void* emptyFunction(void* arg) {
    pthread_exit(NULL);
}

inline uint64_t leerContadorCiclos() {
    return __rdtsc();
}

double convertirCiclosAMilisegundos(uint64_t ciclos) {
    return (ciclos / FRECUENCIA_CPU_MHZ) / 1000.0;
}

int main() {
    int num_threads;
    cout << "Ingrese el número de hilos: ";
    cin >> num_threads;

    vector<pthread_t> threads(num_threads);
    uint64_t tiempo_total_ciclos = 0;

    for (int i = 0; i < num_threads; ++i) {
        uint64_t start = leerContadorCiclos();

        pthread_create(&threads[i], NULL, emptyFunction, NULL);
        pthread_join(threads[i], NULL);

        uint64_t end = leerContadorCiclos();
        uint64_t duracionCiclos = end - start;

        tiempo_total_ciclos += duracionCiclos;
        cout << "Hilo " << i << " duró " << convertirCiclosAMilisegundos(duracionCiclos) << " milisegundos" << endl;
    }

    double tiempo_promedio_ms = convertirCiclosAMilisegundos(tiempo_total_ciclos / num_threads);
    cout << "Tiempo promedio para crear y terminar un hilo: " << tiempo_promedio_ms << " milisegundos" << endl;

    return 0;
}
