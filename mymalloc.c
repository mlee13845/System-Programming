#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

struct meta_data
{
    int flag;
    size_t size;
    struct meta_data *next;
    struct meta_data *prev;
};

void *heap_memory;
int alloc_mode;
struct meta_data *head;
struct meta_data *foot;
struct meta_data *current_loc;

/*
    INPUT:    allocAlg - int - numerical value that determines how the method will search for free space
                    0: First fit
                    1: Next fit
                    2: Best fit
    OUTPUT:   NULL
    ERROR:    perror - Throws an exeption with invalid input
*/
void myinit(int allocAlg)
{
    heap_memory = (void *)malloc(1000000 * sizeof(void));
    struct meta_data *initial_header = heap_memory;
    initial_header->flag = 0;
    initial_header->size = 1000000 - sizeof(struct meta_data) - sizeof(struct meta_data);
    initial_header->next = NULL;
    initial_header->prev = NULL;
    char *sum = (char *)initial_header + sizeof(struct meta_data) + initial_header->size;
    struct meta_data *initial_footer = (struct meta_data *)sum;
    initial_footer->flag = initial_header->flag;
    initial_footer->size = initial_header->size;
    initial_footer->next = NULL;
    initial_footer->prev = NULL;
    head = initial_header;
    foot = initial_footer;
    current_loc = head;
    if (allocAlg == 0)
    {
        alloc_mode = 0;
    }
    else if (allocAlg == 1)
    {
        alloc_mode = 1;
    }
    else if (allocAlg == 2)
    {
        alloc_mode = 2;
    }
    else
    {
        perror("Invalid Input");
    }
}

/*
    INPUT:    size - size_t - the desired amount of allocated memory
    OUTPUT:   new_memory - void* - pointer with not specified type pointing to the beginning of the data chunk
    ERROR:    NULL - memory was unable to be allocated
*/
void *mymalloc(size_t size)
{
    int count = 0;
    if (size == 0)
    {
        return NULL;
    }
    else
    {
        //makes the memory work on an 8 byte shift
        if(size%8 != 0)
        {
            while(size %8 != 0)
            {
                size = size+1;
            }
        }
        if (alloc_mode == 0)
        {
            // START FROM THE BEGINNING
            struct meta_data *ptr = head;
            // IF THIS IS THE ONLY EXISTING FREE META DATA
            if (ptr->prev == NULL && ptr->next == NULL)
            {
                // CHECKS IF PTR SIZE IS LARGER THAN SIZE NEEDED
                if (ptr->size > (size + sizeof(struct meta_data) + sizeof(struct meta_data)))
                {
                    // RE-ADJUST FOOTER FOR ALLOCATED MEMORY
                    char *sum = (char *)ptr + sizeof(struct meta_data) + size;
                    struct meta_data *ptr_footer = (struct meta_data *)sum;
                    ptr_footer->flag = 1;
                    ptr_footer->size = size;
                    ptr_footer->next = NULL;
                    ptr_footer->prev = NULL;
                    // CUTS PTR TO THE SIZE OF DESIRED AMOUNT AND LEAVES REST TO FREE_MEMORY
                    char *sum_2 = (char *)ptr + sizeof(struct meta_data) + size + sizeof(struct meta_data);
                    struct meta_data *free_memory_header = (struct meta_data *)sum_2;
                    free_memory_header->flag = 0;
                    free_memory_header->size = ptr->size - size - sizeof(struct meta_data) - sizeof(struct meta_data);
                    free_memory_header->next = NULL;
                    free_memory_header->prev = NULL;
                    // SET FOOTER FOR FREE_MEMORY
                    struct meta_data *free_memory_footer = foot;
                    free_memory_footer->flag = free_memory_header->flag;
                    free_memory_footer->size = free_memory_header->size;
                    free_memory_footer->next = NULL;
                    free_memory_footer->prev = NULL;
                    // SET ALLOCATED MEMORY HEADER
                    ptr->flag = 1;
                    ptr->size = size;
                    // RETURNS THE CHUNK OF ALLOCATED MEMORY
                    void *new_memory = (char *)ptr + sizeof(struct meta_data);
                    head = free_memory_header;
                    return new_memory;
                    // CHECKS IF SIZE OF PTR IS THE EXACT SAME SIZE OF NEEDED MEMORY SPACE
                }
                // CHECKS IF THE ONE SPACE HAS ENOUGH FOR JUST THE SIZE AND GIVING ENTIRE RELAVANT DATACHUNK WITHOUT SPLITTING
                else if (ptr->size >= size)
                {
                    // UPDATE FLAG OF ALLOCATED MEMORY
                    ptr->flag = 1;
                    // FINDS THE BEGINNING OF THE ALLOCATED MEMORY CHUNK
                    void *new_memory = (char *)ptr + sizeof(struct meta_data);
                    // REMOVES THE LAST FREE MEMORY FROM THE FREE LIST
                    head = NULL;
                    return new_memory;
                }
                // WHEN THERE ARE MORE THAN ONE LINKS OF FREE META DATAS
            }
            else
            {
                // ITERATES THROUGH LINKEDLIST OF FREE META DATAS
                while (ptr->next != NULL)
                {
                    if (ptr->size >= size)
                    {
                        break;
                    }
                    ptr = ptr->next;
                }
                // CHECKS IF SPACE IS ENOUGH TO SPLIT
                if (ptr->size > (size + sizeof(struct meta_data) + sizeof(struct meta_data)))
                {
                    // RE-ADJUST FOOTER FOR ALLOCATED MEMORY
                    char *sum = (char *)ptr + sizeof(struct meta_data) + size;
                    struct meta_data *ptr_footer = (struct meta_data *)sum;
                    ptr_footer->flag = 1;
                    ptr_footer->size = size;
                    // CUTS PTR TO THE SIZE OF DESIRED AMOUNT AND LEAVES REST TO FREE_MEMORY
                    char *sum_2 = (char *)ptr + sizeof(struct meta_data) + size + sizeof(struct meta_data);
                    struct meta_data *free_memory_header = (struct meta_data *)sum_2;
                    free_memory_header->flag = 0;
                    free_memory_header->size = ptr->size - size - sizeof(struct meta_data) - sizeof(struct meta_data);
                    // REPLACES PTR SPACE WITH FREE MEMORY HEADER
                    ptr->prev->next = free_memory_header;
                    free_memory_header->prev = ptr->prev;
                    free_memory_header->next = ptr->next;
                    if (ptr->next != NULL)
                    {
                        ptr->next->prev = free_memory_header;
                    }

                    // SET FOOTER FOR FREE_MEMORY
                    struct meta_data *free_memory_footer = foot;
                    free_memory_footer->flag = free_memory_header->flag;
                    free_memory_footer->size = free_memory_header->size;
                    free_memory_footer->next = NULL;
                    free_memory_footer->prev = NULL;
                    // SET ALLOCATED MEMORY HEADER
                    ptr->flag = 1;
                    ptr->size = size;
                    // RETURNS THE CHUNK OF ALLOCATED MEMORY
                    void *new_memory = (char *)ptr + sizeof(struct meta_data);
                    return new_memory;
                }
                // CHECKS IF THERE IS STILL ENOUGH SPACE FOR MALLOC
                else if (ptr->size >= size)
                {
                    // CALCULATES NEW MEMORY BY SHIFTING IN ADDRESS
                    void *new_memory = (char *)ptr + sizeof(struct meta_data);
                    // IF THE PTR HAS META DATA BEFORE IT
                    if (ptr->prev != NULL)
                    {
                        // PTR REMOVES ITSELF FROM THE LINKLIST
                        ptr->prev->next = ptr->next;
                        ptr->next->prev = ptr->prev;

                        // IF PTR IS THE HEAD OF THE EXPLICIT FREELIST
                    }
                    else
                    {
                        // RE-ASSIGNS HEAD TO THE SECOND META DATA FROM THE TOP
                        head = ptr->next;
                        head->prev = NULL;
                    }
                    return new_memory;
                    // CHECKS IF PTR NODE SIZE IS GREATER THAN SIZE
                }
            }
            return NULL;

            // STOP HERE
        }
        else if (alloc_mode == 1)
        {
            struct meta_data *ptr = head;
            if (ptr->prev == NULL && ptr->next == NULL)
            {
                // CHECKS IF PTR SIZE IS LARGER THAN SIZE NEEDED
                if (ptr->size > (size + sizeof(struct meta_data) + sizeof(struct meta_data)))
                {
                    // RE-ADJUST FOOTER FOR ALLOCATED MEMORY
                    char *sum = (char *)ptr + sizeof(struct meta_data) + size;
                    struct meta_data *ptr_footer = (struct meta_data *)sum;
                    ptr_footer->flag = 1;
                    ptr_footer->size = size;
                    ptr_footer->next = NULL;
                    ptr_footer->prev = NULL;
                    // CUTS PTR TO THE SIZE OF DESIRED AMOUNT AND LEAVES REST TO FREE_MEMORY
                    char *sum_2 = (char *)ptr + sizeof(struct meta_data) + size + sizeof(struct meta_data);
                    struct meta_data *free_memory_header = (struct meta_data *)sum_2;
                    free_memory_header->flag = 0;
                    free_memory_header->size = ptr->size - size - sizeof(struct meta_data) - sizeof(struct meta_data);
                    free_memory_header->next = NULL;
                    free_memory_header->prev = NULL;
                    // SET FOOTER FOR FREE_MEMORY
                    struct meta_data *free_memory_footer = foot;
                    free_memory_footer->flag = free_memory_header->flag;
                    free_memory_footer->size = free_memory_header->size;
                    free_memory_footer->next = NULL;
                    free_memory_footer->prev = NULL;
                    // SET ALLOCATED MEMORY HEADER
                    ptr->flag = 1;
                    ptr->size = size;
                    current_loc = ptr;
                    // RETURNS THE CHUNK OF ALLOCATED MEMORY
                    void *new_memory = (char *)ptr + sizeof(struct meta_data);
                    head = free_memory_header;
                    return new_memory;
                    // CHECKS IF SIZE OF PTR IS THE EXACT SAME SIZE OF NEEDED MEMORY SPACE
                }
                // CHECKS IF THE ONE SPACE HAS ENOUGH FOR JUST THE SIZE AND GIVING ENTIRE RELAVANT DATACHUNK WITHOUT SPLITTING
                else if (ptr->size >= size)
                {
                    // UPDATE FLAG OF ALLOCATED MEMORY
                    ptr->flag = 1;
                    current_loc = ptr;
                    // FINDS THE BEGINNING OF THE ALLOCATED MEMORY CHUNK
                    void *new_memory = (char *)ptr + sizeof(struct meta_data);
                    // REMOVES THE LAST FREE MEMORY FROM THE FREE LIST
                    head = NULL;
                    return new_memory;
                }
                // WHEN THERE ARE MORE THAN ONE LINKS OF FREE META DATAS
            }
            else
            {
                // ITERATES THROUGH LINKEDLIST OF FREE META DATAS
                while (ptr->next != NULL)
                {
                    // CHECKS TO SEE IF MEMORY SPACE HAS ENOUGH SPACE AND HAS A BIGGER ADDRESS THAN THE PREVIUS ALLOCATED MEMORY
                    if (ptr->size >= size && ptr > current_loc)
                    {
                        break;
                    }
                    ptr = ptr->next;
                }
                // CHECKS IN CASE THE CURRENT LOC
                if (ptr < current_loc || ptr->size < size)
                {
                    // LOOK FOR MEMORY LOCATION UP TO  BEFORE THE LAST MALLO DATA
                    while (ptr->next < current_loc)
                    {
                        //CHECK TO SEE IF DATA CAN FIT
                        if (ptr->size >= size)
                        {
                            break;
                        }
                        ptr = ptr->next;
                    }
                    //CHECKS IF THE MEMORY IS A FIT
                    if (ptr->size >= size)
                    {
                        //CHECKS IF SPACE IS BIG ENOUGH FOR SIZE AND 2 META DATAS
                        if (ptr->size > (size + sizeof(struct meta_data) + sizeof(struct meta_data)))
                        {
                            // RE-ADJUST FOOTER FOR ALLOCATED MEMORY
                            char *sum = (char *)ptr + sizeof(struct meta_data) + size;
                            struct meta_data *ptr_footer = (struct meta_data *)sum;
                            ptr_footer->flag = 1;
                            ptr_footer->size = size;
                            // CUTS PTR TO THE SIZE OF DESIRED AMOUNT AND LEAVES REST TO FREE_MEMORY
                            char *sum_2 = (char *)ptr + sizeof(struct meta_data) + size + sizeof(struct meta_data);
                            struct meta_data *free_memory_header = (struct meta_data *)sum_2;
                            free_memory_header->flag = 0;
                            free_memory_header->size = ptr->size - size - sizeof(struct meta_data) - sizeof(struct meta_data);
                            // REPLACES PTR SPACE WITH FREE MEMORY HEADER
                            ptr->prev->next = free_memory_header;
                            free_memory_header->prev = ptr->prev;
                            free_memory_header->next = ptr->next;
                            if (ptr->next != NULL)
                            {
                                ptr->next->prev = free_memory_header;
                            }
                            else
                            {
                                ptr->next = NULL;
                            }

                            // SET FOOTER FOR FREE_MEMORY
                            struct meta_data *free_memory_footer = foot;
                            free_memory_footer->flag = free_memory_header->flag;
                            free_memory_footer->size = free_memory_header->size;
                            free_memory_footer->next = NULL;
                            free_memory_footer->prev = NULL;
                            // SET ALLOCATED MEMORY HEADER
                            ptr->flag = 1;
                            ptr->size = size;
                            current_loc = ptr;
                            // RETURNS THE CHUNK OF ALLOCATED MEMORY
                            void *new_memory = (char *)ptr + sizeof(struct meta_data);
                            head = free_memory_header;
                            return new_memory;
                        }
                        // CHECKS IF THERE IS STILL ENOUGH SPACE FOR MALLOC
                        else if (ptr->size >= size)
                        {
                            current_loc = ptr;
                            // CALCULATES NEW MEMORY BY SHIFTING IN ADDRESS
                            void *new_memory = (char *)ptr + sizeof(struct meta_data);
                            // IF THE PTR HAS META DATA BEFORE IT
                            if (ptr->prev != NULL)
                            {
                                // PTR REMOVES ITSELF FROM THE LINKLIST
                                ptr->prev->next = ptr->next;
                                ptr->next->prev = ptr->prev;

                                // IF PTR IS THE HEAD OF THE EXPLICIT FREELIST
                            }
                            else
                            {
                                // RE-ASSIGNS HEAD TO THE SECOND META DATA FROM THE TOP
                                head = ptr->next;
                            }
                            return new_memory;
                            // CHECKS IF PTR NODE SIZE IS GREATER THAN SIZE
                        }
                    }
                    return NULL;
                } // need to fix

                // CHECK ONE LAST TIME TO SEE IF NEXT FIT CONDITION IS MET
                if (ptr->size >= size && ptr > current_loc)
                {
                    // CHECKS IF SPACE IS ENOUGH TO SPLIT
                    if (ptr->size > (size + sizeof(struct meta_data) + sizeof(struct meta_data)))
                    {
                        // RE-ADJUST FOOTER FOR ALLOCATED MEMORY
                        char *sum = (char *)ptr + sizeof(struct meta_data) + size;
                        struct meta_data *ptr_footer = (struct meta_data *)sum;
                        ptr_footer->flag = 1;
                        ptr_footer->size = size;
                        // CUTS PTR TO THE SIZE OF DESIRED AMOUNT AND LEAVES REST TO FREE_MEMORY
                        char *sum_2 = (char *)ptr + sizeof(struct meta_data) + size + sizeof(struct meta_data);
                        struct meta_data *free_memory_header = (struct meta_data *)sum_2;
                        free_memory_header->flag = 0;
                        free_memory_header->size = ptr->size - size - sizeof(struct meta_data) - sizeof(struct meta_data);
                        // REPLACES PTR SPACE WITH FREE MEMORY HEADER
                        ptr->prev->next = free_memory_header;
                        free_memory_header->prev = ptr->prev;
                        free_memory_header->next = ptr->next;
                        if (ptr->next != NULL)
                        {
                            ptr->next->prev = free_memory_header;
                        }
                        else
                        {
                            ptr->next = NULL;
                        }

                        // SET FOOTER FOR FREE_MEMORY
                        struct meta_data *free_memory_footer = foot;
                        free_memory_footer->flag = free_memory_header->flag;
                        free_memory_footer->size = free_memory_header->size;
                        free_memory_footer->next = NULL;
                        free_memory_footer->prev = NULL;
                        // SET ALLOCATED MEMORY HEADER
                        ptr->flag = 1;
                        ptr->size = size;
                        current_loc = ptr;
                        // RETURNS THE CHUNK OF ALLOCATED MEMORY
                        void *new_memory = (char *)ptr + sizeof(struct meta_data);
                        head = free_memory_header;
                        return new_memory;
                    }
                    // CHECKS IF THERE IS STILL ENOUGH SPACE FOR MALLOC
                    else if (ptr->size >= size)
                    {
                        current_loc = ptr;
                        // CALCULATES NEW MEMORY BY SHIFTING IN ADDRESS
                        void *new_memory = (char *)ptr + sizeof(struct meta_data);
                        // IF THE PTR HAS META DATA BEFORE IT
                        if (ptr->prev != NULL)
                        {
                            // PTR REMOVES ITSELF FROM THE LINKLIST
                            ptr->prev->next = ptr->next;
                            ptr->next->prev = ptr->prev;

                            // IF PTR IS THE HEAD OF THE EXPLICIT FREELIST
                        }
                        else
                        {
                            // RE-ASSIGNS HEAD TO THE SECOND META DATA FROM THE TOP
                            head = ptr->next;
                        }
                        return new_memory;
                        // CHECKS IF PTR NODE SIZE IS GREATER THAN SIZE
                    }
                }
            }
            return NULL;
        }
        else if (alloc_mode == 2)
        {
            // START FROM THE BEGINNING
            struct meta_data *ptr = head;
            struct meta_data *temp;
            size_t minimum = 1000000;
            // IF THIS IS THE ONLY EXISTING FREE META DATA
            if (ptr->prev == NULL && ptr->next == NULL)
            {
                // CHECKS IF PTR SIZE IS LARGER THAN SIZE NEEDED
                if (ptr->size > (size + sizeof(struct meta_data) + sizeof(struct meta_data)))
                {
                    // RE-ADJUST FOOTER FOR ALLOCATED MEMORY
                    char *sum = (char *)ptr + sizeof(struct meta_data) + size;
                    struct meta_data *ptr_footer = (struct meta_data *)sum;
                    ptr_footer->flag = 1;
                    ptr_footer->size = size;
                    ptr_footer->next = NULL;
                    ptr_footer->prev = NULL;
                    // CUTS PTR TO THE SIZE OF DESIRED AMOUNT AND LEAVES REST TO FREE_MEMORY
                    char *sum_2 = (char *)ptr + sizeof(struct meta_data) + size + sizeof(struct meta_data);
                    struct meta_data *free_memory_header = (struct meta_data *)sum_2;
                    free_memory_header->flag = 0;
                    free_memory_header->size = ptr->size - size - sizeof(struct meta_data) - sizeof(struct meta_data);
                    free_memory_header->next = NULL;
                    free_memory_header->prev = NULL;
                    // SET FOOTER FOR FREE_MEMORY
                    struct meta_data *free_memory_footer = foot;
                    free_memory_footer->flag = free_memory_header->flag;
                    free_memory_footer->size = free_memory_header->size;
                    free_memory_footer->next = NULL;
                    free_memory_footer->prev = NULL;
                    // SET ALLOCATED MEMORY HEADER
                    ptr->flag = 1;
                    ptr->size = size;
                    // RETURNS THE CHUNK OF ALLOCATED MEMORY
                    void *new_memory = (char *)ptr + sizeof(struct meta_data);
                    head = free_memory_header;
                    return new_memory;
                    // CHECKS IF SIZE OF PTR IS THE EXACT SAME SIZE OF NEEDED MEMORY SPACE
                }
                // CHECKS IF THE ONE SPACE HAS ENOUGH FOR JUST THE SIZE AND GIVING ENTIRE RELAVANT DATACHUNK WITHOUT SPLITTING
                else if (ptr->size >= size)
                {
                    // UPDATE FLAG OF ALLOCATED MEMORY
                    ptr->flag = 1;
                    // FINDS THE BEGINNING OF THE ALLOCATED MEMORY CHUNK
                    void *new_memory = (char *)ptr + sizeof(struct meta_data);
                    // REMOVES THE LAST FREE MEMORY FROM THE FREE LIST
                    head = NULL;
                    return new_memory;
                }
                // WHEN THERE ARE MORE THAN ONE LINKS OF FREE META DATAS
            }
            else
            {
                // ITERATES THROUGH LINKEDLIST OF FREE META DATAS
                while (ptr->next != NULL)
                {
                    // COMPARES EACH ITERATION AND UPDATES MINIMUM SIZE AS WELL AS THE THE META DATA WITH THE CLOSEST FITTING SIZE
                    if (ptr->size >= size && minimum > ptr->size)
                    {
                        // HOLD OPTIMAL SIZE NODE
                        temp = ptr;
                        // UPDATES MINIMUM
                        minimum = ptr->size;
                    }
                    ptr = ptr->next;
                }
                if (ptr->size >= size && minimum > ptr->size)
                {
                    *temp = *ptr;
                    minimum = ptr->size;
                }
                // CHECKS IF SPACE IS ENOUGH TO SPLIT
                if (temp->size > (size + sizeof(struct meta_data) + sizeof(struct meta_data)))
                {
                    // RE-ADJUST FOOTER FOR ALLOCATED MEMORY
                    char *sum = (char *)temp + sizeof(struct meta_data) + size;
                    struct meta_data *ptr_footer = (struct meta_data *)sum;
                    ptr_footer->flag = 1;
                    ptr_footer->size = size;
                    // CUTS PTR TO THE SIZE OF DESIRED AMOUNT AND LEAVES REST TO FREE_MEMORY
                    char *sum_2 = (char *)temp + sizeof(struct meta_data) + size + sizeof(struct meta_data);
                    struct meta_data *free_memory_header = (struct meta_data *)sum_2;
                    free_memory_header->flag = 0;
                    free_memory_header->size = temp->size - size - sizeof(struct meta_data) - sizeof(struct meta_data);
                    // REPLACES PTR SPACE WITH FREE MEMORY HEADER
                    temp->prev->next = free_memory_header;
                    free_memory_header->prev = temp->prev;
                    free_memory_header->next = temp->next;
                    if (temp->next != NULL)
                    {
                        temp->next->prev = free_memory_header;
                    }
                    else
                    {
                        temp->next = NULL;
                    }

                    // SET FOOTER FOR FREE_MEMORY
                    struct meta_data *free_memory_footer = foot;
                    free_memory_footer->flag = free_memory_header->flag;
                    free_memory_footer->size = free_memory_header->size;
                    free_memory_footer->next = NULL;
                    free_memory_footer->prev = NULL;
                    // SET ALLOCATED MEMORY HEADER
                    temp->flag = 1;
                    temp->size = size;
                    // RETURNS THE CHUNK OF ALLOCATED MEMORY
                    void *new_memory = (char *)temp + sizeof(struct meta_data);
                    head = free_memory_header;
                    return new_memory;
                }
                // CHECKS IF THERE IS STILL ENOUGH SPACE FOR MALLOC
                else if (temp->size >= size)
                {
                    // CALCULATES NEW MEMORY BY SHIFTING IN ADDRESS
                    void *new_memory = (char *)temp + sizeof(struct meta_data);
                    // IF THE PTR HAS META DATA BEFORE IT
                    if (temp->prev != NULL)
                    {
                        // PTR REMOVES ITSELF FROM THE LINKLIST
                        temp->prev->next = temp->next;
                        temp->next->prev = temp->prev;

                        // IF PTR IS THE HEAD OF THE EXPLICIT FREELIST
                    }
                    else
                    {
                        // RE-ASSIGNS HEAD TO THE SECOND META DATA FROM THE TOP
                        head = temp->next;
                    }
                    return new_memory;
                    // CHECKS IF PTR NODE SIZE IS GREATER THAN SIZE
                }
            }
            return NULL;
        }
    }
}

/*
    INPUT:    ptr - struct meta_data - pointer pointing to the data chunk
    OUTPUT:   NULL
    ERROR:    N/A
*/
void myfree(void *ptr)
{
    if (ptr != NULL)
    {
        // FINDS THE META DATA ASSOCIATED WTIH DATA AND CHANGED FREE FLAG
        struct meta_data *total_left;
        struct meta_data *total_right;
        char *sum = (char *)ptr - sizeof(struct meta_data);
        struct meta_data *ptr_2 = (struct meta_data *)sum;
        ptr_2->flag = 0;
        int right_is_filled = 0;
        int left_is_filled = 0;
        // NAVIGATE TO THE FOOTER OF THE CURRENT DATA CHUNK
        char *right_sum = (char *)ptr_2 + ptr_2->size + sizeof(struct meta_data);
        struct meta_data *ptr2_footer = (struct meta_data *)right_sum;
        ptr2_footer->flag = 0;

        // CHECK TO SEE IF THE FOOTER OF THE DATA CHUNK IS THE SAME AS THE FOOTER OF THE ENTIRE HEAP
        if (ptr2_footer != foot)
        {
            // PEEKS AT RIGHT META DATA TO SEE IF IT IS FREE OR NOT
            right_sum = right_sum + sizeof(struct meta_data);
            struct meta_data *peek_right = (struct meta_data *)right_sum;
            total_right = peek_right;
            // IF RIGHT SIDE IS FREE THEN COALESCE
            if (peek_right->flag == 0)
            {
                // SHIFT TO FOOTER META DATA OF NEXT NEIGHBOR
                char *footer_sum = (char *)peek_right + peek_right->size + sizeof(struct meta_data);
                struct meta_data *right_footer = (struct meta_data *)footer_sum;
                // UPDATE SIZE OF NEW COALSCE DATA
                ptr_2->size = peek_right->size + sizeof(struct meta_data) + sizeof(struct meta_data) + ptr_2->size;
                right_footer->size = ptr_2->size;
                // ITERATE THROUGH FREE LIST AND REPLACE MATCHING METADATA
                struct meta_data *ptr_3 = head;
                while (ptr_3->next != NULL)
                {
                    if (ptr_3 == peek_right)
                    {
                        break;
                    }
                    ptr_3 = ptr_3->next;
                }
                // CHECKS IF THERE IS A MATCH
                if (ptr_3 == peek_right)
                {
                    // REPLACES PTR_3 IN LIST WITH NEW LEFT_HEADER
                    if (ptr_3->prev != NULL)
                    {
                        ptr_3->prev->next = ptr_2;
                        ptr_2->prev = ptr_3->prev;
                        ptr_2->next = ptr_3->next;
                        if (ptr_3->next != NULL)
                        {
                            ptr_3->next->prev = ptr_2;
                        }
                        if (ptr_3->prev == NULL)
                        {
                            head = ptr_2;
                        }
                    }
                    else
                    {
                        ptr_2->prev = NULL;
                        ptr_3->next->prev = ptr_2;
                        head = ptr_2;
                    }
                }
            }
            else
            {
                right_is_filled = 1;
            }
            // ELSE ADD NEW FREE META DATA TO THE BEGINNING OF FREE LIST
        }
        else
        {

            struct meta_data *newPtr = head;
            while (newPtr->next != NULL)
            {
                newPtr = newPtr->next;
            }
            newPtr->next = ptr_2;
        }
        // SEEIF PTR_2 METADATA IS AT THE HEAD
        if (ptr_2 != heap_memory)
        {
            // SHIFT BACK TO THE FOOTER OF THE PREVIOUS DATA
            char *left_sum = (char *)ptr_2 - sizeof(struct meta_data);
            struct meta_data *peek_left = (struct meta_data *)left_sum;
            total_left = peek_left;
            // COALSCE CURRENT MEMORY TO PREVIOUS DATA
            if (peek_left->flag == 0)
            {
                // SHIFT BACK TO HEAD META DATA OF PRIVOUS NEIGHBOR
                char *header_sum = (char *)peek_left - peek_left->size - sizeof(struct meta_data);
                struct meta_data *left_header = (struct meta_data *)header_sum;
                // UPDATE SIZE OF NEW COALSCE DATA
                left_header->size = left_header->size + sizeof(struct meta_data) + sizeof(struct meta_data) + ptr_2->size;
                ptr2_footer->size = left_header->size;
                // ITERATE THROUGH FREE LIST AND REPLACE MATCHING METADATA
                struct meta_data *ptr_3 = head;
                while (ptr_3->next != NULL)
                {
                    if (ptr_3 == left_header)
                    {
                        break;
                    }
                    ptr_3 = ptr_3->next;
                }
                // CHECKS IF THERE IS A MATCH
                if (ptr_3 == left_header)
                {
                    // REPLACES PTR_3 IN LIST WITH NEW LEFT_HEADER
                    if (ptr_3->prev != NULL)
                    {
                        ptr_3->prev->next = left_header;
                    }
                    left_header->prev = ptr_3->prev;
                    left_header->next = ptr_3->next;
                    if (ptr_3->next != NULL)
                    {
                        ptr_3->next->prev = left_header;
                    }
                    if (ptr_3->prev == NULL)
                    {
                        head = ptr_3;
                    }
                }
                // SETS A NEW POINTER TO HEAD
                struct meta_data *ptr_4 = head;
                // ITERATE THROUGH FREE LIST TO FIND LEFT OVER UNCOALSCED MEMORY
                while (ptr_4->next != NULL)
                {
                    // BREAK OUT WHEN MATCH IS FOUND
                    if (ptr_4 == ptr_2)
                    {
                        break;
                    }
                    ptr_4 = ptr_4->next;
                }
                // CHECKS ONE MORE TIME TO VERIFY THEY ARE INFACT THE SAME FOR LAST NODE
                if (ptr_4 == ptr_2)
                {
                    if (ptr_4->prev == NULL)
                    {
                        head = head->next;
                    }
                    else if (ptr_4->next == NULL)
                    {
                        ptr_4->prev->next = NULL;
                    }
                    else
                    {
                        ptr_4->prev->next = ptr_4->next;
                    }
                }
            }
            else
            {
                left_is_filled = 1;
            }
        }
        else
        {
            struct meta_data *temp = head;
            head = ptr_2;
            head->next = temp;
            temp->prev = head;
        }
        if (left_is_filled == 1 && right_is_filled == 1)
        {
            struct meta_data *newPtr = head;
            // IF IT IS THE SMALLEST ADDRESS
            if (ptr_2 < newPtr)
            {
                struct meta_data *temp = head;
                head = ptr_2;
                head->next = temp;
                temp->prev = head;
            }
            // ITERATES THROUGH TO FIND RIGHT LOCATION TO PUT IN THE ADDRESS
            while (newPtr->next != NULL)
            {
                // WHEN THE ADDRESS OF PTR_2 IS GREATER THAN NEWPTR ADDRESS AND SMALLER THAT NEWPTR->NEXT ADDRESS
                if (newPtr->next > ptr_2 && newPtr < ptr_2)
                {
                    break;
                }
                newPtr = newPtr->next;
            }
            // IF IT IS THE BIGGEST ADDRESS
            if (newPtr < ptr_2 && newPtr->next == NULL)
            {
                newPtr->next = ptr_2;
                ptr_2->prev = newPtr;
                // IF IT IS IN BETWEEN THE TWO ADDRESSES
            }
            else if (newPtr < ptr_2 && newPtr->next > ptr_2)
            {

                ptr_2->next = newPtr->next;
                ptr_2->next->prev = ptr_2;
                ptr_2->prev = newPtr;
                newPtr->next = ptr_2;
            }
        }
    }
}

/*
    INPUT:    size - size_t - mew updated size
              ptr - struct meta_data - pointer to data chunk
    OUTPUT:   NULL
    ERROR:    N/A
*/
void *myrealloc(void *ptr, size_t size)
{
    // LOOKING FOR HEAD OF DATA CHUNK
    char *sum = (char *)ptr - sizeof(struct meta_data);
    struct meta_data *target_Meta = (struct meta_data *)sum;
    // STORING THE CURRENT SIZE OF THE DATA CHUNK FOR MEMORY COPY PURPOSES
    size_t data_len = target_Meta->size;
    // ALLOCATE MEMORY FOR THE GIVEN SIZE
    void *newAddr = mymalloc(size);
    // MEMORY COPY INTO NEW ADDRESS
    memcpy(newAddr, ptr, data_len);
    // FREE MEMORY IN THE PTR
    myfree(ptr);
    return newAddr;
}
/*
    INPUT:    N/A
    OUTPUT:   NULL
    ERROR:    N/A
*/
void mycleanup()
{
    free(heap_memory);
}
