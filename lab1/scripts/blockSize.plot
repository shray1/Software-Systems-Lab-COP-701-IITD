set xlabel "Block Size(b)"
set autoscale
set ylabel "Time (s)"
set title "Comparison of Block Size with Block Matrix Multiplication Performance"
set grid
plot "./data/blockSizeVariation.txt" title "Block Size Variation with Matrix size: 1000x1000" with linespoint
pause -1