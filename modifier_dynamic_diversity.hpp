#ifndef MODIFIER_DYNAMIC_DIVERSITY_HPP
#define MODIFIER_DYNAMIC_DIVERSITY_HPP

#include "behavior_dist.hpp"
#include <sferes/stc.hpp>
#include<vector>
//#include "viz_server.hpp"
//#include "ea_server.hpp"

namespace sferes
{
  typedef Eigen::Matrix<float,Eigen::Dynamic,Eigen::Dynamic> matrix_t;

  namespace modif
  {

    using namespace b_dist;

    SFERES_CLASS(Modifier_DD)
    {

    private:
      // Current behavioral diversity measure
      int _div;

      std::ofstream dumpfile;
      std::ofstream dumpfile2;


      //std::vector<matrix_t> m_dists;


      template<typename Ea>
	void choose_div(Ea& ea)
      {
		_div=misc::rand<size_t>(0,nb_measures);
#ifdef VERBOSE
		std::cout<<"choose random, _div="<<_div<<std::endl;
#endif
      }

    
      
    public:
      Modifier_DD():_div(Params::behavior_diversity::behavior_distance) {
		//std::cout<<__PRETTY_FUNCTION__<<std::endl;
		/*for (int i=0;i<nb_measures;i++) {
	  		matrix_t m;
	  		m.setZero();
	  		m_dists.push_back(m);
		}*/
      }

      template<typename Ea>
	void apply(Ea& ea)
      {
	  //std::cout<<__PRETTY_FUNCTION__<<std::endl;
	  if(!dumpfile.is_open()) {
	    std::string filename =ea.res_dir()+"/dump_dynamic_diversity.dat";
	    dumpfile.open(filename.c_str());
	  }
	  
	  std::cout<<"Modifier_DD Gen: "<<ea.gen()<<" Pop size="<<ea.pop().size()<<std::endl;

	  if ((_div == multi)) {
	    choose_div(ea);
	  }
	  dumpfile<<ea.gen()<<" "<<_div<<" - ";

	  matrix_t m_dist=matrix_t::Constant(ea.pop().size(),ea.pop().size(),0);
	  for (int i=0;i<ea.pop().size()-1;i++) {
	    m_dist(i,i)=0;
	    for (int j=i+1;j<ea.pop().size();j++) {
	  	  //std::cerr<<"i="<<i<<" j="<<j<<" pop size="<<ea.pop().size()<<std::endl;
	      m_dist(i,j)=b_dist::multi_dist(_div,*ea.pop()[i],*ea.pop()[j]);
	      m_dist(j,i)=m_dist(i,j);
	      if (std::isnan(m_dist(i,j))) {
			std::cout<<"Warning, distance between indiv "<<i<<" and "<<j<<" is nan"<<std::endl;
	      }
	    }	    
	  }
	  float min=-1,max=-1, mean=0;
 	  for (int i=0;i<ea.pop().size();i++) {
 	    float d = m_dist.row(i).sum();
 	    ea.pop()[i]->fit().set_obj(div_behavior,d);
	    if ((min ==-1)||(d<min)) {
	      min = d;
	    }
	    if ((max ==-1)||(d>max)) {
	      max = d;
	    }
	    mean+=d;
 	  }
	  mean/=(float)ea.pop().size();
	  dumpfile<<max<<" "<<min<<" "<<mean<<" - ";
 	  for (int i=0;i<ea.pop().size();i++) {
	    dumpfile<<ea.pop()[i]->fit().obj(1)<<" ";
	  }
 	  dumpfile<<std::endl;

	  if ((Params::behavior_diversity::behavior_distance==multi)&&
	  	((ea.gen()==1)||((ea.gen()!=0)&&(ea.gen()%Params::dynamic_diversity::change_period == 0)))) {
	    choose_div(ea);
	  }

       }
    };
  }
}

#endif
