set xlabel "No. of iterations"
set ylabel "Loss Value"
set title "GD variations with Sigmoid"
set grid
plot "../data/graph2/sdg_sig_mse_reg.txt" title "Stochastic GD" with linespoint
replot "../data/graph2/batch_sig_mse_reg.txt" title "Batch GD" with linespoint
replot "../data/graph2/mini_batch_sig_mse_reg.txt" title "Mini-Batch(32) GD" with linespoint
pause -1