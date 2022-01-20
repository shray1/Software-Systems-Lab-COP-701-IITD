set xlabel "No. of threads"
set ylabel "Time(s)"
set title "Time vs Number Of Threads. (Matrix size: 600x600)"
set grid
plot "./data/mythreads.data" title "Mythread Time" with linespoint
replot "./data/pthreads.data" title "Pthread Time" with linespoint
pause -1