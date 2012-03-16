#include <stdio.h>
#include "sort.h"

void quick_sort(void **array, int l, int r, int (*cmp)(void *lv, void *rv))
{
   int i, j;
   void *pivot, *tmp;

   if (l >= r) return;

   pivot = array[l];
   i = l - 1; j = r + 1;
   while (i < j) {

      while (cmp(array[++i], pivot) < 0);
      while (cmp(array[--j], pivot) > 0);

      if (i < j) {
         tmp = array[i];
         array[i] = array[j];
         array[j] = tmp;
      }
   }
   quick_sort(array, l, j, cmp);
   quick_sort(array, j+1, r, cmp);
}

int binary_search(void **array, int l, int r, void *key, int (*cmp)(void *lv, void *rv))
{
  int res, c;

  while (l <= r) {
    c = (l+r)/2;
    res = cmp(key, array[c]);
    if (res == 0)
      return c; /* data found */
    if (res > 0)
      l = c+1;
    else
      r = c-1;
  }

  return -1; /* data not found */
}
