
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
  typedef boost::fusion::vector<stat::BestFitMultipleMaps<phen_t, Params>,stat::Container<phen_t, Params>,stat::Progress<phen_t, Params>, stat::MultipleMaps<phen_t, Params>,  stat::MultipleMapsNovelty<phen_t, Params>
#ifdef TRACELOG
    ,sferes::stat::TraceLog<phen_t, Params>
#endif

> stat_t;

    //MODIFIER
    typedef modif::Dummy<> modifier_t;
    /*
  typedef boost::fusion::vector<
#if defined (DYNAMIC_DIVERSITY) || defined(DIV_BEHAVIOR)
    modif::Modifier_DD<Params>
#elif defined(NOV_BEHAVIOR) 
    modif::BehaviorNov<Params>  
#else
    modif::Dummy<Params>
#endif
    > modifier_t;
    */
  //typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t; 
  //typedef ea::EaServ<ea_t> eas_t;

    /*
#ifdef FILIATION
  typedef ea::Nsga2Filiation<phen_t, eval_t, stat_t, modifier_t, Params> ea_t; 
#else
  typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t; 
#endif
    */

#if defined(GRID)
    typedef container::Grid<phen_t, Params1> container1_t;
    typedef container::Grid<phen_t, Params2> container2_t;
    typedef container::Grid<phen_t, Params3> container3_t;
    //    typedef container::Grid<phen_t, Params4> container4_t;
    typedef boost::fusion::vector<container1_t, container2_t, container3_t//, container4_t
				  > container_v_t;
#else // ARCHIVE
    typedef container::Archive<phen_t, Params1> container1_t;
    typedef container::Archive<phen_t, Params2> container2_t;
    typedef container::Archive<phen_t, Params3> container3_t;
    //    typedef container::Archive<phen_t, Params4> container4_t;
    typedef boost::fusion::vector<container1_t, container2_t, container3_t //, container4_t
				  > container_v_t;
#endif
#if defined(RANDOM)
    typedef selector::Random<phen_t> select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(FITNESS)
    typedef selector::ScoreProportionate<phen_t,selector::getFitness> select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(NOVELTY)
    typedef selector::ScoreProportionate<phen_t,selector::getNovelty> select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(CURIOSITY)
    typedef selector::ScoreProportionate<phen_t,selector::getCuriosity> select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(POPFITNESS)
    typedef selector::PopulationBased<phen_t, selector::ScoreProportionate<phen_t, selector::getFitness> > select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(POPNOVELTY)
    typedef selector::PopulationBased<phen_t, selector::ScoreProportionate<phen_t, selector::getNovelty> > select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(POPCURIOSITY)
    typedef selector::PopulationBased<phen_t, selector::ScoreProportionate<phen_t, selector::getCuriosity> > select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(TOURFITNESS)
    typedef selector::TournamentBased<phen_t,selector::getFitness> select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(TOURNOVELTY)
    typedef selector::TournamentBased<phen_t,selector::getNovelty> select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(TOURCURIOSITY)
    typedef selector::TournamentBased<phen_t,selector::getCuriosity> select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(PARETO) //NSLC
    typedef selector::ParetoBased<phen_t,boost::fusion::vector<selector::getNovelty,selector::getLocalQuality>, Params > select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(FITNESSNOVELTY) 
    typedef selector::ParetoBased<phen_t,boost::fusion::vector<selector::getNovelty,selector::getFitness>, Params > select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#elif defined(FITNESSCURIOSITY) 
    typedef selector::ParetoBased<phen_t,boost::fusion::vector<selector::getCuriosity,selector::getFitness>, Params > select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#else // NOSELECTION
    typedef selector::NoSelection<phen_t> select_t;
    typedef boost::fusion::vector<select_t, select_t, select_t> select_v_t;
#endif

    typedef map_selector::MSRandom<Params,fit_v_t,phen_t> map_select_t;
    typedef map_inclusion_selector::MIRandomGen<Params,fit_v_t, phen_t> map_inclusion_select_t;

    typedef ea::MultipleMapsQualityDiversity<phen_t, eval_t, stat_t, modifier_t, select_t, map_inclusion_select_t, map_select_t, container_v_t, Params> ea_t;


    ea_t ea;
    
    try {
      res_dir=ea.res_dir();
      //ea.run();
      run_ea(argc, argv, ea);
    }
    catch(fastsim::Exception e) {
      std::cerr<<"fastsim::Exception: "<<e.get_msg()<<std::endl;
    }
    return 0;
}
