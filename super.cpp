#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

// constants
#define XMIN -1
#define XMAX 1
#define YMIN -1
#define YMAX 1
#define N 0.7

// prototypes
float Height(int, int);

// set number of threads
#ifndef NUMT
#define NUMT 16
#endif

// set number of nodes
#ifndef NUMNODES
#define NUMNODES 1000
#endif

// how many tries for max performance
#ifndef NUMTRIES
#define NUMTRIES 5000
#endif

int main( int argc, char *argv[])
{
#ifndef _OPENMP
    fprintf(stderr, "No OpenMP Support!\n");
    return 1;
#endif

    omp_set_num_threads(NUMT);
    double maxPerformance = 0.;
    double volume;

    // looking for maximum performance
    for (int tries = 0; tries < NUMTRIES; tries++)
    {
        double time0 = omp_get_wtime();
        volume = 0.;

        // calculate tile area
        float fullTileArea = ( ((XMAX - XMIN)/(float)(NUMNODES - 1)) * ((YMAX - YMIN)/(float)(NUMNODES - 1)) );

        // calculate the four corners
        double quarterArea = fullTileArea / 4.;
        double quarterVolume = quarterArea * Height(0, 0);
        volume += quarterVolume * 4.;

        // calculate sides
        double halfArea = fullTileArea / 2.;
        double halfVolume = halfArea * Height(0, 1);
        volume += halfVolume * ((NUMNODES * 4) - 4);

        // calculate volume
        # pragma omp parallel for collapse(2) default(none), shared(fullTileArea), reduction(+:volume)
        // since we calculated sides and corners outside the loop, only iterate over interior "full" nodes
        for (int iv = 1; iv < NUMNODES - 1; iv++) // "Y" coordinate
        {
            for (int iu = 1; iu < NUMNODES - 1; iu++) // "X" coordinate
            {
                float z = Height(iu, iv);
                volume += fullTileArea * z;                
            }
        }
        double time1 = omp_get_wtime();
        double megaVolumesPerSecond = (double)NUMNODES*NUMNODES / (time1 - time0) / 1000000.;
        if (megaVolumesPerSecond > maxPerformance)
            maxPerformance = megaVolumesPerSecond;
    }
    volume = volume * 2.;
    fprintf(stdout, "%2d threads: %8d nodes; volume = %6.2f; megaVolumes/sec = %6.2f\n", NUMT, NUMNODES, volume, maxPerformance);

    std::cout << maxPerformance << ",";
    return 0;
}

// iu, iv = 0 .. NUMNODES - 1
float Height (int iu, int iv)  
{
    float x = -1. + 2.*(float)iu / (float)(NUMNODES - 1); // -1. to +1. 
    float y = -1. + 2.*(float)iv / (float)(NUMNODES - 1); // -1. to +1.

    float xn = pow(fabs(x), (double)N);
    float yn = pow(fabs(y), (double)N);
    float r = 1. - xn - yn;
    if (r <= 0.)
        return 0.;
    float height = pow(r, 1./(float)N);
    return height;
}