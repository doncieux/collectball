#ifndef _BEHAVIOR_DIST_HPP
#define _BEHAVIOR_DIST_HPP

#include "includes.hpp"
#include "defparams.hpp"

#ifndef NB_TILES
#define NB_TILES 12.0
#endif

#define T_EQUAL 0.01


namespace sferes {


  namespace b_dist {

  // Computes the edit distance between two vectors
  template<typename point_type>
  float edit_dist(const ::std::vector<point_type> &d1, 
         const ::std::vector<point_type> &d2)
  {
    int d1size = d1.size()+1;
    int d2size = d2.size()+1;
    float *matrix = new float[d1size  * d2size];
    for (int i = 0; i < d1size; i++)
      for (int j = 0; j < d2size; j++)
  matrix[i+d1size*j]=0.0;
    for (int i = 0; i < d1size; i++)
      matrix[i] = i;
    for (int j = 0; j < d2size; j++)
      matrix[d1size*j] = j;
    
    for (int i = 1; i < d1size; i++) {
      for (int j = 1; j < d2size; j++) {
  float cost = (d2[j - 1] == d1[i - 1]) ? 0 : d2[j-1] - d1[i-1];
  matrix[i+d1size*j] = std::min(matrix[i - 1+d1size*j] + 1, std::min(matrix[i+d1size*(j - 1)] + 1, matrix[i - 1+d1size*(j - 1)] + cost));
  
  if ((i > 1) && (j > 1) &&
      (d1[i - 1] == d2[j - 2]) &&
      (d1[i - 2] == d2[j - 1])) {
    matrix[i + d1size*j] = std::min(matrix[i + d1size*j],matrix[i - 2 + d1size*(j - 2)] + cost);
  }
      }
    }
    float ret_val=matrix[d1size * d2size - 1];
    delete [] matrix;
   
    return ret_val;
  }

  
template <int nb_bits>
  float hamming_dist(const ::std::vector< std::bitset<nb_bits> > &d1, 
         const ::std::vector< std::bitset<nb_bits> > &d2)
  {
    float ham = 0;
    assert(d1.size() == d2.size());
    for (size_t i = 0; i < d1.size(); i++)
      ham += (float) (d1[i] ^ d2[i]).count() / (float) d1[i].size();   
    return ham / (float)d1.size();
  }

    float euclide_dist(Eigen::VectorXf a, Eigen::VectorXf b) {
      Eigen::VectorXf diff=a-b;
      return sqrtf(diff.dot(diff));
    }

    template<typename Indiv>
    float multi_dist(int div,const Indiv& o1, const Indiv &o2)
    {
      switch(div) {
        case adhoc: 
	       return euclide_dist(o1.fit()._behavior_adhoc, o2.fit()._behavior_adhoc);
        case hamming:
	       return hamming_dist<Params::fitness::nb_bits>(o1.fit()._behavior_binary,o2.fit()._behavior_binary);
        case trajectory:
	       return edit_dist(o1.fit()._trajectory,o2.fit()._trajectory);
        case entropy:
	       return euclide_dist(o1.fit()._entropy,o2.fit()._entropy);
        default:
	       std::cerr<<"WARNING: unknown behavioral diversity measure: "<<div<<std::endl;
      }
      return 0;

    }
  }
}
#endif
