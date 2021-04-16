#!/bin/csh

#number of threads:
foreach t (1 2 4 8 16)
    echo NUMT = $t
    foreach n (4 8 16 32 64 128 256 512)
        echo NUMNODES = $n
        g++-10 -DNUMT=$t -DNUMNODES=$n super.cpp -o super -lm -fopenmp
        ./super >> results.csv
    end
    printf "\n" >> results.csv
end