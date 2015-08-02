#! /usr/bin/env python

import sferes


def build(bld):



        sferes.create_variants(bld,
                           source='collectballhard.cpp',
                           uselib_local = 'sferes2 fastsim',
                           uselib = 'EIGEN3 SDL BOOST_GRAPH TBB',
                           target = 'collectballhard',
                           cxxflags = bld.get_env()['CXXFLAGS'] + ['-Wno-unknown-pragmas'],
                           json = 'collectballhard.json',
                           variants = [
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

