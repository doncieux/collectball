#ifndef _FIT_COLLECTBALL_HPP
#define _FIT_COLLECTBALL_HPP

namespace sferes
{

  // ********** Main Class ***********
  SFERES_FITNESS(FitCollectBall, sferes::fit::Fitness)
  {
  public:
    FitCollectBall():_number(0),_father(0),tracefile(NULL) { }

	// Accessor/selectors for managing the filiation 
    void set_number(int number) { _number = number; }
    void set_new_number() {
      static long current_number=1;
      _number = current_number++;
    }
    void set_father(int father) { _father = father; }
    int father() { return _father; }
    int number() { return _number; }

		
    // *************** _eval ************
    //
    // This is the main function to evaluate the individual
    // It runs fastsim (simu_t simu)
    // 
    // **********************************
    template<typename NN>
      void _eval(NN &nn)
    { 


#ifdef VERBOSE
	std::cout<<"Eval ..."<<std::endl;
#endif

      // Init of the number of fitness objectives
      this->_objs.resize(nb_fit);
      inputs.resize(Params::dnn::nb_inputs);
      

      watch=true;

      typedef simu::Fastsim<Params> simu_t;
      simu_t simu;
      using namespace fastsim;	
      
      // Parameters of the evaluation
      nbinst=3;
      nb_balls = 8;

      // *** Init of behavior descriptors ***

      // adhoc
      _behavior_adhoc = Eigen::VectorXf::Zero(nb_balls*nbinst*2); 

      // entropy      
#if defined(ENTROPY) || defined(MULTIDIST)
      _nbv_ent=0;
      for(int i=0; i < Params::fitness::nb_io; i++) {
        std::vector<float> temp;
        _p.push_back(temp);
      }
      for(int i=0; i < _p.size(); i++) {
        unsigned int nbv=i<3?10:(i<12?2:10);
        _p[i].insert(_p[i].begin(),nbv,0.0);        
      }
#endif

      init_simu(simu);
      

      // *** Main Loop *** 
      for (size_t i = 0; i < Params::simu::nb_steps && !stop_eval; ++i)
	{	    

	  // Number of steps the robot is evaluated
	  _nb_eval=i;
	  
	  // Starting position for the 3 instances
	  instance(simu, nn, i);
	  
	  // Update robot info 
	  update_infos(simu);
	  
	  // Get inputs
	  get_inputs(simu);
	  
	  // Step  neural network -- outf is the output vector.
	  step_check(nn);
	  
	  // Append data describing the robot behavior
	  add_history_value(simu, outf, i);
	  
	  // check if any action is possible - collect/drop ball ; use switch 
	  doaction(simu);
	  
	  // move the robot and check for collision and if is still 
	  move_check(simu);
	  
	} 


    // Update entropy
#if defined(ENTROPY) || defined(MULTIDIST)
    _entropy=Eigen::VectorXf::Zero(_p.size());
    for(unsigned int i=0;i<_p.size();i++) {
      _entropy[i]=0;
      for (unsigned int j=0;j<_p[i].size();j++){
        _p[i][j]/=(float)_nbv_ent;
        if (_p[i][j]>0.0000000000001)
          _entropy[i]+= - _p[i][j] * log(_p[i][j])/log(_p[i].size());
      }
    }
#endif


    // Update objectives
      unsigned int i;
      for (i=0;i<nb_fit;i++) {
	     this->_objs[i]=0;
      }

      // Ball Objective 
      this->_objs[ballcount] = nb_collected/(1.0*nbinst*nb_balls); 
      this->_value = nb_collected/(1.0*nbinst*nb_balls); 

      // Diversity/novelty objectives are updated elsewhere (in a modifier)

#ifdef VERBOSE
      static int nbeval=0;
      std::cout<<"fit="<<this->_objs[0]<<" nb_collected="<<nb_collected<<" eval_length="<<_nb_eval<<" nbeval="<<nbeval<<std::endl;
      nbeval++;
#endif

    } // *** end of eval ***

    
    template<typename Simu>
      void init_simu(Simu& simu)
    {
      
      /*
	VARIANTS :
	* VARIANT_SW1: the balls do not appear at first, only one ball appears, one after the other, when the switch is pushed
	* VARIANT_SW2: the balls do not appear at first, only one ball appears, each time in the long room (the farthest position possible)

       */

     	
      //Visualisation mode
#ifdef VISU
	  simu.init_view(true);      
#elif !defined(NO_VISU)			
	if(this->mode() == fit::mode::view)
	  simu.init_view(true);      
#endif

      //Position balls, basket, etc...

      prepare_env(simu);

      // the behavior is described by the final position of the balls (x, y coordinates).
      // The behavior adhoc is not always used for the selection, but it is always dumped to compare behavior exploration in the different setups.
      

      ball_number=-1; // 0 to 3, -1 if not initialized
      for (ball_number_base=0;ball_number_base<nbinst;ball_number_base++) { 
	for (size_t k = 0; k < simu.map()->get_illuminated_switches().size(); ++k) {
	  if (simu.map()->get_illuminated_switches()[k]->get_color() == BALL
	      && simu.map()->get_illuminated_switches()[k]->get_on()) {
	    _behavior_adhoc[2*(nb_balls*ball_number_base+k)+0] = simu.map()->get_illuminated_switches()[k]->get_x();
	    _behavior_adhoc[2*(nb_balls*ball_number_base+k)+1] = simu.map()->get_illuminated_switches()[k]->get_y();  
	  }
	}
      }
      ball_number_base=0;

      
      
      fit = 1e10;
      stop_eval = false;
      opened = false;
      _carrying_ball=false;
      _carried_ball=false;
      nb_collected=0;
      stand_still=0;

    }
		

    template<typename Simu>
      void reinit_simu(Simu& simu)
    {
      using namespace fastsim;
      
      simu.robot().reinit();

#if defined(VARIANT_SW1) || defined(VARIANT_SW2)
      swtimer=0;
      // reinit the balls
      for (size_t k = 0; k < _nb_ball_to_appear; ++k) {
        simu.map()->get_illuminated_switches()[k]->set_on(false);
      }      
      // We switch on the switch so that the robot can go and activate it again to make a new ball appear
      simu.map()->get_illuminated_switches()[simu.map()->get_illuminated_switches().size()-1]->set_on(true);
#else
      //Reinit map
      if(opened) 
	{
	  opened=false;
	  simu.reset_map();

#ifdef VISU
	  simu.refresh_map_view();
#elif !defined(NO_VISU)
	  if(this->mode() == fit::mode::view)
	    simu.refresh_map_view();
#endif

	}
#ifdef VERBOSE
      std::cout<<"Reinit the balls"<<std::endl;
#endif
      // reinit the balls
      for (size_t k = 0; k < simu.map()->get_illuminated_switches().size(); ++k) {
        simu.map()->get_illuminated_switches()[k]->set_on(true);
      }      
#endif
    }		
 
 
    // Prepare environment (add all the objects in the arena)
    template<typename Simu>
      void prepare_env(Simu& simu)
    {
      using namespace fastsim;
	
      simu.init();
      // *** Preparing the environment
	
      //Sizes
      width=simu.map()->get_real_w();
      height=simu.map()->get_real_h();
	
      oldwidth=width-195;
      oldorigin=195;
      oldheight=405;

      // Sensors:
      // * 3 lasers range sensors

      //right
      simu.robot().add_laser(Laser(M_PI / 4.0, 8.f*simu.robot().get_radius()*2.f));

      // left
      simu.robot().add_laser(Laser(-M_PI / 4.0, 8.f*simu.robot().get_radius()*2.f));

      //middle
      simu.robot().add_laser(Laser(0.0f, 8.f*simu.robot().get_radius()*2.f));
	
      // * 2 ball sensors (ball=light of color 0)

      //right
      simu.robot().add_light_sensor(LightSensor(BALL,  M_PI / 4.0, M_PI / 2.0 + 0.01));

      //left
      simu.robot().add_light_sensor(LightSensor(BALL, -M_PI / 4.0, M_PI / 2.0 + 0.01));
      // * 2 basket sensors (basket surrounded by lights of color 1)
      //right
      simu.robot().add_light_sensor(LightSensor(BASKET,  M_PI / 4.0, M_PI / 2.0 + 0.01));
      // left
      simu.robot().add_light_sensor(LightSensor(BASKET, -M_PI / 4.0, M_PI / 2.0 + 0.01));
      
      // * 2 switch sensors 
      //right
      simu.robot().add_light_sensor(LightSensor(SWITCH,  M_PI / 4.0, M_PI / 2.0 + 0.01));

      //left
      simu.robot().add_light_sensor(LightSensor(SWITCH, -M_PI / 4.0, M_PI / 2.0 + 0.01));


      // * 2 bumpers
      // * 1 carrying object sensor
      // -> all these three sensors are directly computed below, no dedicated sensor.
	
#ifdef VARIANT_SW1
      // No balls at first in the variant -> we place the balls, but
      // switch them off
      
      // Place balls
      Map::ill_sw_t s1 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10,oldorigin+oldwidth/4.0,      oldheight/8.0,     false));
      simu.map()->add_illuminated_switch(s1);
      Map::ill_sw_t s2 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10,oldorigin+oldwidth/4.0,  3.0*oldheight/8.0,     false));
      simu.map()->add_illuminated_switch(s2);
      Map::ill_sw_t s3 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin+oldwidth/2.0,     oldheight/8.0,     false));
      simu.map()->add_illuminated_switch(s3);
      Map::ill_sw_t s4 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin+oldwidth/2.0, 3.0*oldheight/8.0,     false));
      simu.map()->add_illuminated_switch(s4);
	
      // Placing bonus balls
      Map::ill_sw_t s5 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin/2.0,      oldheight/8.0,     false));
      simu.map()->add_illuminated_switch(s5);
      Map::ill_sw_t s6 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin/2.0,  3.0*oldheight/8.0,     false));
      simu.map()->add_illuminated_switch(s6);
      Map::ill_sw_t s7 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin/2.0, 5.0*oldheight/8.0,     false));
      simu.map()->add_illuminated_switch(s7);
      Map::ill_sw_t s8 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin/2.0, 7.0*oldheight/8.0,     false));
      simu.map()->add_illuminated_switch(s8);
      
      
      
      opened=true;
      simu.switch_map();

      _next_ball_to_appear=0;
      _nb_ball_to_appear=8;
      
#elif defined(VARIANT_SW2)
      // No balls at first in the variant -> we place one ball only and switch it off
      
	
      // Placing bonus balls
      Map::ill_sw_t s5 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin/2.0,      oldheight/8.0,     false));
      simu.map()->add_illuminated_switch(s5);
      
      _next_ball_to_appear=0;
      _nb_ball_to_appear=1;      
      
      opened=true;
      simu.switch_map();


#else
      // Place balls
      Map::ill_sw_t s1 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10,oldorigin+oldwidth/4.0,      oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s1);
      Map::ill_sw_t s2 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10,oldorigin+oldwidth/4.0,  3.0*oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s2);
      Map::ill_sw_t s3 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin+oldwidth/2.0,     oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s3);
      Map::ill_sw_t s4 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin+oldwidth/2.0, 3.0*oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s4);
	
      // Placing bonus balls
      Map::ill_sw_t s5 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin/2.0,      oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s5);
      Map::ill_sw_t s6 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin/2.0,  3.0*oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s6);
      Map::ill_sw_t s7 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin/2.0, 5.0*oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s7);
      Map::ill_sw_t s8 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin/2.0, 7.0*oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s8);

#endif
	
      // Placing the basket "lights"
      Map::ill_sw_t s9 = Map::ill_sw_t(new IlluminatedSwitch(BASKET,10,oldorigin+oldwidth/6.0,          oldheight-20.0, true));
      simu.map()->add_illuminated_switch(s9);
      Map::ill_sw_t s10 = Map::ill_sw_t(new IlluminatedSwitch(BASKET,10,oldorigin+2.0*oldwidth/6.0,      oldheight-20.0, true));
      simu.map()->add_illuminated_switch(s10);
      Map::ill_sw_t s11 = Map::ill_sw_t(new IlluminatedSwitch(BASKET,10,oldorigin+4.0*oldwidth/18.0,     oldheight-20.0, true));
      simu.map()->add_illuminated_switch(s11);
      Map::ill_sw_t s12 = Map::ill_sw_t(new IlluminatedSwitch(BASKET,10,oldorigin+5.0*oldwidth/18.0,     oldheight-20.0, true));
      simu.map()->add_illuminated_switch(s12);
	
      // Placing the switch
      Map::ill_sw_t s13 = Map::ill_sw_t(new IlluminatedSwitch(SWITCH,10,oldorigin+15.0*oldwidth/18.0,     oldheight/10.0, true));
      simu.map()->add_illuminated_switch(s13);

      basket_x=oldorigin+3.0*oldwidth/12.0;
      basket_y=oldheight;
      basket_radius=oldwidth/12.0;
			
#ifdef VERBOSE
      std::cout<<"Basket: x="<<basket_x<<" y="<<basket_y<<" radius="<<basket_radius<<std::endl;
#endif

      old_pos=simu.robot().get_pos();
      //std::cout<<"Preparing the environment. Width="<<width<<" height="<<height<<std::endl;
	
    }

    // *** Set initial position of robot for all instances
    template<typename Simu, typename NN>
      void instance(Simu &simu, NN &nn, int num_eval) 
    {
      using namespace fastsim;
      //Initial Positions
      if (num_eval==0) {
	simu.robot().set_pos(Posture(oldorigin+oldwidth/4.0, oldheight/4.0, M_PI/4.0));
	simu.robot().move(0,0,simu.map());
	nn.init();
	_carrying_ball=false;
	_carried_ball=false;
	reinit_simu(simu);
	ball_number_base=0;
      }
      else if (num_eval==Params::simu::nb_steps/3) {
	if (this->mode() == fit::mode::view)
	  std::cerr<<"====== end of eval ======"<<std::endl;
	simu.robot().set_pos(Posture(oldorigin+3.0*oldwidth/4.0, 3.0*oldheight/4.0, -3.0*M_PI/4.0));
	simu.robot().move(0,0,simu.map());
	nn.init();
	_carrying_ball=false;
	_carried_ball=false;
	ball_number_base=1;
	reinit_simu(simu);
      }
      else if (num_eval==2*Params::simu::nb_steps/3) {
	if (this->mode() == fit::mode::view)
	  std::cerr<<"====== end of eval ======"<<std::endl;
	simu.robot().set_pos(Posture(oldorigin+oldwidth/4.0, 3.0*oldheight/4.0, -M_PI/4.0));
	simu.robot().move(0,0,simu.map());
	nn.init();
	_carrying_ball=false;
	_carried_ball=false;
	ball_number_base=2;
	reinit_simu(simu);				
      }
    }
    // *** Refresh infos about the robot (position, color)
    template<typename Simu>
      void update_infos(Simu &simu) {
      // refresh
      using namespace fastsim;
      simu.refresh();
				
#ifdef VISU
      if (1)
#elif defined(NO_VISU)
      if (0)
#else
      if (this->mode() == fit::mode::view)
#endif
	  {
	    if (_carrying_ball) {
	      simu.robot().set_color(3);
	    }
	    else {
	      simu.robot().set_color(7);
	    }
	    simu.refresh_view();
	  }
	
    }

    // *** Get sensors inputs
    template<typename Simu>
      void get_inputs(Simu &simu) 
    {
      using namespace fastsim;
      // Update of the sensors
      size_t nb_lasers = simu.robot().get_lasers().size();
      size_t nb_light_sensors = simu.robot().get_light_sensors().size();
	  
      // *** set inputs ***
      std::vector<float> dlasers;
      for (size_t j = 0; j < nb_lasers; ++j)
	{
	  float d = simu.robot().get_lasers()[j].get_dist();
	  //std::vector<float> d(simu.robot().get_lasers()[j].get_dist());
	  dlasers.push_back(simu.robot().get_lasers()[j].get_dist());
	  float range = simu.robot().get_lasers()[j].get_range();
	  inputs[j] = (d == -1 ? 0 : 1 - d / range);
	} 
	
      for (size_t j = 0; j < nb_light_sensors; j+=2)
	{
	  bool actL = simu.robot().get_light_sensors()[j].get_activated();
	  bool actR = simu.robot().get_light_sensors()[j+1].get_activated();
	  inputs[nb_lasers+j] = actL ? 1 : 0;
	  inputs[nb_lasers+j+1] = actR ? 1 : 0;
	}

      inputs[nb_lasers+nb_light_sensors+0] = simu.robot().get_left_bumper() ? 1 : 0;
      inputs[nb_lasers+nb_light_sensors+1] = simu.robot().get_right_bumper() ? 1 : 0;
      inputs[nb_lasers+nb_light_sensors+2] = _carrying_ball ? 1 : 0;//carrying ball
    }

    // *** Step Neural Network and various checks
    template<typename NN>
      void step_check(NN &nn) 
    {
      nn.step(inputs);
      const std::vector<float>& outf2 = nn.get_outf();
      outf = outf2;
      assert(outf.size() == 3);
	
      for(size_t j = 0; j < outf.size(); j++)
	if(std::isnan(outf[j]))
	  outf[j] = 0.0;
      //std::cout<<"Outf: "<<outf[0]<<" "<<outf[1]<<" "<<outf[2]<<std::endl;

      if (tracefile) {
	*tracefile<<"nn s ";
	for (size_t j=0;j<inputs.size();j++) {
	  *tracefile<<inputs[j]<<" ";
	}
	*tracefile<<" e ";
	for (size_t j=0;j<outf.size();j++) {
	  *tracefile<<outf[j]<<" ";
	}
	*tracefile<<std::endl;
      }

      if (this->mode() == fit::mode::view) {
	std::cerr<<"nn s ";
	for (size_t j=0;j<inputs.size();j++) {
	  std::cerr<<inputs[j]<<" ";
	}
	std::cerr<<" e ";
	for (size_t j=0;j<outf.size();j++) {
	  std::cerr<<outf[j]<<" ";
	}
	std::cerr<<std::endl;
      }

    }

    // *** Check if acton is possible and perform it
    template<typename Simu>
      void doaction(Simu &simu) 
    {
      int r=0;
      using namespace fastsim;
      int state_for_display=_carrying_ball; // used for logging purposes

      float rx=simu.robot().get_pos().x();
      float ry=simu.robot().get_pos().y();


#if defined(VARIANT_SW1) || defined(VARIANT_SW2)
      // We switch on the switch so that the robot can go and activate it again to make a new ball appear
      if (swtimer==0) {
	simu.map()->get_illuminated_switches()[simu.map()->get_illuminated_switches().size()-1]->set_on(true);
      }
      else swtimer--;
#endif


      if (outf[2]<0.5) {
	if (_carrying_ball) {
	  

#ifdef VERBOSE
	  std::cout<<"Ball launched. x="<<rx<<" ry="<<ry<<std::endl;
#endif
	  _carrying_ball=false;
	  if (ball_number==-1) {
	    //std::cerr<<"WARNING: ball_number==-1 there must be a bug somewhere..."<<std::endl;
	    ball_number=0;
	  }
	  
	  _behavior_adhoc[2*(nb_balls*ball_number_base+ball_number)+0] = rx;
	  _behavior_adhoc[2*(nb_balls*ball_number_base+ball_number)+1] = ry;
	    
	  float xx = rx-basket_x;
	  float yy = ry-basket_y;
	  float dist = sqrtf(xx * xx + yy * yy);
	  if (simu.robot().get_collision()&&dist<=basket_radius) { // if near enough from the basket
	    nb_collected++;
	    //if(i<Params::fitness::nb_step_watch) 
	    r=1;
#ifdef VERBOSE
	    std::cout<<"Ball collected !!"<<std::endl;
#endif
	  }
	}
      }
      else {
	if (!_carrying_ball) {
	  // is there a ball nearby ?
	  simu.map()->get_illuminated_switches();
	  for (size_t k = 0; k < simu.map()->get_illuminated_switches().size(); ++k) {
	    if (simu.map()->get_illuminated_switches()[k]->get_color() == BALL
		&& simu.map()->get_illuminated_switches()[k]->get_on()) {
	      //std::cout<<"Ball"<<std::endl;
	      // This is a valid ball, i.e. a ball that hasn't been taken yet
	      float xx = rx-simu.map()->get_illuminated_switches()[k]->get_x();
	      float yy = ry-simu.map()->get_illuminated_switches()[k]->get_y();
	      float dist = sqrtf(xx * xx + yy * yy);

	      if (dist<simu.robot().get_radius()){
		// the ball is within range
#ifdef VERBOSE
		std::cout<<"Ball taken. x="<<rx<<" ry="<<ry<<std::endl;
#endif
		//if(i<Params::fitness::nb_step_watch) 
		_carrying_ball=true;
		_carried_ball=true;
		simu.map()->get_illuminated_switches()[k]->set_on(false);
		ball_number=k;
		break;
	      }
	    }
	    // *** Press Switch ***
	    if (simu.map()->get_illuminated_switches()[k]->get_color() == SWITCH && simu.map()->get_illuminated_switches()[k]->get_on()) {
	      float xx = rx-simu.map()->get_illuminated_switches()[k]->get_x();
	      float yy = ry-simu.map()->get_illuminated_switches()[k]->get_y();
	      float dist = sqrtf(xx * xx + yy * yy);
	      if (dist<simu.robot().get_radius()){
            simu.map()->get_illuminated_switches()[k]->set_on(false);


#if defined(VARIANT_SW1) || defined(VARIANT_SW2)
            if (_next_ball_to_appear<_nb_ball_to_appear) {
#ifdef VERBOSE
	      std::cout<<"Ball "<<_next_ball_to_appear<<" set to appear (x="<<simu.map()->get_illuminated_switches()[_next_ball_to_appear]->get_x()<<", y="<<simu.map()->get_illuminated_switches()[_next_ball_to_appear]->get_y()<<")"<<std::endl;
#endif
              simu.map()->get_illuminated_switches()[_next_ball_to_appear]->set_on(true);
#ifdef VARIANT_SW1
              _next_ball_to_appear++;
#endif
            }
            simu.map()->get_illuminated_switches()[k]->set_on(false);
	    swtimer=100;
#else
	    if (this->mode() == fit::mode::view) {
	      std::cout<<"Switch"<<std::endl;
	    }
            // the ball is within range
            state_for_display = 2;
            
            
            opened=true;
            simu.switch_map();
            
#ifdef VISU
            simu.refresh_map_view();
#elif !defined(NO_VISU)
            if(this->mode() == fit::mode::view)
              simu.refresh_map_view();
#endif
#endif
            break;
	      }
	    }
	  }
	}
      }
      
      if (tracefile) {
        *tracefile<<"p "<<simu.robot().get_pos().x()<<" "<<simu.robot().get_pos().y()<<" "<<simu.robot().get_pos().theta()<<" "<<state_for_display<<std::endl;
        *tracefile<<"r "<<r<<std::endl;	
      }

      if (this->mode() == fit::mode::view) {
        std::cerr<<"p "<<simu.robot().get_pos().x()<<" "<<simu.robot().get_pos().y()<<" "<<simu.robot().get_pos().theta()<<" "<<state_for_display<<std::endl;
        std::cerr<<"r "<<r<<std::endl;
      }


    }

    // *** Move and check if robot is colliding, or still
    template<typename Simu>
      void move_check(Simu &simu) 
    {
      using namespace fastsim;
      // *** move robot ***
      simu.move_robot(4.0*(outf[0] * 2.0 - 1.0), 4.*(outf[1] * 2.0 - 1.0));
	
      // *** Check if robot is stuck ***
      if ((old_pos.dist_to(simu.robot().get_pos())<0.0001)&&
	  (fabs(old_pos.theta()-simu.robot().get_pos().theta())<0.0001))
	{
	  stand_still++;
	  if (stand_still>100) {
	    stop_eval=true;
	    //if(i<Params::fitness::nb_step_watch)
#ifdef VERBOSE
	    std::cout<<"Still robot, we stop the eval..."<<std::endl;
#endif

	    for (size_t j=_nb_eval+1;j<Params::simu::nb_steps;j++) {
	      add_history_value(simu,outf,j);
	    }
	  }
	}
      old_pos=simu.robot().get_pos();
    }

    // *** Keep track of robot behavior
    template<typename Simu>
      void add_history_value(Simu &simu,std::vector<float>& outf, int num_eval) 
    {

      if (!watch) return;

      float rx=simu.robot().get_pos().x();
      float ry=simu.robot().get_pos().y();


#if defined(HAMMING) || defined(ENTROPY) || defined(MULTIDIST)
      std::bitset<Params::fitness::nb_io> sv;    // binarized sensorimotor stream
      std::vector<float> svf(Params::fitness::nb_io);                      // sensorimotor stream

      size_t ind=0;
      _nbv_ent++;

       int nb_lasers=simu.robot().get_lasers().size();
       int nb_light_sensors=simu.robot().get_light_sensors().size();

      for (size_t j = 0; j < nb_lasers; ++j)
	     {
	       float d = simu.robot().get_lasers()[j].get_dist();
	       float range = simu.robot().get_lasers()[j].get_range();
          d=d>range?range:d;
	       svf[ind]=(d == -1 ? 0 : 1 - d / range);
#if defined(ENTROPY) || defined(MULTIDIST)
         int ind_v=(int)(floor(svf[ind]*10.0*0.999));
         //
         if (ind_v>=_p[ind].size()){
          std::cerr<<"WARNING: d="<<d<<" svf[ind]="<<svf[ind]<<" range="<<range<<" Updating _p["<<ind<<"]["<<ind_v<<"] _p["<<ind<<"].size()="<<_p[ind].size()<<std::endl;
         }
         _p[ind][ind_v] += 1.0;
#endif
	  //svf[ind]=d/range;
	       ind++;
	       float inp = (d == -1 ? -1 : 1 - 2 * d / range);
	       sv[j]=inp>0 ? 1:0;


	     }


      for (size_t j = 0; j < nb_light_sensors; ++j)
	     {
	       sv[j+nb_lasers] = simu.robot().get_light_sensors()[j].get_activated()? 1.0 : 0.0;
	       svf[ind]=simu.robot().get_light_sensors()[j].get_activated()? 1.0 : 0.0;
#if defined(ENTROPY) || defined(MULTIDIST)
         _p[ind][(int)(svf[ind])] += 1.0;
#endif
         ind++;
	     }
      sv[nb_lasers+nb_light_sensors+0] = simu.robot().get_left_bumper() ? 1 : 0;
      sv[nb_lasers+nb_light_sensors+1] = simu.robot().get_right_bumper() ? 1 : 0;
      sv[nb_lasers+nb_light_sensors+2] = _carrying_ball ? 1 : 0;

      svf[ind]=simu.robot().get_left_bumper() ? 1.0 : 0.0;
#if defined(ENTROPY) || defined(MULTIDIST)
      _p[ind][(int)(svf[ind])] += 1.0;
#endif
      ind++;
      svf[ind]=simu.robot().get_right_bumper() ? 1.0 : 0.0;
#if defined(ENTROPY) || defined(MULTIDIST)
      _p[ind][(int)(svf[ind])] += 1.0;
#endif
      ind++;
      svf[ind]=_carrying_ball ? 1.0 : 0.0;
#if defined(ENTROPY) || defined(MULTIDIST)
      _p[ind][(int)(svf[ind])] += 1.0;
#endif
      ind++;

      for (size_t j = 0; j < outf.size(); ++j)
	     {
	       sv[j+nb_lasers+nb_light_sensors+3]=outf[j]>0.5 ? 1:0; //outf is between 0 and 1, it is put back between -1 and 1 afterwards.
	       svf[ind]=outf[j];
#if defined(ENTROPY) || defined(MULTIDIST)
         int ind_v=(int)(floor(svf[ind]*10.0*0.999));
         if (ind_v>=_p[ind].size()){
          std::cerr<<"WARNING: svf[ind]="<<svf[ind]<<" Updating _p["<<ind<<"]["<<ind_v<<"] _p["<<ind<<"].size()="<<_p[ind].size()<<" line="<<__LINE__<<std::endl;
         }
         _p[ind][ind_v] += 1.0;
#endif
         ind++;
	     }


      //_behavior: binarized sensorimotor stream (during nb_step_watch steps)
#if defined(HAMMING) || defined(MULTIDIST)
      if(num_eval<Params::fitness::nb_step_watch) 
	       _behavior_binary.push_back(sv);
#endif

#endif 

#if defined(TRAJECTORY) || defined(MULTIDIST)
      if(num_eval<Params::fitness::nb_step_watch) {
	     if(num_eval%50==0) {
	       struct point_traj up;
	       up.x=(int)(rx*NB_TILES/simu.map()->get_real_w());
	       up.y=(int)(ry*NB_TILES/simu.map()->get_real_h());
	       up.ball=_carrying_ball;
	       if(up.x>12 || up.x<0 || up.y>12 || up.y<0) 
	         std::cout<<"Problem! "<<up.x<<" "<<up.y<<" "<<simu.map()->get_real_w()<<" "<<rx<<" "<<stop_eval<<std::endl;
	       else
	    _trajectory.push_back(up);
	}
      }
#endif 

    } // *** end of add_history_value ***


    void set_tracefile(std::ofstream *df) {tracefile=df;}

#if defined(VARIANT_SW1) || defined(VARIANT_SW2)
    unsigned int _next_ball_to_appear;
    unsigned int _nb_ball_to_appear;
    int swtimer;
#endif



    int _number, _father;
    bool stop_eval;                                  // Stops the evaluation

    // Environment features
    int ball_number, ball_number_base;
    float width, height, oldwidth, oldheight, oldorigin, basket_x, basket_y, basket_radius, fit;
    int nb_balls, nbinst;
    bool opened;                                     // Is the door opened ?

    // Robot state and features
    unsigned int nb_collected, stand_still;
    fastsim::Posture old_pos;
    bool _carrying_ball;                             // Is the robot carrying a ball?
    bool _carried_ball;
    std::vector<float> outf, inputs;

    int _nb_eval;                                    // Number of evaluation (until the robot stands still)

    bool watch; // log values in history. Default to true, equals false when calling eval_visu

    Eigen::VectorXf _behavior_adhoc;                 // Ad Hoc Behavior
    std::vector< std::bitset<Params::fitness::nb_bits> > _behavior_binary;      // binarized sensorimotor stream (for hamming behavioral distance)
    std::vector<struct point_traj> _trajectory;                                // vector of discretized positions (for the behavioral distance based on robot trajectory)
    std::vector<std::vector<float> > _p;                                       // entropy: probability of each input/output
    Eigen::VectorXf _entropy;                                                  // entropy
    int _nbv_ent;                                                              // entropy: nb of values to normalize probability

    std::ofstream *tracefile;
  };
	

  // ****************** Fitness ELMAN *************************
  SFERES_FITNESS(FitCollectBallElman, FitCollectBall)
  {
  public:
    template<typename Indiv>
      float dist(const Indiv& o) const 
    {
      return SFERES_PARENT(FitCollectBallElman, FitCollectBall)::dist(o);
    }
    template<typename Indiv>
      void eval(const Indiv& ind) 
    {
      using namespace nn;
      typedef Elman<Neuron<PfWSum<>, AfSigmoidNoBias<> >, Connection<> > elman_t;
      elman_t nn(Params::dnn::nb_inputs, Params::elman::nb_hidden, Params::dnn::nb_outputs); 
      nn.set_all_weights(ind.data());
      _genotype = ind.data();
      this->_eval(nn);
    }
  protected:
    float _sqr(float x) const { return x * x; }
    std::vector<float> _genotype;
  };

  // ****************** Fitness DNN *************************
  SFERES_FITNESS(FitCollectBallDnn, FitCollectBall)
  {
  public:
    template<typename Indiv>
      void eval(Indiv& ind) 
    {
      ind.nn().simplify();
      this->_eval(ind.nn());
    }
  };
  

}


#endif
