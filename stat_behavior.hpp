#ifndef STAT_BEHAVIOR_HPP
#define STAT_BEHAVIOR_HPP
#include <sferes/stat/stat.hpp>
namespace sferes
{
  namespace stat
  {
    // warning : your fitness must have a behavior_adhoc() method
    SFERES_STAT(Behavior, Stat)
    {
    public:
      template<typename E>
	void refresh(const E& ea)
      {
	this->_create_log_file(ea, "behaviors.dat");	
	for (size_t i = 0; i < ea.pop().size(); ++i)
	  {
	    (*this->_log_file)<<ea.gen()<< " ";
	    for (size_t j = 0; j < ea.pop()[i]->fit().objs().size(); ++j)
	      (*this->_log_file) << ea.pop()[i]->fit().obj(j) << " ";
	    for (size_t j = 0; j < ea.pop()[i]->fit().behavior_adhoc().size(); ++j)
	      (*this->_log_file) << ea.pop()[i]->fit().behavior_adhoc()[j] << " ";
	    (*this->_log_file)<<std::endl;	    
	  }
      } 
    };
  }
}
#endif
