
Open the terminal inside the code folder containing the Makefile

Compilation
>make program

To call the function help()
>./main

To run ACO on an instance
>./main --SLS ACO --instance_file ../instances/50_20_01

Specify the maximum number of iterations with the parameter max_iterations
>./main --SLS ACO --instance_file ../instances/50_20_01 --max_iterations 62

To run ILS, just replace ACO by ILS
>./main --SLS ILS --instance_file ../instances/50_20_01 --max_iterations 62

Clean
>make clean