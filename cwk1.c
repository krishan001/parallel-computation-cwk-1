//
// Starting code for coursework 1. Requires "cwk1_extra.h", which should be placed in the same directory.
//
// Compile with (on the school machines) or use the provided makefile:
// gcc -fopenmp -Wall -o cwk1 cwk1.c
// where '-fopenmp' is redundant until OpenMP functionality has been added.
//


//
// Standard includes.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
//
// The set itself and the routines to initialise, destroy and print it are defined
// in the file cwk1_extra.h that you should also have downloaded. Although you are
// free to inspect this file, fo not alter the routines it contains, or replace them
// with your own versions, as they will be replaced as part of the assessment.
//
// cwk1_extra.h includes the following global variables:
//
// int setSize;                 // The current size of the set = number of items it contant
// int maxSetSize;              // The maximum size for the set (which should not be altered).
// int *set;                    // The set itself, an array of size maxSetSize.
//
// It also contains the following routines:
//
// int initSet( int maxSize );  // Initialises the set to the given maximum size.
//                              // Returns 0 if successful, -1 for an allocation error.
//
// void destroySet();           // Destroys the set (i.e. deletes all associated resources).
//
// void printSet();             // Prints the set.
//
#include "cwk1_extra.h"


//
// Add a value to the set if it does not currently exist.
//
void addToSet( int value )
{
    int i;
    int shouldReturn = 0;

    // Cannot exceed the maximum size.
    #pragma omp critical
    {
        if( setSize==maxSetSize ) shouldReturn = 1;

        // Since sets should not have duplicates, first check this value is not already in the set.
        if (shouldReturn == 0){
            for( i=0; i<setSize; i++ )
                if( set[i]==value )
                    shouldReturn = 1;
        }
        // Only reach this point if the value was not found and there is room to add to the set.
        if (shouldReturn == 0){
            set[setSize] = value;
            setSize++;
        }
    }
    if (shouldReturn) return;
    
}


//
// Remove a value from the set, if it exists, and shuffle the remaining values so the set remains contiguous.
//
void removeFromSet( int value )
{
    int i;
    int temp[setSize];

    // Find where the index in the set corresponding to the value, if any.
    int index = -1;
    #pragma omp parallel for 
    for( i=0; i<setSize; i++ )
        if( set[i]==value )
            index = i;
    
    // If found, 'remove'. Here, 'removal' is achieved by moving all values later in the set down by one index,
    // and also reducing the set size by one.
    if( index!= -1 )
    {
        #pragma omp parallel for
        for (i = index + 1; i<setSize; i++)
            temp[i] = set[i];
        #pragma omp parallel for
        for( i=index; i<setSize-1; i++ )
            set[i] = temp[i+1];
        setSize--;
    }
}


//
// Parallel sort.
//
void swap(int *num1, int *num2)
{

	int temp = *num1;
	*num1 =  *num2;
	*num2 = temp;
}
void sortSet()
{
    // // Your parallel sort should go here. 

    int i;
    int sorted;
    do{
    sorted = 1;
    //odd phase
    #pragma omp parallel for 
    for (i = 2; i<setSize; i+=2){
        if (set[i-1]>set[i]){
            swap(&set[i-1], &set[i]);
            sorted = 0;
        }
    }
    //even phase
    #pragma omp parallel for 
    for (i = 0; i<setSize-1; i+=2){
        if (set[i]>set[i+1]){
            swap(&set[i], &set[i+1]);
            sorted = 0;
        }
    }
    }while(sorted==0);
}

//
// Main.
//
int main( int argc, char **argv )
{
    int i;

    // Get from the command line the maximum set size, the number of values to add, and the number to remove.
    // You do not need to alter this section of the code.
    if( argc!=4 )
    {
        printf( "Need 3 command line arguments: The maximum set size, the no. values to add, and the no. values to remove.\n" );
        return EXIT_FAILURE;
    }

    int
        maxSetSize  = atoi(argv[1]),
        numToAdd    = atoi(argv[2]),
        numToRemove = atoi(argv[3]);

    if( maxSetSize<=0 || numToAdd<0 || numToRemove<0 )
    {
        printf( "Invalid argumen4942ts: max. size must be > 0, and the numbers to add and remove must both be non-negative." );
        return EXIT_FAILURE;
    }

    // Initialise the set. Retur4942ns -1 if could not allocate memory.
    if( initSet(maxSetSize)==-1 ) return EXIT_FAILURE;

    // Seed the psuedo-random number generator to the current time.
    srand( time(NULL) );
    // Add random numbers in the range 0 to maxSetSize-1 inclusive. You are asked to parallelise this loop in Task 1.
    #pragma omp parallel for
    for( i=0; i<numToAdd; i++ )
        addToSet( rand()%maxSetSize );
    printf( "Attempted to add %i random values. Current state of set:\n", numToAdd );
    printSet();

    // Remove values from the set; random values from the same range as they were added.
    for( i=0; i<numToRemove; i++ )
        removeFromSet( rand()%maxSetSize );

    printf( "\nRemoved up to %i random values if present. Current state of set:\n", numToRemove );
    printSet();

    // Finally, sort the set in increasing order.
    sortSet();
    printf( "\nCalled sortSet(). Current state of set:\n" );
    printSet();

    // You MUST call this function just before finishing - do NOT remove, or change the definition of destroySet(),
    // as it will be changed with a different version for assessment.
    destroySet();

    return EXIT_SUCCESS;
}