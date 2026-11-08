#include <stdlib.h>
#include <string.h>
/* Nur im Hash-Grenztest: Adressraum reservieren, ohne 40 GB RAM zu belegen. */
void *hash_test_malloc(size_t bytes);
void hash_test_free(void *pointer);
void *hash_test_memset(void *pointer, int value, size_t bytes);
#define malloc hash_test_malloc
#define free hash_test_free
#define memset hash_test_memset
