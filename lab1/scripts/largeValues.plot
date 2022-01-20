set xlabel "Size Of Array (NxN)"
set autoscale
set ylabel "Time (s)"
set title "Comparison of Matrix Multiplication Algos"
set grid
plot "./data/largeArraySimpleMultiplication.txt" title "Simple Multiplication" with linespoint
replot "./data/largeArrayBlockMultiplication.txt" title "Block Multiplication(10)" with linespoint
pause -1