set xlabel "Number of terms(10^n)"
set ylabel "Accuracy (no. of same digits)"
set title "Accuracy As Number of Terms Scale"
set grid
plot "./data/leibniz.data" title "Leibniz Series" with linespoint
replot "./data/wallis.data" title "Wallis Series" with linespoint
pause -1