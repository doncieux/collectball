

#ifndef MODIFIER_BEHAVIOR_NOV_HPP
#define MODIFIER_BEHAVIOR_NOV_HPP

#include <sferes/stc.hpp>
#include <Eigen/Core>
#include <algorithm>


using namespace sferes::ea;
using namespace sferes::b_dist;

#ifdef MULTIDIST 
#error "Novelty modifier does not work with multiple behavior distances."
#endif

namespace sferes
{


  namespace modif
  {
    namespace novelty
    {

      struct dist_t{
		float dist;
		int index;
		bool operator<(const dist_t &disto) const {return dist<disto.dist;};
      };      

      template<typename Phen, typename Archive, typename Params>
      struct _parallel_behavior_nov
      {
		typedef typename std::vector<boost::shared_ptr<Phen> > pop_t;
	
		pop_t _pop;
		const Archive& _apop; 
	
	
		_parallel_behavior_nov(pop_t& pop, const Archive& apop) : _pop(pop), _apop(apop) {}
		_parallel_behavior_nov(const _parallel_behavior_nov& ev) : _pop(ev._pop), _apop(ev._apop) {}
	

		void operator() (const parallel::range_t& r) const
		{
	  
	  		for (size_t i = r.begin(); i != r.end(); ++i)
	    	{
	      		float sum=0.0;
	      		std::vector<struct dist_t> v_dist;
	      		//v_dist.reserve(_pop.size() + _apop.size());

	      		//Maximum archive size
	      		int arsize=Params::novelty::max_archive_size;
	      		if(_apop.size()<Params::novelty::max_archive_size)
					arsize=_apop.size();

	      		for (size_t j = 0; j < _pop.size() + arsize; ++j) {
					float hd=1.0;
					if(i!=j) {
		  				if(j<_pop.size()) {
		    				hd = multi_dist(Params::behavior_diversity::behavior_distance,*_pop[i],*_pop[j]);
		    				sum+=hd;
		  				}
		  				else
		    				hd = multi_dist(Params::behavior_diversity::behavior_distance,*_pop[i],*_apop[j-_pop.size()-arsize+_apop.size()]);
					}
					struct dist_t dist_temp;
					dist_temp.index=j;
					dist_temp.dist=hd;
					v_dist.push_back(dist_temp);
		
	      		}
	      //Sort the distance vector
	      sort(v_dist.begin(),v_dist.end());
	      
	      float d = 0.0f;
	      //int n_archive=0;
	      
	      //Get the sparsness with the k closest vectors
	      for(size_t j=0; j<Params::novelty::k; ++j) 
		d+=v_dist[j].dist;
	      
	      d /= (float)Params::novelty::k;
	      
	      int l =  _pop[i]->fit().objs().size() - 1;
#if defined(USE2OBJ)	      
	      assert(l > 0);
#endif
	      //Set the last objective to the novelty found
	      _pop[i]->fit().set_obj(l, d);

#ifdef LOGNOV	      
	      _pop[i]->fit().lognov=d;
#endif
	    }
	}
      };
    }

    // ADD the mean distance to the population to the last objective (it
    // DOESN'T add the objective automatically)
    // you HAVE to initialize this value to a "good" one (depending on
    // your constraints scheme; for instance, 0)
    // you phenotype/individual class must have a float dist(const
    // Phen& o) method (the dist method must be thread-safe)
    SFERES_CLASS(BehaviorNov)

    {
    public:
      
      BehaviorNov() : _rho_min(Params::novelty::rho_min), _not_added(0) {}
      
      template<typename Ea>
	void apply(Ea& ea)
      {

	//Archive file NN to open
	if(!archfileNN) {
	  std::string filename =ea.res_dir()+"/archive_NN.dat";
	  archfileNN.open(filename.c_str());
	}

#ifdef LOGTRAJECTORY
	//Archive trajectory file to open
	if(!trajfile) {
	  std::string filename =ea.res_dir()+"/archive_traj.dat";
	  trajfile.open(filename.c_str());
	}
#endif
	// parallel compute
	parallel::init();
	parallel::p_for(parallel::range_t(0, ea.pop().size()), 
			novelty::_parallel_behavior_nov<typename Ea::phen_t, archive_t, Params>(ea.pop(),_archive));

	//Update archive
	int l =  ea.pop()[0]->fit().objs().size() - 1;
	size_t added = 0;
	size_t added2 = 0;
	_popsize=ea.pop().size();
	//_n_archive=0;
	
	// Permutation for adding in archive in random order
	std::vector<int> permutation;
	for(size_t i = 0; i < ea.pop().size(); ++i)
	  permutation.push_back(i);
	random_shuffle(permutation.begin(),permutation.end());
	    
	for (size_t ii = 0; ii < ea.pop().size(); ++ii)
	  {
	    size_t i = permutation[ii];

#ifndef LOGNOV
	    float sparse = ea.pop()[i]->fit().obj(l);
#else
	    float sparse = ea.pop()[i]->fit().lognov;
#endif
	    if (sparse > _rho_min)// || !(rand()%2500))
	      {

#ifdef LOGTRAJECTORY
		trajfile<<_archive.size()<<" ";
		for (size_t k=0;k<ea.pop()[i]->fit()._trajectory.size();k++) 
		  trajfile<<ea.pop()[i]->fit()._trajectory[k]<<" ";
		trajfile<<std::endl;
#endif


		_archive.push_back(ea.pop()[i]);
		_not_added = 0;
		added++;
		added2++;
		if (_archive.size() > Params::novelty::k  && added2 > 4) {
		  _rho_min *= 1.1f;
		  added2=0;
		}
		std::cout<<"added a indivi., sparse="<<sparse<<" rho="<<_rho_min<<std::endl;
	      }
	    
	    else 
	      ++_not_added;
	    // if (_not_added > 200) {
	    //_rho_min *= 0.999;
	    //_not_added=0;
	    //}
	    
	  }

	trajfile<<"new gen--"<<std::endl;

	if (_not_added > 1000)
	  _rho_min *= 0.97;
	
	//if (_archive.size() > Params::novelty::k  && added > 200)
	//  _rho_min *= 1.5f;
	std::cout<<"nb added--: "<<added<<std::endl;
	
      }
      int _size() const {return _archive.size();}
      float _rho() const {return _rho_min;}      
      float _prop_archive() const {return (float)_n_archive/(float)_popsize;}
	
    protected:      
      typedef std::vector<boost::shared_ptr<phen_t> > archive_t;
      archive_t _archive;
      std::ofstream trajfile;
      std::ofstream archfileNN;
      float _rho_min;
      size_t _not_added;
      int _n_archive;
      size_t _popsize;
    };
  }
}

#endif
