#ifndef _INCLUDES_HPP_
#define _INCLUDES_HPP_

/***
includes.hpp : All necessary includes and defines
***/

// Program variants are defined by a set of labels described below:

// these are the default, no need to specify it anymore



#ifdef DYNAMIC_DIVERSITY
#define MULTIDIST
#define DIV_BEHAVIOR
#endif

#if !defined(ELMAN)
#define DNN 
#endif

enum diversity_measure_t {multi=-1,adhoc=0,hamming,trajectory,entropy,
            nb_measures};

typedef enum _fit_id{
  ballcount
#ifdef DIV_BEHAVIOR
  ,div_behavior
#endif 
#ifdef NOV_BEHAVIOR
  ,nov_behavior
#endif
  ,nb_fit
} Fit_id;

#if !defined(ADHOC) && !defined(HAMMING) && !defined(TRAJECTORY) && !defined(ENTROPY) && !defined(MULTIDIST) 
#define TRAJECTORY
#endif

#define NB_TILES 12.0

//switch color
#define SWITCH 4

// ball color
#define BALL 3    

//basket color
#define BASKET 2


// To avoid problems between SDL and multithreading
#if defined(APPLE) || defined(VISU)
#define NO_PARALLEL
#endif

#ifdef NO_PARALLEL
#warning no parallel.
#endif

#ifndef EIGEN3_ENABLED
#error "Eigen3 must be enabled for this experiment"
#endif

#include <Eigen/Core>
 
#include <iostream>
#include <bitset>
#include <limits>
#include <sferes/phen/parameters.hpp>

#ifdef USE_SDL
#include <SDL.h>
#endif


#include <modules/nn2/mlp.hpp>
#include <modules/nn2/phen_dnn.hpp>
#include <modules/nn2/elman.hpp>
//#include <modules/nn2/esn_2.hpp>
//#include "phen_dnn.hpp"

#include <sferes/run.hpp>
//#include <sferes/ctrl.hpp>
#include <sferes/stc.hpp>
#include <sferes/misc.hpp>

#ifndef SAMPLEDPARAMS
#include <sferes/gen/evo_float.hpp>
#else
#include <sferes/gen/sampled.hpp>
#endif

#ifdef FILIATION
#include "nsga2_filiation.hpp"
#else
#include <sferes/ea/nsga2.hpp>
#endif

#include <sferes/fit/fitness.hpp>
//#include <sferes/modif/dummy.hpp>
//#include "modifier_filiation.hpp"
#include <sferes/eval/parallel.hpp>
#include <sferes/eval/eval.hpp>
#include <sferes/stat/pareto_front.hpp>
#include <sferes/stat/best_fit.hpp>



#ifdef FILIATION
#include "stat_filiation.hpp"
#endif

#include "stat_nn.hpp"
#include "stat_tracelog.hpp"
#include <modules/fastsim/simu_fastsim.hpp>

using namespace sferes;
#ifndef SAMPLEDPARAMS
using namespace sferes::gen::evo_float;
#endif
using namespace sferes::gen::dnn;

// Any other definition of point_type is possible, not necessary the trajectory
struct point_traj
{
  int x,y;
  bool ball;
  bool operator==(const struct point_traj &op) const {return x==op.x && y==op.y; } //&& eqball(op); }
  bool eqball(const struct point_traj &op) const {return ball^op.ball;}
  float operator-(const struct point_traj &op) const {return (abs(x-op.x)+abs(y-op.y))*1.0/NB_TILES; } //+NB_TILES*eqball(op); }
};


#include "fit_collectball.hpp"

#include "behavior_dist.hpp"


#if defined(NOV_BEHAVIOR)
#include "modifier_behavior_nov.hpp"
#endif

#if defined(DYNAMIC_DIVERSITY) || defined(DIV_BEHAVIOR)
#include "modifier_dynamic_diversity.hpp"
#endif

#endif