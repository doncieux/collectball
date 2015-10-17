Collectball
==========

This is the source code of the collectball experiment.

*If you use this software in an academic article, please cite:*

Doncieux, S. and Mouret, J.-B. (2010). Behavioral diversity measures for Evolutionary Robotics. WCCI 2010 IEEE World Congress on Computational Intelligence, Congress on Evolutionary Computation (CEC). Pages 1303--1310.

@INPROCEEDINGS{2010ACTI1525,
	CATEGORY = {ACTI},
	AUTHOR = {Doncieux, S. and Mouret, J.-B.},
	TITLE = {Behavioral diversity measures for Evolutionary Robotics},
	YEAR = {2010},
	PDF = {http://www.isir.upmc.fr/files/2010ACTI1525.pdf},
	BOOKTITLE = {WCCI 2010 IEEE World Congress on Computational Intelligence, Congress on Evolutionary Computation (CEC)},
	PAGES = {1303--1310}
}


*The dynamic behavioral diversity approach, that gives the best results in this experiment, is described in the following paper, please cite it if you use it:*

Doncieux, S. and Mouret, J.B. (2013). Behavioral Diversity with Multiple Behavioral Distances. Proc. of IEEE Congress on Evolutionary Computation, 2013 (CEC 2013).

@INPROCEEDINGS{2013ACTI2839,
	CATEGORY = {ACTI},
	AUTHOR = {Doncieux, S. and Mouret, J.B.},
	TITLE = {Behavioral Diversity with Multiple Behavioral Distances},
	YEAR = {2013},
	PDF = {http://www.isir.upmc.fr/files/2013ACTI2839.pdf},
	BOOKTITLE = {Proc. of IEEE Congress on Evolutionary Computation, 2013 (CEC 2013)}
}

Usage & installation
--------------------

### Dependencies:
sferes2 core (https://github.com/sferes2/sferes2)
sferes2 modules:
* fastsim (https://github.com/jbmouret/libfastsim for the core library and https://github.com/sferes2/fastsim to connect it to sferes)
* nn2 (https://github.com/sferes2/nn2)

sferes2 has its own dependencies (notably boost, tbb and eigen). Check sferes2 documentation for more details.

### Installation

In the following, it is assumed that all the git repositories have been cloned in ~/git.

First, libfastsim need to be compiled:
    cd ~/git/libfastsim
    ./waf configure
    ./waf build

Before compiling the experiment, the following instructions need to be executed:
    cd ~/git/sferes2/modules
    ln -s ~/git/fastsim
    ln -s ~/git/nn2
    cd ~/git/sferes2/exp
    ln -s ~/git/collectball
    cd ~/git/sferes2/
    echo fastsim >> modules.conf
    echo nn2 >> modules.conf

To compile the experiment:
    cd ~/git/sferes2
    ./waf configure
    ./waf build --exp=collectball

### Launching an experiment and viewing the results

To run an experiment, launch one of the executables in ~/git/sferes2/build/default/exp/collectball. It will create a directory with the generated results.
gen_XX files contain the best individual(s) of the generation XX. To look at its behavior, launch the following command:
    my_executable -o data --load=gen_XX
where my_executable is the the program that has generated gen_XX and XX the generation number to look at.

### Experiments in sferes

An experiment describes what is to be evolved, how and how it is evaluated. The experiments provided here contain mostly the evaluation part (the fitness function). The other parts are standard sferes2 modules.

Variants of an experiments are defined using macros. This is the reason why the source conde contains many
    #ifdef VARIANT1
       // what to do for VARIANT1
    #else
       // what to do for other variants 
    #endif

A variant of an experiment is defined by a set of labels. The choice of the variants to compile is made in the wscript file. The variant with the labels VARIANT1 FIT1 ENV1 of an exp my_exp.cpp creates an executable called my_exp_variant1_fit1_env1.

Author
-------
- Stephane Doncieux stephane.doncieux@isir.upmc.fr: main author and maintainer

Other contributors
------------
- Jean-Baptiste Mouret
- Charles Ollion 


Peer-reviewed academic papers that use this experiment
----------------------------------------


(you can find a pdf for most of these publications on http://scholar.google.com).

### 2014
- Doncieux, S. (2014). Knowledge Extraction from Learning Traces in Continuous Domains. AAAI 2014 fall Symposium ''Knowledge, Skill, and Behavior Transfer in Autonomous Robots''.

### 2013
- Doncieux, S. and Mouret, J.B. (2013). Behavioral Diversity with Multiple Behavioral Distances. Proc. of IEEE Congress on Evolutionary Computation, 2013 (CEC 2013).
- Doncieux, S. (2013). Transfer Learning for Direct Policy Search: A Reward Shaping Approach. Proceedings of ICDL-EpiRob conference.

### 2012
- Mouret, J.-B. and Doncieux, S. (2012). Encouraging Behavioral Diversity in Evolutionary Robotics: an Empirical Study. Evolutionary Computation. Vol 20 No 1 Pages 91-133.

### 2011
- Ollion, C. and Doncieux, S. (2011). Why and How to Measure Exploration in Behavioral Space. GECCO'11: Proceedings of the 13th annual conference on Genetic and evolutionary computation ACM, publisher. Pages 267--294.

### 2010
- Doncieux, S. and Mouret, J.-B. (2010). Behavioral diversity measures for Evolutionary Robotics. WCCI 2010 IEEE World Congress on Computational Intelligence, Congress on Evolutionary Computation (CEC). Pages 1303--1310.

Documentation
-------------

See sferes2 main documentation for an overview of this framework. 

The collectball experiment aims at making a robot collect balls and put them into a basket. Each collected ball gives one fitness point. The environment is continuous, as well as the orders sent to the motors.

The main variants defined are:

Fitness objectives:
* NO_DIV: no diversity nor novelty objectives. Number of collected balls only. Don't expect it to work
* DIV_BEHAVIOR: basic behavioral diversity mechanism
* NOV_BEHAVIOR: novelty objective (diversity with respect to the current population and an archive of past individuals).
* DYNAMIC_DIVERSITY: behavioral diversity within a behavioral space that changes along evolution.

* VARIANT_SW1 and VARIANT_SW2: more difficult versions of the original task. There are less balls and the switch needs to be activated for them to appear.  

* VISU: look at the behaviors during an evolutionary run. By default, they do not appear for the run to be faster. You can anyway see them afterwards.

