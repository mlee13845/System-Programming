#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
int isDirectory(char *name)
{
    //char *instancePath = mergeString(path, name);
    struct stat randomStruct;
    stat(name, &randomStruct);  
    int result = S_ISDIR(randomStruct.st_mode);
    if(result == 0)
    {
        return 0;
    }else{
        return 1;
    }
}

char* mergeString(char *original, struct dirent *current)
{   
    
    int originalLen = strlen(original);
    int currentlen = strlen(current->d_name);
    int totalLen = originalLen + currentlen + 100;
    
    char* fullPath = (char*)malloc(sizeof(char)* totalLen );

    strcpy(fullPath, original);
    strcat(fullPath, "/");
    strcat(fullPath, current->d_name);
    free(original);
    return fullPath;
    //printf("%s\n", fullPath);
    
}

int recursiveSearch (char *pathName, char *target, int *found)
{

  

    //OPENS CURRENT DIRECTORY
    DIR *dir;
    struct dirent *contents;
    dir = opendir(pathName);
    //INITIALIZES INSTANCEPATH
    char *instancePath;


    //READS INTO CONTENTS UNTIL NULL FLAG
    while((contents = readdir(dir))!= NULL)
    {
      
        //CHECKS IF IT IS ".." OR "." AND IGNORES IT
       if(strcmp(contents->d_name, "..")!= 0)
        {
            if(strcmp(contents->d_name, ".")!= 0)
            {  

                //printf("%s\n", contents->d_name);
                //CREATES AN INSTANCE OF THE PATH AND CREATES IT THE SAME SIZE AS ORIGINAL PATH
                instancePath = (char*)malloc(sizeof(char)* (strlen(pathName) + 100));
                //COPY PATH INTO THE NEW INSTANCE OF PATH
                strcpy(instancePath, pathName);
                //MERGEST THE NEW FILE NAME WITH THE CURRENT PATH
                instancePath = mergeString(instancePath, contents);


        
        //CHECKS IF CURRENT FOLDER/FILE NAME MATCHES THE TARGET NAME
                if(isDirectory(instancePath))
                {

                     if(strstr(contents->d_name,target)){

                        printf("%s\n", instancePath);
                    }

                    *found = recursiveSearch(instancePath, target, found);
                    
                   


        
                //CHECKS IF THE CURRENT DIRECTORY PATH IS A DIRECTORY
                }else if(strstr(contents->d_name, target))
                {
                   
                     //PRINTS OUT PATH 
                    printf("%s\n", instancePath);
                    //FREES THE INSTANCE OF PATH IN THIS ONE RECURSIVE LAYER
                    free(instancePath);
                    //RETURNS TRUE TO INDICATE TO BEGINE TERMINATION OF RECURSION
                    closedir(dir);
                    return 1;
                    
                }

                free(instancePath);
            
            }
        }
            //IF FILE IS FOUND CONTINUES THE TERMINATION OF THE RECURSIVE CYCLE
        // if(*found == 1)
        // {   
        //     //FREES THIS INSTANCCE OF PATH IN THIS ONE RECURSIVE LAYER
        //     free(instancePath);
        //     //CLOSES DIRECTORY IN THIS ONE RECURSIVE LAYER
        //     closedir(dir);
        //     //RETURNS TRUE
        //     return 1;
        // }
        
        
    }   
    
       
 
    closedir(dir);
    //ENDS THE EXPLORATION OF THE CURRENT DIRECTORY BY ASSIGNING FOUND THE FALSE
    return 0;



}

int treePrint (char *pathName, int *found)
{

  

    //OPENS CURRENT DIRECTORY
    DIR *dir;
    struct dirent *contents;
    dir = opendir(pathName);
    //INITIALIZES INSTANCEPATH
    char *instancePath;


    //READS INTO CONTENTS UNTIL NULL FLAG
    while((contents = readdir(dir))!= NULL)
    {
      
        //CHECKS IF IT IS ".." OR "." AND IGNORES IT
       if(strcmp(contents->d_name, "..")!= 0)
        {
            if(strcmp(contents->d_name, ".")!= 0)
            {  
                //printf("%s\n", contents->d_name);
                //CREATES AN INSTANCE OF THE PATH AND CREATES IT THE SAME SIZE AS ORIGINAL PATH
                instancePath = (char*)malloc(sizeof(char)* (strlen(pathName) + 100));
                //COPY PATH INTO THE NEW INSTANCE OF PATH
                strcpy(instancePath, pathName);
                
                //MERGEST THE NEW FILE NAME WITH THE CURRENT PATH
                instancePath = mergeString(instancePath, contents);

                 printf("%s\n", instancePath);
               
        //CHECKS IF CURRENT FOLDER/FILE NAME MATCHES THE TARGET NAME
                if(isDirectory(instancePath))
                {


                    *found = treePrint(instancePath, found);
                    
                   


        
                //CHECKS IF THE CURRENT DIRECTORY PATH IS A DIRECTORY
                }

                free(instancePath);
            
            }
        }
        
        
    }   
    
       
 
    closedir(dir);
    //ENDS THE EXPLORATION OF THE CURRENT DIRECTORY BY ASSIGNING FOUND THE FALSE
    return 0;



}



int main(int args, char* argv[])
{
  
    
     int found = 0;
    // char *curDir = get_current_dir_name();
    DIR *dir;
    struct dirent *contents;
    dir = opendir(".");
    char *path;
    if(args == 1){

       while((contents = readdir(dir))!= NULL){
            path = (char*)malloc(sizeof(char)*5);
            strcpy(path, ".");
            if(strcmp(contents->d_name, "..")!= 0)
            {
                if(strcmp(contents->d_name, ".")!= 0)
                {
                    path = mergeString(path, contents);
                    printf("%s\n", path);
                    if(isDirectory(contents->d_name))
                    {
                        found = treePrint(path,&found);  
                    }
                
                
                }      
            
        }
        free(path);
        }
        
    }else{
        char target[256];
        sscanf(argv[1],"%s", target);
        while((contents = readdir(dir))!= NULL){
            path = (char*)malloc(sizeof(char)*5);
            strcpy(path, ".");
            if(strcmp(contents->d_name, "..")!= 0)
            {
                if(strcmp(contents->d_name, ".")!= 0)
                {
                    path = mergeString(path, contents);
                    if(isDirectory(contents->d_name))
                    {
                    if(strstr(contents->d_name,target)){

                            printf("%s\n", path);
                        } 

                        found = recursiveSearch(path, target,&found);  
                    }else if(strstr(contents->d_name,target)){
                        
                        printf("%s\n", path);
                    }
                
                
                }      
            
        }
        free(path);
        }
    
    }
    
    closedir(dir);
 

    return 0;
}
