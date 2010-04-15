#include <stdio.h>
#include <string.h>

typedef struct {
  void *state;
  long current;
} coroutine_state;


coroutine_state* createCoroutineState() {
  coroutine_state *s = (coroutine_state*) malloc(sizeof (coroutine_state));
  bzero(s, sizeof(coroutine_state));
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

#define yield(f)                                                              \
  s->current = __LINE__; return f; case __LINE__:

#define finalizeCoroutine() }



/* ************************************************************************* */

typedef struct {
  int a;
  int b; 
} fib_state;

int fibs(coroutine_state *s) {
  initializeCoroutineState(s,fib_state, mystate); 

  mystate->a = 0;
  mystate->b = 1;

  while(1) {
    yield(mystate->a + mystate->b);
    int f = mystate->a;
    mystate->a = mystate->b;
    mystate->b += f;
  }
  finalizeCoroutine();
}

int main() {
  int i;
  coroutine_state *s = createCoroutineState();
  for( i = 0; i < 20; i++) {
    printf("%d: %d\n", i, fibs(s));
  }
  return 0;
}
