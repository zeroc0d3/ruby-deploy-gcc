#!/usr/bin/env sh

ENV='staging'

BUNDLE=`which bundle`
PWD_PUMA=`which puma`

if [ "$ENV" = "production" ]
then
  PWD_ROOT='/home/deploy/rb_deploy/current'
  CFG_PUMA='/home/deploy/rb_deploy/current/config/puma/production.rb'
else
  PWD_ROOT='/home/zeroc0d3/zeroc0d3-deploy/current'
  CFG_PUMA='/home/zeroc0d3/zeroc0d3-deploy/current/config/puma/staging.rb'
fi

cd $PWD_ROOT
RAILS_ENV=$ENV $BUNDLE exec $PWD_PUMA -C $CFG_PUMA $PWD_ROOT/config.ru
