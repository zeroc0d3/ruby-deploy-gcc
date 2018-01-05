#!/usr/bin/env sh

# ========== PREINSTALLATION SCRIPTS ========== # 
#  Do Not Remove This File !                    #
#  This is customize script before server up    # 
# ============================================= #

ENV='staging'
GEM=`which gem`
BUNDLE=`which bundle`

if [ "$ENV" = "production" ]
then
  PWD='/home/deploy/rb_deploy/current'
else
  PWD='/home/zeroc0d3/zeroc0d3-deploy/current'
fi

cd $PWD
$GEM install bundle
$BUNDLE install --binstubs