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

ps aux | grep -i puma | awk {'print $2'} | sudo xargs kill -9
