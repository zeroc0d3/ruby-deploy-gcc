# ruby-deploy-gcc
Ruby Deploy build with GCC

![alt text](https://github.com/zeroc0d3/ruby-deploy-gcc/blob/master/snapshot/deploy_process1.png)

```
==========================================================================
  ZeroC0D3 Ruby Deploy :: ver-1.2.9                                          
  (c) 2017 ZeroC0D3 Team                                                  
==========================================================================
  ### NGINX SERVICES ###                                                  
--------------------------------------------------------------------------
  # ./rb_deploy -nr            --> Restart NGINX                          
  # ./rb_deploy -no            --> Reload NGINX                           
--------------------------------------------------------------------------
  ### ASSETS SERVICES ###                                                 
--------------------------------------------------------------------------
  # ./rb_deploy -ap            --> Assets Precompile                      
  # ./rb_deploy -ac            --> Assets Clobber (Rollback)              
--------------------------------------------------------------------------
  ### RESTART SERVICES ###                                                
--------------------------------------------------------------------------
  # ./rb_deploy -ru            --> Restart Unicorn                        
  # ./rb_deploy -rf            --> Restart Faye                           
  # ./rb_deploy -rp            --> Restart Pushr                          
  # ./rb_deploy -rm            --> Restart MongoDB                        
  # ./rb_deploy -rq            --> Restart Sidekiq                        
  # ./rb_deploy -rs            --> Restart Redis                          
--------------------------------------------------------------------------
  ### STOP SERVICES ###                                                   
--------------------------------------------------------------------------
  # ./rb_deploy -du            --> Stop Unicorn                              
  # ./rb_deploy -df            --> Stop Faye                              
  # ./rb_deploy -dp            --> Stop Pushr                             
  # ./rb_deploy -dq            --> Stop Sidekiq                           
  # ./rb_deploy -ds            --> Stop Redis                             
--------------------------------------------------------------------------
  ### VIEW LOGS ###                                                       
--------------------------------------------------------------------------
  # ./rb_deploy -le-nginx      --> View NGINX Error Log                   
  # ./rb_deploy -la-nginx      --> View NGINX Access Log                  
  # ./rb_deploy -l-mongodb     --> View MongoDB Log                       
  # ./rb_deploy -l-memcached   --> View Memcached Log                     
  # ./rb_deploy -l-redis       --> View Redis Log                         
  # ./rb_deploy -l-pushr       --> View Pushr Log                         
  # ./rb_deploy -l-sidekiq     --> View Sidekiq Log                       
  # ./rb_deploy -l-unicorn     --> View Unicorn Log                       
--------------------------------------------------------------------------
  ### SERVER ###                                                          
--------------------------------------------------------------------------
  # ./rb_deploy -key           --> Generate Secret Token                              
  # ./rb_deploy -up            --> Server Up                              
  # ./rb_deploy -down          --> Server Down                            
  # ./rb_deploy -deploy / -dep --> Running Deploy                         
==========================================================================
```

## Folder Structure
After successfully deploy, your application would be look's like:
```
.
├── current -> release/201710250642_1508888522
├── make-rb_deploy
├── preinstall.sh
├── rb_deploy
├── rb_deploy.c
├── release
│   ├── 201710241803_1508842994
│   ├── 201710241906_1508846808
│   └── 201710250642_1508888522
└── shared
```

## Deploy Process
* Clone Repository to Unix DateTime Release Folder
* Checkout Branch
* Remove Shared Folders
* Remove Shared Files
* Create Symlink Shared Folders
* Create Symlink Shared Files
* Install Bundle  (gem install bundle)
* Install Package (bundle install)
* Compile Assets 
* Install NPM [optional]
* Migrate Database
* Seed Database
* Create Symlink Release -> Current
* Service Unicorn Stop
* Service Unicorn Start
* **FINISH**
    
## Deploy Rollback 
On Failed Deploy:
* Remove Unix DateTime Release Folder
* Migration Rollback
* Do not run service "server_up"

## Configuration
* Clone this repo
* Add `rb_deploy.c`, `make-rb_deploy` and `preinstall.sh` to your root path of Ruby or Rails project
* Setup Number of Release 
  ```
  int NUM_RELEASE  = 10; // Maximum Number of Release Folder 
  ```
* Setup Number of Line Log
  ```
  int NUM_LOG_VIEW = 50; // Maximum Line Number Viewing Log 
  ```
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
  Hint: `which [binary]` 
  (eg: `which gem`, `which bundle`, `which unicorn`, `which rake`, `which rakeup`)
  ```
  // Log
  char SYS_LOG_PUSHR[512];                                             // Path Log Pushr
  char SYS_LOG_SIDEKIQ[512];                                           // Path Log Sidekiq
  char SYS_LOG_UNICORN[512];                                           // Path Log Unicorn
  char SYS_LOG_NGINX_ERROR[512]  = "/var/log/nginx/error.log";         // Path Log NGINX Error
  char SYS_LOG_NGINX_ACCESS[512] = "/var/log/nginx/access.log";        // Path Log NGINX Access
  char SYS_LOG_MONGODB[512]      = "/var/log/mongodb.log";             // Path Log MongoDB
  char SYS_LOG_MEMCACHED[521]    = "/var/log/memcached.log";           // Path Log Memcached
  char SYS_LOG_REDIS[521]        = "/var/log/redis/redis-server.log";  // Path Log Redis

  // DEVELOPMENT CONFIGURATION //
  // Development Environment
  char DEV_APP_ROOT[512]        = "/home/zeroc0d3/zeroc0d3-deploy";                                // Development Root Path
  char DEV_CONFIG_UNICORN[512]  = "/home/zeroc0d3/zeroc0d3-deploy/config/unicorn/staging.rb";      // Development Unicorn Config
  char DEV_CONFIG_FAYE[512]     = "/home/zeroc0d3/zeroc0d3-deploy/faye.ru";                        // Development Faye Config
  char DEV_CONFIG_PUSHR[512]    = "/home/zeroc0d3/zeroc0d3-deploy/config/pushr-development.yaml";  // Development Pushr Config
  char DEV_CONFIG_SIDEKIQ[512]  = "/home/zeroc0d3/zeroc0d3-deploy/config/sidekiq.yml";             // Development Sidekiq Config

  char DEV_PID_UNICORN[512]     = "/home/zeroc0d3/zeroc0d3-deploy/tmp/pids/unicorn.pid";           // Development Path PID Unicorn
  char DEV_PID_FAYE[512]        = "/home/zeroc0d3/zeroc0d3-deploy/tmp/pids/faye.pid";              // Development Path PID Faye
  char DEV_PID_PUSHR[512]       = "/home/zeroc0d3/zeroc0d3-deploy/tmp/pids/pushr.pid";             // Development Path PID Pushr
  char DEV_PID_SIDEKIQ[512]     = "/home/zeroc0d3/zeroc0d3-deploy/tmp/pids/sidekiq.pid";           // Development Path PID Sidekiq

  char DEV_LOG_PUSHR[512]       = "/home/zeroc0d3/zeroc0d3-deploy/log/pushr.log";                  // Development Path Log Pushr
  char DEV_LOG_SIDEKIQ[512]     = "/home/zeroc0d3/zeroc0d3-deploy/log/sidekiq.log";                // Development Path Log Sidekiq
  char DEV_LOG_UNICORN[512]     = "/home/zeroc0d3/zeroc0d3-deploy/log/unicorn.log";                // Development Path Log Unicorn

  char DEV_PATH_UNICORN[512]    = "/home/zeroc0d3/.rbenv/shims/unicorn";                 // Development Path of Unicorn Binary
  // < Rails v5.0
  char DEV_PATH_RAKE[512]       = "/home/zeroc0d3/.rbenv/shims/rake";                    // Development Path of Rake Binary
  // >= Rails v5.0
  char DEV_PATH_RAILS[512]      = "/home/zeroc0d3/.rbenv/shims/rails";                   // Development Path of Rails Binary
  char DEV_PATH_RACKUP[512]     = "/home/zeroc0d3/.rbenv/shims/rackup";                  // Development Path of Rackup Binary
  char DEV_PATH_GEM[512]        = "/home/zeroc0d3/.rbenv/shims/gem";                     // Development Path of Gem Binary
  char DEV_PATH_BUNDLE[512]     = "/home/zeroc0d3/.rbenv/shims/bundle";                  // Development Path of Bundle Binary

  // PRODUCTION CONFIGURATION //
  // Production Environment
  char PROD_APP_ROOT[512]       = "/home/deploy/rb_deploy";                               // Production Root Path
  char PROD_CONFIG_UNICORN[512] = "/home/deploy/rb_deploy/config/unicorn/production.rb";  // Production Unicorn Config
  char PROD_CONFIG_FAYE[512]    = "/home/deploy/rb_deploy/faye.ru";                       // Production Faye Config
  char PROD_CONFIG_PUSHR[512]   = "/home/deploy/rb_deploy/config/pushr-production.yaml";  // Production Pushr Config
  char PROD_CONFIG_SIDEKIQ[512] = "/home/deploy/rb_deploy/config/sidekiq.yml";            // Production Sidekiq Config

  char PROD_PID_UNICORN[512]    = "/home/deploy/rb_deploy/tmp/pids/unicorn.pid";          // Production Path PID Unicorn
  char PROD_PID_FAYE[512]       = "/home/deploy/rb_deploy/tmp/pids/faye.pid";             // Production Path PID Faye
  char PROD_PID_PUSHR[512]      = "/home/deploy/rb_deploy/tmp/pids/pushr.pid";            // Production Path PID Pushr
  char PROD_PID_SIDEKIQ[512]    = "/home/deploy/rb_deploy/tmp/pids/sidekiq.pid";          // Production Path PID Sidekiq

  char PROD_LOG_PUSHR[512]      = "/home/deploy/rb_deploy/log/pushr.log";                 // Production Path Log Pushr
  char PROD_LOG_SIDEKIQ[512]    = "/home/deploy/rb_deploy/log/sidekiq.log";               // Production Path Log Sidekiq
  char PROD_LOG_UNICORN[512]    = "/home/deploy/rb_deploy/log/unicorn.log";               // Production Path Log Unicorn

  char PROD_PATH_UNICORN[512]   = "/home/deploy/.rbenv/shims/unicorn";          // Production Path of Unicorn Binary
  // < Rails v5.0
  char PROD_PATH_RAKE[512]      = "/home/deploy/.rbenv/shims/rake";             // Production Path of Rake Binary
  // >= Rails v5.0
  char PROD_PATH_RAILS[512]     = "/home/deploy/.rbenv/shims/rails";            // Production Path of Rails Binary
  char PROD_PATH_RACKUP[512]    = "/home/deploy/.rbenv/shims/rackup";           // Production Path of Rackup Binary
  char PROD_PATH_GEM[512]       = "/home/deploy/.rbenv/shims/gem";              // Production Path of Gem Binary
  char PROD_PATH_BUNDLE[512]    = "/home/deploy/.rbenv/shims/bundle";           // Production Path of Bundle Binary
  ```   
* Setup Shared Folders & Files
  ```
  char *SHARED_FOLDERS[] = {
    "log",
    "tmp/pids",
    "tmp/cache",
    "tmp/sockets",
    "vendor/bundle",
    "public/uploads",
    "public/system",
    "public/assets"};

  char *SHARED_FILES[] = {
    "config/application.yml",
    "config/database.yml",
    "config/mongoid.yml",
    "config/secrets.yml",
    "config/sidekiq.yml"};
  ```

## Make Binary
* Build your binary deploy
  `./make-rb_deploy`

## Run
* Running 
  `./rb_deploy [options]`

## Setup For All Users
* Uncomment line in file `make-rb_deploy`
  ```
  # sudo gcc -o /usr/local/bin/rb_deploy rb_deploy.c 
  # sudo ln -s /usr/local/bin/rb_deploy /bin/rb_deploy
  ```
* Running 
  ```
  rb_deploy [options]
  ```

## Road Map
- [X] Nginx `[restart|reload]`
- [X] Assets Precompile
- [X] Assets Clobber (Cleanup Compiled)
- [X] MongoDB `[restart|stop]`
- [X] Unicorn `[restart|stop]`
- [X] Faye `[restart|stop]`
- [X] Pushr `[restart|stop]`
- [X] Sidekiq `[restart|stop]`
- [X] Redis `[restart|stop]`
- [ ] Deploy

## License
[**MIT License**](https://github.com/zeroc0d3/ruby-deploy-gcc/blob/master/LICENSE)
