
# ILS and ACO for the Permutation Flow-Shop Problem (PFSP)

This project is part of the "Heuristics" course at the Free University of Brussels (Master 1 Optimization and Operational Research - 2021 - 2022).

I implement two metaheuristics for solving the PFSP. I program in C++ without using object-oriented programming. The code takes as input a PFSP instance as well as a heuristic and returns a good solution to the instance.

Keywords : <code>Metaheuristics</code> <code>ILS</code> <code>ACO</code> <code>Local Search</code> <code>Intensification & Diversification</code> <code>Variable Neighborhood Descent</code>

## How to use

- Compilation : <code>$ make program</code>
- Help : <code>$ ./main</code>
- Run ACO on an instance : <code>$ ./main --SLS ACO --instance_file ../instances/50_20_01</code>
- Specify the maximum number of iterations with the parameter max_iterations : <code>$ ./main --SLS ACO --instance_file ../instances/50_20_01 --max_iterations 62</code>
- To run ILS, just replace ACO by ILS : <code>$ ./main --SLS ILS --instance_file ../instances/50_20_01 --max_iterations 62</code>
- Clean : <code>$ make clean</code>

## Doc

<table>
<thead>
<tr>
<th>Code file</th>
<th>Label</th>
</tr>
</thead>
<tbody>
<tr>
<td>aco.h</td>
<td>Ant Colony Optimization heuristic</td>
</tr>
<tr>
<td>errors.h</td>
<td>Error and help messages</td>
</tr>
  <tr>
<td>experiment.h</td>
<td>Experimentation file</td>
</tr>
  <tr>
<td>files.h</td>
<td>File management, writing, reading...</td>
</tr>
  <tr>
<td>globalVariables.h</td>
<td>Declaration of global parameters and variables</td>
</tr>
  <tr>
<td>ils.h</td>
<td>Iterated Local Search heuristic</td>
</tr>
  <tr>
<td>localSearch.h</td>
<td>Iterative Improvement and Variable Neighborhood Descent heuristics</td>
</tr>
  <tr>
<td>main.cpp</td>
<td>Main file</td>
</tr>
  <tr>
<td>parameters.h</td>
<td>Parameters parsing and assertions handling</td>
</tr>
  <tr>
<td>pfspinstance.cpp</td>
<td>PFSP instance class implementation</td>
</tr>
  <tr>
<td>pfspinstance.h</td>
<td>PFSP instance class declaration</td>
</tr>
</tbody>
</table>

## Results - ILS vs ACO - Correlation Plots

### Instances of size 50

![cor_50](https://user-images.githubusercontent.com/40352310/134543547-42daba4d-45c2-4897-add3-d91b3cb2b7f8.png)

### Instances of size 100

![cor_100](https://user-images.githubusercontent.com/40352310/134543549-d115c6c5-f15c-4c50-8a6a-e6bf53e3251a.png)
