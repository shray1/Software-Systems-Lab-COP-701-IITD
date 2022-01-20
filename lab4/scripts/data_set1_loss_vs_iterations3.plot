set xlabel "No. of iterations"
set ylabel "Loss Value"
set title "Data Set 1 - Loss vs Iterations"
#set title "Stochastic Gradient Descent"
#set title "Batch Gradient Descent"
#set title "Mini-Batch(32 size) Gradient Descent"
set grid
plot "../data/graph3_3/sdg_sig_mse_cls.txt" title "Stochastic GD" with linespoint
#plot "../data/graph3_3/batch_sig_mse_cls.txt" title "Batch GD" with linespoint
#plot "../data/graph3_3/mini_batch_sig_mse_cls.txt" title "Mini-Batch(32) GD" with linespoint
pause -1


