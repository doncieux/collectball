
#include "includes.hpp"      // Sferes & other includes -- SEE THIS FILE FOR PROGRAM VARIANTS
                             // also include entropy_distance.hpp
                             //              edit_distance.hpp
                             //              modifier_behavior.hpp
#include "defparams.hpp"     // Params
#include "defstats.hpp"      // Define all stats
#include "definitsimu.hpp"   // Simulation initialization

#include "fit_collectball.hpp"



std::string res_dir="not_initialized";

// ****************** Main *************************
int main(int argc, char **argv)
{
  srand(time(0));
  

  typedef eval::Parallel<Params> eval_t;
  // STATS 
  typedef boost::fusion::vector<
    sferes::stat::ParetoFront<phen_t, Params>
#ifdef FILIATION
    ,sferes::stat::Filiation<phen_t, Params>
#endif
    ,sferes::stat::BestFitVal<phen_t, Params>
#ifdef TRACELOG
    ,sferes::stat::TraceLog<phen_t, Params>
#endif
    >  stat_t;

  //MODIFIER
  typedef boost::fusion::vector<
#if defined (DYNAMIC_DIVERSITY) || defined(DIV_BEHAVIOR)
    modif::Modifier_DD<Params>
#elif defined(NOV_BEHAVIOR) 
    modif::BehaviorNov<Params>  
#else
    modif::Dummy<Params>
#endif
    > modifier_t;
  //typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t; 
  //typedef ea::EaServ<ea_t> eas_t;


#ifdef FILIATION
  typedef ea::Nsga2Filiation<phen_t, eval_t, stat_t, modifier_t, Params> ea_t; 
#else
  typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t; 
#endif
  ea_t ea;

  try {
    res_dir=ea.res_dir();
    run_ea(argc, argv, ea);
  }
  catch(fastsim::Exception e) {
    std::cerr<<"fastsim::Exception: "<<e.get_msg()<<std::endl;
  }
  return 0;
}
