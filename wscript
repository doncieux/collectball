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
                           variants = [
                                 'MULTIDIST ARCHIVE PARETO', #SD: need to find a way to compute a crowding distance on NN
                                 'MULTIDIST ARCHIVE PARETO VARIANT_SW1',
                                 'MULTIDIST ARCHIVE PARETO VARIANT_SW2',
                                 'MULTIDIST ARCHIVE FITNESSNOVELTY',
                                 'MULTIDIST ARCHIVE FITNESSNOVELTY VARIANT_SW1',
                                 'MULTIDIST ARCHIVE FITNESSNOVELTY VARIANT_SW2',
                                 'MULTIDIST ARCHIVE FITNESSCURIOSITY',
                                 'MULTIDIST ARCHIVE FITNESSCURIOSITY VARIANT_SW1',
                                 'MULTIDIST ARCHIVE FITNESSCURIOSITY VARIANT_SW2',
                                 'MULTIDIST ARCHIVE FITNESS',
                                 'MULTIDIST ARCHIVE FITNESS VARIANT_SW1',
                                 'MULTIDIST ARCHIVE FITNESS VARIANT_SW2',
                                 'MULTIDIST ARCHIVE CURIOSITY',
                                 'MULTIDIST ARCHIVE CURIOSITY VARIANT_SW1',
                                 'MULTIDIST ARCHIVE CURIOSITY VARIANT_SW2'
                                       ])

