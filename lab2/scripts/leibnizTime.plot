set xlabel "No. of threads"
set ylabel "Time(s)"
set title "Time vs Number Of Threads"
set grid
plot "./data/leibnizTime.data" title "Leibniz Series" with linespoint
pause -1