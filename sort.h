#ifndef _SORT_INCLUDED
#define _SORT_INCLUDED

void quick_sort(void **array, int l, int r, int (*cmp)(void *lv, void *rv));
int binary_search(void **array, int l, int r, void *key, int (*cmp)(void *lv, void *rv));

#endif
