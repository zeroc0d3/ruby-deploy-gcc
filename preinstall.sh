#!/usr/bin/env sh

# ========== PRE-INSTALLATION SCRIPTS ========== # 
#  Do Not Remove This File !                     #
#  This is customize script before server up     # 
# ============================================== #

ENV='staging'
GEM=`which gem`
BUNDLE=`which bundle`

if [ "$ENV" = "production" ]
then
  PWD='/home/deploy/zeroc0d3lab/current'
else
  PWD='/home/deploy/zeroc0d3lab.dev/current'
fi

cd $PWD
$GEM install bundle
$BUNDLE install --binstubs