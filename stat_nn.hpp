#ifndef STAT_NN_HPP
#define STAT_NN_HPP
#include <sferes/stat/stat.hpp>

namespace sferes
{
  namespace stat
  {
    SFERES_STAT(Nn, Stat)
    {
    public:
      template<typename E>
	void refresh(const E& ea)
      {
	this->_create_log_file(ea, "nn.dat");	
	for (size_t i = 0; i < ea.pop().size(); ++i)
	  {
	    (*this->_log_file) << ea.gen() << " "
			       << ea.pop()[i]->nn().get_nb_neurons() << " "
			       << ea.pop()[i]->nn().get_nb_connections()
			       << std::endl;
	  }
      } 
    };
  }
}
#endif
