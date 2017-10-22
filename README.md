# ruby-deploy-gcc
Ruby Deploy build with GCC

## Configuration
* Clone this repo
* Add `rb_deploy.c` and `make-rb_deploy` to your root path of Ruby or Rails project
* Setup Environment
  ```
  char ENV[64] = "production";   # for Production
  char ENV[64] = "development";  # for Development
  ```
* Setup Repository & Branch
  ```
  char REPO_NAME[1024] = "git@github.com:zeroc0d3/ruby-installation.git";
  char REPO_BRANCH[64] = "master";
  ```
* Setup Your Root Path
  ```
  char DEV_APP_ROOT[512]  = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary";  // Development Root Path
  char PROD_APP_ROOT[512] = "/home/zeroc0d3/deploy";                                 // Production Root Path
  ```
* Setup Your Binary Path
  Hint: `which [binary]` (eg: `which gem`, `which bundle`, `which unicorn`, `which rake`, `which rakeup`)
  ```
  char DEV_APP_ROOT[512]        = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary";                            // Development Root Path
  char DEV_CONFIG_UNICORN[512]  = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/config/unicorn/staging.rb";  // Development Unicorn Config
  char DEV_CONFIG_FAYE[512]     = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/faye.ru";                    // Development Faye Config
  char DEV_PID_UNICORN[512]     = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/tmp/pids/unicorn.pid";       // Development Path PID Unicorn
  char DEV_PID_FAYE[512]        = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/tmp/pids/faye.pid";          // Development Path PID Faye
  char DEV_PATH_UNICORN[512]    = "/home/zeroc0d3/.rbenv/shims/unicorn";                 // Development Path of Unicorn Binary
  char DEV_PATH_RAKE[512]       = "/home/zeroc0d3/.rbenv/shims/rake";                    // Development Path of Rake Binary
  char DEV_PATH_RACKUP[512]     = "/home/zeroc0d3/.rbenv/shims/rackup";                  // Development Path of Rackup Binary
  char DEV_PATH_GEM[512]        = "/home/zeroc0d3/.rbenv/shims/gem";                     // Development Path of Gem Binary
  char DEV_PATH_BUNDLE[512]     = "/home/zeroc0d3/.rbenv/shims/bundle";                  // Development Path of Bundle Binary

  // Production Environment
  char PROD_APP_ROOT[512]       = "/home/zeroc0d3/deploy";                               // Production Root Path
  char PROD_CONFIG_UNICORN[512] = "/home/zeroc0d3/deploy/config/unicorn/production.rb";  // Production Unicorn Config
  char PROD_CONFIG_FAYE[512]    = "/home/zeroc0d3/deploy/faye.ru";                       // Production Faye Config
  char PROD_PID_UNICORN[512]    = "/home/zeroc0d3/deploy/tmp/pids/unicorn.pid";          // Production Path PID Unicorn
  char PROD_PID_FAYE[512]       = "/home/zeroc0d3/deploy/tmp/pids/faye.pid";             // Production Path PID Faye
  char PROD_PATH_UNICORN[512]   = "/home/zeroc0d3/.rbenv/shims/unicorn";                 // Production Path of Unicorn Binary
  char PROD_PATH_RAKE[512]      = "/home/zeroc0d3/.rbenv/shims/rake";                    // Production Path of Rake Binary
  char PROD_PATH_RACKUP[512]    = "/home/zeroc0d3/.rbenv/shims/rackup";                  // Production Path of Rackup Binary
  char PROD_PATH_GEM[512]       = "/home/zeroc0d3/.rbenv/shims/gem";                     // Production Path of Gem Binary
  char PROD_PATH_BUNDLE[512]    = "/home/zeroc0d3/.rbenv/shims/bundle";                  // Production Path of Bundle Binary
  ```   

## Make Binary
* Build your binary deploy
  `./make-rb-deploy`

## Run
* Running 
  `./rb_deploy` [options]
