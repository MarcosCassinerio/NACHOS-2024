/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2007-2009 Universidad de Las Palmas de Gran Canaria.
///               2016-2021 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "thread_test_prod_cons.hh"
#include "system.hh"


unsigned amount = 0;
SynchList<char*> *lista = new SynchList<char*>;
Lock *lock = new Lock("Lock");
Condition *conditionProd = new Condition("Condition Prod", lock);
static bool done[PRODS + CONS];



static void
Consumer(void *n_) 
{
    unsigned *n = (unsigned *) n_;
    for (unsigned i = 0; i < MAX_ITERS; i++) {
        lock->Acquire();

        if (amount == 0)
            printf("Consumidor %d esperando (buffer vacio)\n", *n);

        lock->Release();

        char* produce = lista->Pop();

        lock->Acquire();
        conditionProd->Signal();
        printf("Consumidor %d consume: %s\n", *n, produce);
        amount -= 1;

        lock->Release();
    }
    
    printf("Consumidor %d termina\n", *n);
    done[*n] = true;
    delete n;
}
static void
Producer(void *n_)
{
    unsigned *n = (unsigned *) n_;
    for (unsigned i = 0; i < MAX_ITERS; i++) {
        lock->Acquire();
        
        if (amount == MAX_AMOUNT)
            printf("Productor %d esperando (buffer lleno)\n", *n);

        while (amount == MAX_AMOUNT)
            conditionProd->Wait();

        char* produce = new char[64];
        sprintf(produce, "%d_%d", *n, i);
        lista->Append(produce);
        amount += 1;
        printf("Productor %d produce: %s en %d\n", *n, produce, amount);

        lock->Release();
    }
    
    
    printf("Productor %d termina\n", *n);
    done[CONS + (*n)] = true;
    delete n;
}

void
ThreadTestProdCons()
{
    for (unsigned i = 0; i < PRODS; i++) {
        done[i] = false;
        unsigned *pos = new unsigned();
        *pos = i;
        char *name = new char [64];
        sprintf(name, "Producer %d", i + 1);
        Thread *newThread = new Thread(name);
        newThread->Fork(Producer, pos);
    }
    
    for (unsigned i = 0; i < CONS; i++) {
        done[i + PRODS] = false;
        unsigned *pos = new unsigned();
        *pos = i;
        char *name = new char [64];
        sprintf(name, "Consumer %d", i + 1);
        Thread *newThread = new Thread(name);
        newThread->Fork(Consumer, pos);
    }

    for (unsigned i = 0; i < PRODS + CONS; i++) {
        while (!done[i]) {
            currentThread->Yield();
        }
    }
}
