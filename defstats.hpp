namespace sferes
{
  namespace stat
  {
    std::ofstream _NNfile;
    SFERES_STAT(BestFitVal, BestFit)
    {
    public:
      
      //float bob;
      template<typename E>
	void refresh(const E& ea)
      {
	assert(!ea.pop().empty());
	float max_f = -std::numeric_limits<float>::max();
	float max_v = -std::numeric_limits<float>::max();
	float max_c = -std::numeric_limits<float>::max();
	float max_n = -std::numeric_limits<float>::max();
	float mean_eval=0.0;
	float mean_nb_conn=0.0;
	float mean_nb_neur=0.0;


#ifdef USE3OBJ
	int l =  ea.pop()[0]->fit().objs().size() - 2;
#else
	int l =  ea.pop()[0]->fit().objs().size() - 1;
#endif
	
	for (size_t i = 0; i < ea.pop().size(); ++i)
	  {
	    mean_eval+=ea.pop()[i]->fit()._nb_eval;
#if defined(DNN) || defined(EVONEURO)
	    mean_nb_conn+=ea.pop()[i]->nn().get_nb_connections();
	    mean_nb_neur+=ea.pop()[i]->nn().get_nb_neurons();
#endif
	    float v = ea.pop()[i]->fit().value();
	    float c = ea.pop()[i]->fit().obj(l);
	    float f = ea.pop()[i]->fit().obj(0);
#ifdef USE3OBJ
	    int l =  ea.pop()[0]->fit().objs().size() - 1;
#endif
	    float n = ea.pop()[i]->fit().obj(l);
	    if (v > max_v)
	      {
		this->_best = ea.pop()[i];
		max_v = v;
	      }
	    if (c > max_c)
	      max_c = c;
	    if (n > max_n)
	      max_n = n;
	    if (f > max_f)
	      max_f = f;
	  }
	mean_eval/=ea.pop().size();
	mean_nb_conn/=ea.pop().size();
	mean_nb_neur/=ea.pop().size();
	this->_create_log_file(ea, "bestfitval.dat");
	assert(this->_best);

	// Display stats about neural networks structures
#ifdef DNN
	if(!_NNfile) {
	  std::string filename =ea.res_dir()+"/bestfitNN.dat";
	  _NNfile.open(filename.c_str());
	}
	this->_best->nn().write(_NNfile);
#elif defined(EVONEURO)
	if(!_NNfile) {
	  std::string filename =ea.res_dir()+"/bestfitNN.dat";
	  _NNfile.open(filename.c_str());
	}
	this->_best->show(_NNfile);
#endif
	// Display objectives etc...
	if (ea.dump_enabled())
	  (*this->_log_file) << ea.gen() << " " << this->_best->fit().value() << " " << mean_eval<<" "<<mean_nb_conn<< " " <<mean_nb_neur<< " " << max_c << " " << max_n << " " << max_f << std::endl;
      }
    protected:
      //std::ofstream _NNfile;
    };

    SFERES_STAT(PopBehaviorsAdhoc, Stat)
    {
    public:
      template<typename E>
	void refresh(const E& ea)
      {
	this->_create_log_file(ea, "pop_behaviors_adhoc.dat");

	for (size_t i = 0; i < ea.pop().size(); ++i)
	  {
	    (*this->_log_file)<<ea.gen()<< " ";
	    for (size_t k=0;k<ea.pop()[i]->fit()._behavior_adhoc.size();k++) {
	      (*this->_log_file)<<ea.pop()[i]->fit()._behavior_adhoc[k]<<" ";
	    }

	    (*this->_log_file)<<std::endl;				}
      } 
    };

#ifdef LOGTRAJECTORY
    SFERES_STAT(TrajStats, Stat)
    {
    public:
      template<typename E>
	void refresh(const E& ea)
      {
	this->_create_log_file(ea, "pop_traj.dat");

	for (size_t i = 0; i < ea.pop().size(); ++i)
	  {
	    (*this->_log_file)<<ea.gen()<< " ";
	    for (size_t k=0;k<ea.pop()[i]->fit()._trajectory.size();k++) {
	      (*this->_log_file)<<ea.pop()[i]->fit()._trajectory[k].x<<" "<<ea.pop()[i]->fit()._trajectory[k].y<<" "<<ea.pop()[i]->fit()._trajectory[k].ball<<" ";
	    }

	    (*this->_log_file)<<std::endl;				}
      } 
    };
#endif

    SFERES_STAT(FilStats, Stat)
    {
    public:
      template<typename E>
	void refresh(const E& ea)
      {
	std::cout<<"print!"<<std::endl;
	this->_create_log_file(ea, "pop_tree.dat");
	(*this->_log_file)<<"new gen - "<<ea.gen()<<std::endl;
	for (size_t i = 0; i < ea.pop().size(); ++i) {
	  float nb_conn = ea.pop()[i]->nn().get_nb_connections();
	  float nb_neur = ea.pop()[i]->nn().get_nb_neurons();
	  (*this->_log_file)<<i<<" "<<ea.pop()[i]->fit().number()<<" "<<ea.pop()[i]->fit().father()<<" ";
	  for(size_t j = 0; j <ea.pop()[i]->fit().objs().size(); j++)
	    (*this->_log_file)<<ea.pop()[i]->fit().obj(j)<<" ";
	  (*this->_log_file)<<nb_conn<<" "<<nb_neur<<std::endl;
	}
	(*this->_log_file)<<std::endl;				
      } 
    };

  }
}
