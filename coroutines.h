#ifndef __COROUTINES_H__
#define __COROUTINES_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CoroutineState CoroutineState;
struct CoroutineState {
  void *state;            /* Pointer to a struct representing the internal state
                             of the coroutine.  We need this since we have to 
                             reinstate the scope whenever the function is 
                             re-intered.                                      */
  long current;           /* The line number of where the coroutine should
                             return.  Used in 'yield' and 'recur'             */
  unsigned char done;     /* A marker to show that a recurrence is completed  */
  CoroutineState *next;   /* The next in the stack of states                  */
  CoroutineState *parent; /* NULL if actually the parent, pointer otherwise   */
  CoroutineState *tail;   /* Only the parent is guaranteed to know the tail   */
};


CoroutineState* createCoroutineState() {
  CoroutineState *s = (CoroutineState*) malloc(sizeof (CoroutineState));
  bzero(s, sizeof(CoroutineState));
  return s;
}

CoroutineState *getTail(CoroutineState* s) {
  return s->parent ? s->parent->tail : s->tail;
}

#define initializeCoroutineState(coroutineState, UserStateType, userStateVar) \
  UserStateType *userStateVar = (UserStateType*) coroutineState->state;       \
  switch(coroutineState->current) {                                           \
  case 0:                                                                     \
  if(coroutineState->state == 0) {                                            \
    coroutineState->state = (void*) malloc(sizeof (UserStateType));           \
    bzero(coroutineState->state, sizeof(UserStateType));                      \
  }                                                                           \
  userStateVar = (UserStateType*) coroutineState->state;                      \

#define recur(coroutineState,func)                                            \
  do{                                                                         \
    CoroutineState *parent = coroutineState ? coroutineState                  \
        : coroutineState->parent;                                             \
    coroutineState->next = createCoroutineState();                            \
    coroutineState->next->parent = parent;                                    \
    parent->tail = coroutineState->next;                                      \
    while(!coroutineState->next->done) {                                      \
      CoroutineState *parent = coroutineState ? coroutineState                \
          : coroutineState->parent;                                           \
      typeof(func) tmp =  func;                                               \
      coroutineState->current = __LINE__;                                     \
      if(!parent->tail->done)                                                 \
        return tmp;                                                           \
      case __LINE__: 1;                                                       \
    }                                                                         \
  } while(0);                                                                 \

#define yield(coroutineState, value)                                          \
  do{                                                                         \
    CoroutineState *parent = coroutineState ? coroutineState                  \
        : coroutineState->parent;                                             \
    coroutineState->current = __LINE__;                                       \
    parent->tail = coroutineState;                                            \
    free(coroutineState->next);                                               \
    coroutineState->next = 0;                                                 \
    return value;                                                             \
    case __LINE__: 1;                                                         \
  } while(0);                                                                 

#define finalizeCoroutine(s) } s->done = 1; s->next = 0;

#endif /*__COROUTINES_H__ */
