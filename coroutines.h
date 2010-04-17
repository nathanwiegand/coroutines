#ifndef __COROUTINES_H__
#define __COROUTINES_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CoroutineState CoroutineState;
struct CoroutineState {
  void *state;
  long current;
  unsigned char done;
  CoroutineState *next;
};


CoroutineState* createCoroutineState() {
  CoroutineState *s = (CoroutineState*) malloc(sizeof (CoroutineState));
  bzero(s, sizeof(CoroutineState));
  return s;
}

CoroutineState *getTail(CoroutineState* s) {
  while(s->next) s = s->next;
  return s;
}

#define initializeCoroutineState(coroutineState, UserStateType,userStateVar)  \
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
    coroutineState->next = createCoroutineState();                            \
    while(!coroutineState->next->done) {                                      \
      typeof(func) tmp =  func;                                               \
      coroutineState->current = __LINE__;                                     \
      if(!getTail(coroutineState)->done)                                      \
        return tmp;                                                           \
      case __LINE__: 1;                                                       \
    }                                                                         \
  }while(0);                                                                  \

#define yield(coroutineState, value)                                          \
  do{                                                                         \
    coroutineState->current = __LINE__;                                       \
    coroutineState->next = 0;                                                 \
    return value;                                                             \
    case __LINE__: 1;                                                         \
  } while(0);                                                                 

#define finalizeCoroutine(s) } s->done = 1; s->next = 0;


#endif /*__COROUTINES_H__ */
