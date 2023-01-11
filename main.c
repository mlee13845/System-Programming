#include <stdio.h>
#include "mymalloc.h"
int main()
{
    myinit(0);
    printFreeList();
    char* a;
    a = mymalloc(4);
    printFreeList();
     char* b;
    b = mymalloc(17);
     printFreeList();
    printf("Address of a: %p\n", a);
    printf("Address of b: %p\n", b);
    a = myrealloc(a, 24);
    char* c;
    c = mymalloc(31);
    printFreeList();
   /*
     myinit(0);
    printFreeList();
    char* a;
    a = mymalloc(4);
    printFreeList();
    char* b;
    b = mymalloc(17);
    printFreeList();
    printf("Address of a: %p\n", a);
    printf("Address of b: %p\n", b);
    a = myrealloc(a, 24);
    char* c;
    c = mymalloc(31);
    printFreeList();
    printf("Address of c: %p\n", c);
    myfree(c);
    printFreeList();
    myfree(a);
    printFreeList();
    char* d = mymalloc(33);
    printFreeList();
    myfree(b);
    printFreeList();
    myfree(d);
    printFreeList();
    printf("Address of d: %p\n", d);
    mycleanup();
   */
    mycleanup();
}
