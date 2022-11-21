#include <stdio.h>



int main(int args, char **argv){

    int initialNum;
    int divisor = 2;
    int counter = 0;
 
    //uses  sscanf to convert char type variable to integer
    sscanf(argv[1], "%d", &initialNum);
    //creates new variable to hold current number as we divide by divisor
    int currentNum = initialNum;
    //while loop will divide by incrementing numbers if divisible until divisor reaches initialNum value 
    while(divisor < initialNum)
    {
        //checks currentNum is divisible by current divisor
        if(currentNum % divisor == 0)
        {
            //updates currentNum by finding quotient
            currentNum = currentNum / divisor;
            printf("%d ", divisor);
            //counter is used to keep track of number of prime numbers
            counter++;
        }else{
            if(divisor == 2)
            {
                divisor++;
            }else{
                divisor = divisor + 2;
            }
        }
        
    }
    //if there was no prime numbers print the initial number
    if(counter == 0){
        printf("%d\n", initialNum);
    }else{
        printf("\n");
    }
    
}
