/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2007-2009 Universidad de Las Palmas de Gran Canaria.
///               2016-2021 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "thread_test_prod_cons.hh"
#include "system.hh"


unsigned amount = 0;
Lock *lock = new Lock("Lock");
Condition *conditionProd = new Condition("Condition Prod", lock);
Condition *conditionCons = new Condition("Condition Cons", lock);
static unsigned buffer[MAX_AMOUNT];
static bool done[PRODS + CONS];

static void
Consumer(void *n_) 
{
    unsigned *n = (unsigned *) n_;
    for (unsigned i = 0; i < MAX_ITERS; i++) {
        lock->Acquire();
        
        if (amount == 0)
            conditionCons->Wait();

        conditionProd->Signal();

        amount -= 1;

        printf("Consumer %d consumed\n", *n);

        lock->Release();
    }
    
    printf("Consumer %d finished\n", *n);
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
            conditionProd->Wait();
        
        conditionCons->Signal();

        amount += 1;

        printf("Productor %d produced\n", *n);

        lock->Release();
    }
    
    
    printf("Productor %d finished\n", *n);
    done[CONS + (*n)] = true;
    delete n;
}

void
ThreadTestProdCons()
{
    buffer = [];

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
