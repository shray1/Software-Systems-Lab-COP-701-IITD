set xlabel "No. of iterations"
set ylabel "Loss Value"
set title "Batch-GD with Activation Functions"
set grid
plot "../data/graph1/batch_sig_mse_reg.txt" title "Sigmoid" with linespoint
replot "../data/graph1/batch_tanh_mse_reg.txt" title "Tanh" with linespoint
#plot "../data/graph1/batch_relu_mse_reg.txt" title "Relu" with linespoint
pause -1