#include <stdbool.h>

/* locks and unlocks the LSB of the arg (of type long)
   for little-endian machines like Intel and ARM */

void s_lock(volatile bool *l)
{
  while (__atomic_test_and_set(l, __ATOMIC_ACQUIRE));
}

void s_unlock(volatile bool* l)
{
  __atomic_clear(l, __ATOMIC_RELEASE);
}
