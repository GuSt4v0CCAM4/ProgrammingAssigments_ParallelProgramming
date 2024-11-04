/*fuente del codigo de listas enlazadas que utilzian read-write lock para su modificaicon:*/
/* https://rajind.dev/2015/08/25/linked-list-with-read-write-locks-for-the-entire-linked-list/*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

const int MAX_KEY = 65536;
const int MAX_THREADS = 1024;

struct node {
    int data;
    struct node* next;
};

struct node* head = NULL;

int n = 1000;
int m = 10000;
float mMember = 0.50;
float mInsert = 0.25;
float mDelete = 0.25;
int thread_count = 1;

double start_time, finish_time, time_elapsed;
int member_count = 0;
int insert_count = 0;
int delete_count = 0;

pthread_mutex_t rw_mutex;
pthread_cond_t cond_readers;
pthread_cond_t cond_writers;

int readers = 0;
int writers = 0;
int waiting_writers = 0;

int Member(int value);
int Insert(int value);
int Delete(int value);
void Clear_Memory(void);
int Is_Empty(void);
void* Thread_Function(void* rank);

void ReadLock(int prefer_readers) {
    pthread_mutex_lock(&rw_mutex);
    if (prefer_readers) {
        //  lectores
        while (writers > 0) {
            pthread_cond_wait(&cond_readers, &rw_mutex);
        }
    } else {
        //  escritores
        while (writers > 0 || waiting_writers > 0) {
            pthread_cond_wait(&cond_readers, &rw_mutex);
        }
    }
    readers++;
    pthread_mutex_unlock(&rw_mutex);
}

void ReadUnlock() {
    pthread_mutex_lock(&rw_mutex);
    readers--;
    if (readers == 0) {
        pthread_cond_signal(&cond_writers);
    }
    pthread_mutex_unlock(&rw_mutex);
}

void WriteLock() {
    pthread_mutex_lock(&rw_mutex);
    waiting_writers++;
    while (readers > 0 || writers > 0) {
        pthread_cond_wait(&cond_writers, &rw_mutex);
    }
    waiting_writers--;
    writers++;
    pthread_mutex_unlock(&rw_mutex);
}

void WriteUnlock() {
    pthread_mutex_lock(&rw_mutex);
    writers--;
    if (waiting_writers > 0) {
        pthread_cond_signal(&cond_writers);
    } else {
        pthread_cond_broadcast(&cond_readers);
    }
    pthread_mutex_unlock(&rw_mutex);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <número de hilos> <preferencia: 0=escritores, 1=lectores>\n", argv[0]);
        exit(0);
    }

    thread_count = strtol(argv[1], NULL, 10);
    int prefer_readers = strtol(argv[2], NULL, 10);

    if (thread_count <= 0 || thread_count > MAX_THREADS) {
        fprintf(stderr, "El número de hilos debe ser menor o igual a %d\n", MAX_THREADS);
        exit(0);
    }

    int i = 0;
    for (; i < n; i++) {
        int r = rand() % 65536;
        if (!Insert(r)) {
            i--;
        }
    }

    pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));
    pthread_mutex_init(&rw_mutex, NULL);
    pthread_cond_init(&cond_readers, NULL);
    pthread_cond_init(&cond_writers, NULL);

    start_time = clock();
    for (i = 0; i < thread_count; i++) {
        pthread_create(&thread_handles[i], NULL, Thread_Function, (void*)&prefer_readers);
    }
    for (i = 0; i < thread_count; i++) {
        pthread_join(thread_handles[i], NULL);
    }

    finish_time = clock();
    time_elapsed = (finish_time - start_time) / CLOCKS_PER_SEC;
    printf("Tiempo transcurrido = %.10f segundos\n", time_elapsed);

    Clear_Memory();
    pthread_mutex_destroy(&rw_mutex);
    pthread_cond_destroy(&cond_readers);
    pthread_cond_destroy(&cond_writers);
    free(thread_handles);

    return 0;
}

int Member(int value) {
    struct node* temp = head;
    while (temp != NULL && temp->data < value) {
        temp = temp->next;
    }
    return (temp != NULL && temp->data == value);
}

int Insert(int value) {
    struct node* current = head;
    struct node* pred = NULL;
    struct node* temp;
    int return_value = 1;

    while (current != NULL && current->data < value) {
        pred = current;
        current = current->next;
    }

    if (current == NULL || current->data > value) {
        temp = malloc(sizeof(struct node));
        temp->data = value;
        temp->next = current;
        if (pred == NULL) {
            head = temp;
        } else {
            pred->next = temp;
        }
    } else {
        return_value = 0;
    }
    return return_value;
}

int Delete(int value) {
    struct node* current = head;
    struct node* pred = NULL;
    int return_value = 1;

    while (current != NULL && current->data < value) {
        pred = current;
        current = current->next;
    }

    if (current != NULL && current->data == value) {
        if (pred == NULL) {
            head = current->next;
        } else {
            pred->next = current->next;
        }
        free(current);
    } else {
        return_value = 0;
    }
    return return_value;
}

void Clear_Memory(void) {
    struct node* current = head;
    struct node* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

void* Thread_Function(void* rank) {
    int i, val;
    int my_member = 0;
    int my_insert = 0;
    int my_delete = 0;
    int ops_per_thread = m / thread_count;
    int prefer_readers = *((int*)rank);

    for (i = 0; i < ops_per_thread; i++) {
        float operation_choice = (rand() % 10000) / 10000.0;
        val = rand() % MAX_KEY;

        if (operation_choice < mMember) {
            ReadLock(prefer_readers);
            Member(val);
            ReadUnlock();
            my_member++;
        } else if (operation_choice < mMember + mInsert) {
            WriteLock();
            Insert(val);
            WriteUnlock();
            my_insert++;
        } else {
            WriteLock();
            Delete(val);
            WriteUnlock();
            my_delete++;
        }
    }

    pthread_mutex_lock(&rw_mutex);
    member_count += my_member;
    insert_count += my_insert;
    delete_count += my_delete;
    pthread_mutex_unlock(&rw_mutex);

    return NULL;
}
/* lectores1*/