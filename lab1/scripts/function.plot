set xlabel "Size Of Array (NxN)"
set autoscale
set ylabel "Time (s)"
set title "Comparison of Matrix Multiplication Algos"
set grid
plot "./data/simpleMultiplication.txt" title "Simple Multiplication" with linespoint
replot "./data/blockMultiplication.txt" title "Block Multiplication(10)" with linespoint
pause -1
