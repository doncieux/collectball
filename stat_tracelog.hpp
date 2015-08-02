#ifndef _STAT_TRACELOG_HPP_
#define _STAT_TRACELOG_HPP_
#include <sferes/stat/stat.hpp>
namespace sferes
{
  namespace stat
  {
    SFERES_STAT(TraceLog, Stat)
    {
    public:
      template<typename E>
	void refresh(const E& ea)
      {
	if (ea.gen()<Params::pop::nb_gen-1)
	  return;
	
	std::cout<<"Stat traceLog: we generate the trace log files for the whole population"<<std::endl;
	for (size_t i = 0; i < ea.pop().size(); ++i)
	  {
	    std::cout<<" -- generating trace log file for indiv "<<i<<std::endl;
	    std::stringstream filename;
	    filename<<ea.res_dir()<<"/trace_"<<ea.gen()<<"_"<<i<<".log";

	    std::ofstream dumpfile(filename.str().c_str());	
	    dumpfile<<"# gen "<<ea.gen()<<" indiv i="<<i<<" fit=[";
	    for (size_t j = 0; j < ea.pop()[i]->fit().objs().size(); ++j)
	      dumpfile<< ea.pop()[i]->fit().obj(j) << " ";
	    dumpfile<<"]"<<std::endl;

	    // we create a new individual to avoid any persistance problem
	    Phen ind;
	    ind.gen()=ea.pop()[i]->gen();
	    ind.develop();
	    ind.fit().set_tracefile(&dumpfile);
	    ind.fit().eval(ind);
	    dumpfile.close();

	  }
      } 
    };
  }
}
#endif
