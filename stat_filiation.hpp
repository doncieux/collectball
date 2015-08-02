#ifndef STAT_FILIATION_HPP
#define STAT_FILIATION_HPP
#include <sferes/stat/stat.hpp>
namespace sferes
{
  namespace stat
  {
    // warning : your fitness must have a behavior_adhoc() method
    SFERES_STAT(Filiation, Stat)
    {
    public:
      template<typename E>
	void refresh(const E& ea)
      {
	_pop=ea.pop();
	
	// format of the stat file:
	// gen number_in_pop number father fitness_objectives
	this->_create_log_file(ea, "filiation.dat");	
	show_all(*(this->_log_file), ea.gen());

      }

      void show(std::ostream& os, size_t k) const
      {
	os<<"#stat filiation. "<<std::endl;
	if (k>=_pop.size()) {
	  std::cerr<<"Problem while showing stat filiation, k="<<k<<" >= pop.size()="<<_pop.size()<<std::endl;
	  exit(1);
	}
	//show_all(os, 0);
	_pop[k]->develop();
	_pop[k]->show(os);
	_pop[k]->fit().set_mode(fit::mode::view);
	_pop[k]->fit().eval(*_pop[k]);
	os << "#=> displaying individual " << k << std::endl;
	os << "#fit:";
	for (size_t i =0; i < _pop[k]->fit().objs().size(); ++i)
	  os << _pop[k]->fit().obj(i) << " ";
	os << std::endl;
	assert(k < _pop.size());

      }

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
        ar & BOOST_SERIALIZATION_NVP(_pop);
      }

      void show_all(std::ostream& os, size_t gen = 0) const
      {
	for (size_t i = 0; i < _pop.size(); ++i)
	  {
	    (*this->_log_file)<<gen<<" "<<i<<" ";
	    (*this->_log_file)<<_pop[i]->fit().number()<< " ";
	    (*this->_log_file)<<_pop[i]->fit().father()<< " ";

	    for (size_t j = 0; j < _pop[i]->fit().objs().size(); ++j)
	      (*this->_log_file) << _pop[i]->fit().obj(j) << " ";
	    (*this->_log_file)<<std::endl;	    
	  }

      }    

    protected:
      typedef std::vector<boost::shared_ptr<Phen> > pop_t;

      pop_t _pop;
 
    };
  }
}
#endif
