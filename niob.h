#ifndef niob_h
#define niob_h

void niob_init();

char *niob_eval(char *text);

void niob_def(char *key, char *(*cmd)(), char *body);

void niob_set(char *key, char *value);

char *niob_get(char *key);

void niob_del(char *key);

#endif
