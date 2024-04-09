/// Routines for synchronizing threads.
///
/// The implementation for this primitive does not come with base Nachos.
/// It is left to the student.
///
/// When implementing this module, keep in mind that any implementation of a
/// synchronization routine needs some primitive atomic operation.  The
/// semaphore implementation, for example, disables interrupts in order to
/// achieve this; another way could be leveraging an already existing
/// primitive.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2021 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "condition.hh"
#include "system.hh"


/// Dummy functions -- so we can compile our later assignments.
///

Condition::Condition(const char *debugName, Lock *conditionLock)
{
    name = debugName;
    lock = conditionLock;
    semaphoreList = new List<Semaphore*>();
}

Condition::~Condition()
{
    delete semaphoreList;
}

const char *
Condition::GetName() const
{
    return name;
}

void
Condition::Wait()
{
    DEBUG('s', "Thread \"%s\" is Waiting\n", currentThread->GetName());
    ASSERT(lock->IsHeldByCurrentThread());

    Semaphore *waitSemaphore = new Semaphore("Wait Semaphore", 0);
    semaphoreList->Append(waitSemaphore);
    lock->Release();

    waitSemaphore->P();

    lock->Acquire();
}

void
Condition::Signal()
{
    DEBUG('s', "Thread \"%s\" is doing Signal\n", currentThread->GetName());
    ASSERT(lock->IsHeldByCurrentThread());

    Semaphore *currentSemaphore = semaphoreList->Pop();

    if (currentSemaphore != nullptr)
        currentSemaphore->V();
}

void
Condition::Broadcast()
{
    DEBUG('s', "Thread \"%s\" is doing Broadcast\n", currentThread->GetName());
    ASSERT(lock->IsHeldByCurrentThread());

    while (!semaphoreList->IsEmpty()) {
        Semaphore *currentSemaphore = semaphoreList->Pop();
        currentSemaphore->V();
    }
}
