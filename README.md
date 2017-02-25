# GeneticPAC

This is a simple application of a genetic algorithm for Artificial Intelligence in games.
This code have some capacity for learning how to control pacman in google's Pacman DOODLE.

![GeneticPAC](https://github.com/MatheusNBDG/GeneticPAC/blob/master/exemple.jpg)

## How does it work

For each direction of the pacman, this program will find the paths that he could go and
get the followring data for input from each path:

1. The distance for the obstacle and for each detour made.
2. A int value representing if the obstacle is hostile, neutron or a reward.
3. The total posible reward of the path.
4. The rate of change of the distance (velocity).

he could get path with in max two detour, so he only look for a certain
neighborhood of the game.

So we make the input in a simple neural-network (5-5-1 with bias) with the desire 
than the network learn the probability function for the pacman walk in a specific 
path. So we make a one vs. all to decide for what direction the pacman should go.

Because of that, there is no value of activation of the output neuron, the larger is
the choosen one.

## Implementation

This program have been programed in C++. For run this do you will need to configure
you compile to have two extras libraris (Eigen and SFML). This is plataform dependent
(only windows OS).

The files of the project are:

- `main.ccp` Where is the main function and all come together.

- `Genome.h` Header where the Genome class is declared.

- `Genome.ccp` The script where the functions of the Genome class are declared,
  also the save and load, natural-selection, mutation and the cross-over function lies.

- `Network.h` Header where the Network class is declared.

- `Network.ccp` The script where the functions of the Network class are declared,
 here we have the action function (the function than chooses what direction for move).

- `Windows.h` Header with a lot of function for get the input, references of screen, 
 control the pacman etc.

- `SFML.h` Header for create the auxiliar window for show the output (I will revise it yet).

- `Exemple folder` There is a pre-trained population for test. For load you only needed to put
 it in the folder of main.ccp and press L in the main interface.

- `DLL folder` Things needed for compile. Paste them in the folder of the main.cpp.

## RUN ALERT

When you run this input the values of the size of population after the pacman google DOODLE is 
already in the screen, also verify if the google scan page (where you should find the pacman google 
DOODLE) is at the top (the code will press HOME for bug "correction") when run.

If all is OK the mouse incon will point to the pacman and wait for you input. Don't move the pacman 
screen after that.