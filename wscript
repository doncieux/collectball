#! /usr/bin/env python

import sferes
from waflib.Configure import conf

def build(bld):



        sferes.create_variants(bld,
                           source='collectballhard.cpp',
                           use = 'sferes2 fastsim',
                           uselib = 'EIGEN SDL BOOST BOOST_GRAPH TBB SFERES2 FASTSIM',
                           target = 'collectballhard',
                           cxxflags = bld.get_env()['CXXFLAGS'] + ['-Wno-unknown-pragmas'],
                           json = 'collectballhard.json',
                           variants = [
                                 'ADHOC DIV_BEHAVIOR SAVEBMP',
                                 'ADHOC DIV_BEHAVIOR',
                                 'TRAJECTORY DIV_BEHAVIOR',
                                 'HAMMING DIV_BEHAVIOR',
                                 'ENTROPY DIV_BEHAVIOR',
                                 'DYNAMIC_DIVERSITY',
                                 'ADHOC NOV_BEHAVIOR',
                                 'TRAJECTORY NOV_BEHAVIOR',
                                 'HAMMING NOV_BEHAVIOR',
                                 'ENTROPY NOV_BEHAVIOR',
                                 'DYNAMIC_DIVERSITY',
                                 'ELMAN DYNAMIC_DIVERSITY',
                                 'DYNAMIC_DIVERSITY VARIANT_SW1',
                                 'DYNAMIC_DIVERSITY VARIANT_SW2'
                                       ])

