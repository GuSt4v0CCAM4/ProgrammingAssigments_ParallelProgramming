#include <iostream>
#include <cstdlib>
#include <ctime>
#include <pthread.h>

using namespace std;

long long int tiros_circulo_global = 0;
long long int totalTiros;
int num_hilos;
pthread_mutex_t mutex;

struct ThreadData {
    long long int tiros_por_hilo;
    unsigned int seed;
};

void* monteCarlo(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long long int tiros_circulo_local = 0;

    for (long long int i = 0; i < data->tiros_por_hilo; ++i) {
        double x = (rand_r(&data->seed) / (double)RAND_MAX) * 2.0 - 1.0;
        double y = (rand_r(&data->seed) / (double)RAND_MAX) * 2.0 - 1.0;
        double distCuadrado = x * x + y * y;
        if (distCuadrado <= 1) {
            tiros_circulo_local++;
        }
    }

    pthread_mutex_lock(&mutex);
    tiros_circulo_global += tiros_circulo_local;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main() {
    cout << "Ingrese el número total de lanzamientos: ";
    cin >> totalTiros;
    cout << "Ingrese el número de hilos: ";
    cin >> num_hilos;

    pthread_mutex_init(&mutex, NULL);

    pthread_t threads[num_hilos];
    ThreadData thread_data[num_hilos];
    long long int tirosPorHilo = totalTiros / num_hilos;

    for (int i = 0; i < num_hilos; ++i) {
        thread_data[i].tiros_por_hilo = tirosPorHilo;
        thread_data[i].seed = time(0) + i;
        pthread_create(&threads[i], NULL, monteCarlo, (void*)&thread_data[i]);
    }

    for (int i = 0; i < num_hilos; ++i) {
        pthread_join(threads[i], NULL);
    }

    double pi = 4.0 * tiros_circulo_global / ((double)totalTiros);
    cout << "El Pi estimado es " << pi << endl;

    pthread_mutex_destroy(&mutex);

    return 0;
}
