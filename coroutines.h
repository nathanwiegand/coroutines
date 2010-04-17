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

#define initializeCoroutineState(_st, str,my)                                 \
  str *my = (str*) _st->state;                                                \
  switch(_st->current) {                                                      \
  case 0:                                                                     \
  if(_st->state == 0) {                                                       \
    _st->state = (void*) malloc(sizeof (str));                                \
    bzero(_st->state, sizeof(str));                                           \
  }                                                                           \
  my = (str*) _st->state;

#define recur(s,f)                                                            \
  do{s->next = createCoroutineState();                                        \
  while(!s->next->done) {typeof(f) tmp =  f; s->current = __LINE__;           \
  if(!getTail(s)->done) return tmp; case __LINE__: 1;}}while(0);

#define yield(f)                                                              \
  do{s->current = __LINE__;s->next = 0;  return f; case __LINE__: 1;} while(0); 

#define finalizeCoroutine(s) } s->done = 1; s->next = 0;


#endif /*__COROUTINES_H__ */
