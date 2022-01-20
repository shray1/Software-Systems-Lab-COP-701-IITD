set xlabel "Number of Concurrent Requests to Server"
set ylabel "Mean Response Time"
set title "Response Time vs Concurrent Requests"
set grid
plot "response.data" title "Request Response Time" with linespoint
replot "responseWithDb.data" title "Request Response Time (with DB)" with linespoint
pause -1