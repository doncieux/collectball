namespace sferes {
  template<typename Simu>
  void _prepare_simu(Simu& simu)
    {
      using namespace fastsim;
      
      simu.init();

      //Sizes
      float width=simu.map()->get_real_w();
      float height=simu.map()->get_real_h();		

#ifdef HARDMODE
      float oldwidth=width-195;
      float oldorigin=195;
      float oldheight=405;
#else
      float oldwidth=width;
      float oldorigin=0;
      float oldheight=height;
#endif

      // Sensors:
      // * 3 lasers range sensors
      simu.robot().add_laser(Laser(M_PI / 4.0, 8.f*simu.robot().get_radius()*2.f));
      simu.robot().add_laser(Laser(-M_PI / 4.0, 8.f*simu.robot().get_radius()*2.f));
      simu.robot().add_laser(Laser(0.0f, 8.f*simu.robot().get_radius()*2.f));

      // * 2 ball sensors (ball=light of color 0)
      simu.robot().add_light_sensor(LightSensor(BALL,  M_PI / 4.0, M_PI / 2.0 + 0.01));
      simu.robot().add_light_sensor(LightSensor(BALL, -M_PI / 4.0, M_PI / 2.0 + 0.01));
      // * 2 basket sensors (basket surrounded by lights of color 1)
      simu.robot().add_light_sensor(LightSensor(BASKET,  M_PI / 4.0, M_PI / 2.0 + 0.01));
      simu.robot().add_light_sensor(LightSensor(BASKET, -M_PI / 4.0, M_PI / 2.0 + 0.01));
      
#ifdef HARDMODE
      // * 2 switch sensors 
      simu.robot().add_light_sensor(LightSensor(SWITCH,  M_PI / 4.0, M_PI / 2.0 + 0.01));
      simu.robot().add_light_sensor(LightSensor(SWITCH, -M_PI / 4.0, M_PI / 2.0 + 0.01));
#endif
      // * 2 bumpers
      // * 1 carrying object sensor
      // -> all these three sensors are directly computed below, no dedicated sensor.
       
      // Place balls
      Map::ill_sw_t s1 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10,oldorigin+oldwidth/4.0,      oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s1);
      Map::ill_sw_t s2 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10,oldorigin+oldwidth/4.0,  3.0*oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s2);
      Map::ill_sw_t s3 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin+oldwidth/2.0,     oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s3);
      Map::ill_sw_t s4 = Map::ill_sw_t(new IlluminatedSwitch(BALL,10, oldorigin+oldwidth/2.0, 3.0*oldheight/8.0,     true));
      simu.map()->add_illuminated_switch(s4);

#ifdef HARDMODE
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

#ifdef HARDMODE
      // Placing the switch
      Map::ill_sw_t s13 = Map::ill_sw_t(new IlluminatedSwitch(SWITCH,10,oldorigin+15.0*oldwidth/18.0,     oldheight/10.0, true));
      simu.map()->add_illuminated_switch(s13);
#endif		

      //std::cout<<"Preparing the environment. Width="<<width<<" height="<<height<<std::endl;
					      
    }
}
