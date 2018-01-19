#!/usr/bin/env sh

ENV='staging'

BUNDLE=`which bundle`
PWD_PUMA=`which puma`

if [ "$ENV" = "production" ]
then
  PWD_ROOT='/home/deploy/zeroc0d3lab/current'
  CFG_PUMA='/home/deploy/zeroc0d3lab/current/config/puma/production.rb'
else
  PWD_ROOT='/home/deploy/zeroc0d3lab.dev/current'
  CFG_PUMA='/home/deploy/zeroc0d3lab.dev/current/config/puma/staging.rb'
fi

cd $PWD_ROOT
RAILS_ENV=$ENV $BUNDLE exec $PWD_PUMA -C $CFG_PUMA $PWD_ROOT/config.ru
