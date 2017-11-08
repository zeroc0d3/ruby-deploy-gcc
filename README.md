# ruby-deploy-gcc
Ruby Deploy build with GCC

![alt text](https://github.com/zeroc0d3/ruby-deploy-gcc/blob/master/snapshot/deploy_process1.png)

```
==========================================================================
  ZeroC0D3 Ruby Deploy :: ver-1.2.15                                          
  (c) 2017 ZeroC0D3 Team                                                  
==========================================================================
  ### ENVIRONMENT DEPLOY ###                                              
--------------------------------------------------------------------------
  # ENVIRONMENT   : staging                                         
  # PATH ROOT     : /home/zeroc0d3/zeroc0d3-deploy                                         
  # RAILS VERSION : 5                                         
--------------------------------------------------------------------------
  ### NGINX SERVICES ###                                                  
--------------------------------------------------------------------------
  # ./rb_deploy -no            --> Reload NGINX                           
  # ./rb_deploy -nr            --> Restart NGINX                          
--------------------------------------------------------------------------
  ### ASSETS SERVICES ###                                                 
--------------------------------------------------------------------------
  # ./rb_deploy -ac            --> Assets Clobber (Rollback)              
  # ./rb_deploy -ap            --> Assets Precompile                      
--------------------------------------------------------------------------
  ### RESTART SERVICES ###                                                
--------------------------------------------------------------------------
  # ./rb_deploy -rf            --> Restart Faye                           
  # ./rb_deploy -rm            --> Restart MongoDB                        
  # ./rb_deploy -rp            --> Restart Pushr                          
  # ./rb_deploy -rq            --> Restart Sidekiq                        
  # ./rb_deploy -rs            --> Restart Redis                          
  # ./rb_deploy -ru            --> Restart Unicorn                        
--------------------------------------------------------------------------
  ### STOP SERVICES ###                                                   
--------------------------------------------------------------------------
  # ./rb_deploy -df            --> Stop Faye                              
  # ./rb_deploy -dm            --> Stop MongoDB                              
  # ./rb_deploy -dp            --> Stop Pushr                             
  # ./rb_deploy -dq            --> Stop Sidekiq                           
  # ./rb_deploy -ds            --> Stop Redis                             
  # ./rb_deploy -du            --> Stop Unicorn                           
--------------------------------------------------------------------------
  ### VIEW LOGS ###                                                       
--------------------------------------------------------------------------
  # ./rb_deploy -l-env         --> View Environment's Log                   
  # ./rb_deploy -l-memcached   --> View Memcached Log                     
  # ./rb_deploy -l-mongodb     --> View MongoDB Log                       
  # ./rb_deploy -l-pushr       --> View Pushr Log                         
  # ./rb_deploy -l-redis       --> View Redis Log                         
  # ./rb_deploy -l-sidekiq     --> View Sidekiq Log                       
  # ./rb_deploy -l-unicorn     --> View Unicorn Log                       
  # ./rb_deploy -la-nginx      --> View NGINX Access Log                  
  # ./rb_deploy -le-nginx      --> View NGINX Error Log                   
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
├── current -> /home/zeroc0d3/rb_deploy/release/201711031754_1509706492
├── make-rb_deploy
├── preinstall.sh
├── rb_deploy
├── rb_deploy.c
├── release
│    ├── 201711031439_1509694746
│    ├── 201711031445_1509695138
│    └── 201711031754_1509706492
│       ├── config
│       │   ├── application.yml -> /home/zeroc0d3/rb_deploy/shared/config/database.yml
│       │   ├── database.yml -> /home/zeroc0d3/rb_deploy/shared/config/database.yml
│       │   ├── secrets.yml -> /home/zeroc0d3/rb_deploy/shared/config/secrets.yml
│       │   └── sidekiq.yml -> /home/zeroc0d3/rb_deploy/shared/config/sidekiq.yml
│       ├── log -> /home/zeroc0d3/rb_deploy/shared/log
│       ├── public
│       │   ├── assets -> /home/zeroc0d3/rb_deploy/shared/public/assets
│       │   ├── uploads -> /home/zeroc0d3/rb_deploy/shared/public/uploads
│       │   └── system -> /home/zeroc0d3/rb_deploy/shared/public/system
│       ├── tmp 
│       │   ├── pids -> /home/zeroc0d3/rb_deploy/shared/tmp/pids
│       │   ├── cache -> /home/zeroc0d3/rb_deploy/shared/tmp/cache
│       │   └── sockets -> /home/zeroc0d3/rb_deploy/shared/tmp/sockets
│       └── vendor
│           └── bundle -> /home/zeroc0d3/rb_deploy/shared/vendor/bundle
└── shared
    ├── config
    │   ├── application.yml
    │   ├── database.yml
    │   ├── mongoid.yml
    │   ├── secrets.yml
    │   └── sidekiq.yml
    ├── log
    ├── public
    │   ├── assets
    │   ├── uploads
    │   └── system
    ├── tmp
    │   ├── pids
    │   ├── cache
    │   └── sockets
    └── vendor
        └── bundle
```

## Deploy Process
* - [X] Clone Repository to Unix DateTime Release Folder
* - [X] Checkout Branch
* - [X] Install Bundle  (gem install bundle)
* - [X] Install Package (bundle install)
* - [X] Remove Shared Folders
* - [X] Remove Shared Files
* - [X] Create Symlink Shared Folders
* - [X] Create Symlink Shared Files
* - [X] Compile Assets 
* - [ ] Install NPM [**optional**]
* - [X] Migrate Database
* - [X] Seed Database
* - [X] Create Symlink Release -> Current
* - [X] Service Unicorn Stop
* - [X] Service Unicorn Start
* **FINISH**
    
## Deploy Rollback 
On Failed Deploy:
* Remove Unix DateTime Release Folder
* Migration Rollback
* Do not run service "server_up"

## Configuration
* Clone this repo
* Add `rb_deploy.c`, `make-rb_deploy` and `preinstall.sh` to your root path of Ruby or Rails project
* Setup **User** Configuration 
  ```
  int NUM_RELEASE   = 10;               // Maximum Number of Release Folder 
  char ENV[64]      = "staging";        // Selected Environment (staging / production)
  int NUM_LOG_VIEW  = 50;               // Maximum Line Number Viewing Log 
  int RAILS_VERSION = 5;                // Rails Version (default: 5)
  int ENABLE_MIGRATION = 0;             // Force Enable Migration (0 = disable/default, 1 = enable)
  int ENABLE_BUNDLE_INSTALL  = 1;       // Enable Running "bundle install" (0 = disable/default, 1 = enable)
  int ENABLE_CLOBBER_ASSETS  = 1;       // Enable Running Clobber/Cleanup Assets (0 = disable/default, 1 = enable)
  int ENABLE_COMPILE_ASSETS  = 1;       // Enable Running Assets Precompile (0 = disable/default, 1 = enable)
  int ENABLE_FAYE_SERVICE    = 1;       // Enable Running Faye Service (0 = disable/default, 1 = enable)
  int ENABLE_MONGODB_SERVICE = 1;       // Enable Running MongoDB Service (0 = disable/default, 1 = enable)
  int ENABLE_PUSHR_SERVICE   = 1;       // Enable Running Pushr Service (0 = disable/default, 1 = enable)
  int ENABLE_REDIS_SERVICE   = 1;       // Enable Running Redis Service (0 = disable/default, 1 = enable)
  int ENABLE_SIDEKIQ_SERVICE = 1;       // Enable Running Sidekiq Service (0 = disable/default, 1 = enable)

  // Repository
  char REPO_NAME[1024] = "git@github.com:zeroc0d3/ruby-installation.git";
  char REPO_BRANCH[64] = "master";

  // Shared Folders
  char *LIST_SHARED_FOLDERS[] = {
      "log",
      "tmp/pids",
      "tmp/cache",
      "tmp/sockets",
      "vendor/bundle",
      "public/uploads",
      "public/system",
      "public/assets",
      NULL
  };

  // Shared Files
  char *LIST_SHARED_FILES[] = {
      "config/application.yml",
      "config/database.yml",
      "config/mongoid.yml",
      "config/secrets.yml",
      "config/sidekiq.yml",
      NULL
  };
  ```
* Setup **Staging/Development** Environment
  ```
  // DEVELOPMENT CONFIGURATION //
  // Development Environment
  char DEV_APP_ROOT[512]        = "/home/zeroc0d3/zeroc0d3-deploy";                                // Development Root Path
  char DEV_CONFIG_UNICORN[512]  = "/home/zeroc0d3/zeroc0d3-deploy/config/unicorn/staging.rb";      // Development Unicorn Config
  char DEV_CONFIG_FAYE[512]     = "/home/zeroc0d3/zeroc0d3-deploy/faye.ru";                        // Development Faye Config
  char DEV_CONFIG_PUSHR[512]    = "/home/zeroc0d3/zeroc0d3-deploy/config/pushr-staging.yaml";      // Development Pushr Config
  char DEV_CONFIG_SIDEKIQ[512]  = "/home/zeroc0d3/zeroc0d3-deploy/config/sidekiq.yml";             // Development Sidekiq Config

  char DEV_PID_UNICORN[512]     = "/home/zeroc0d3/zeroc0d3-deploy/tmp/pids/unicorn.pid";           // Development Path PID Unicorn
  char DEV_PID_FAYE[512]        = "/home/zeroc0d3/zeroc0d3-deploy/tmp/pids/faye.pid";              // Development Path PID Faye
  char DEV_PID_PUSHR[512]       = "/home/zeroc0d3/zeroc0d3-deploy/tmp/pids/pushr.pid";             // Development Path PID Pushr
  char DEV_PID_SIDEKIQ[512]     = "/home/zeroc0d3/zeroc0d3-deploy/tmp/pids/sidekiq.pid";           // Development Path PID Sidekiq

  char DEV_LOG_ENV[512]         = "/home/zeroc0d3/zeroc0d3-deploy/log/staging.log";                // Development Path Log Environment
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
  ```
* Setup **Production** Environment
  ```  
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

  char PROD_LOG_ENV[512]        = "/home/deploy/rb_deploy/log/production.log";            // Production Path Log Environment
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

## Make Binary
* Build your binary deploy
  `./make-rb_deploy`

## Deploy Target VPS
* Create Project Folder
  ```
  /home/deploy/rb_deploy
  ```
* Upload Deployment Tools (after make binary)
  ```
  make-rb_deploy
  rb_deploy.c
  rb_deploy
  preinstall.sh
  ```
* Upload Shared Folders (example folders)
  ```
  "log",
  "tmp/pids",
  "tmp/cache",
  "tmp/sockets",
  "vendor/bundle",
  "public/uploads",
  "public/system",
  "public/assets",
  ```
* Upload Shared Files (example files)
  ```
  "config/application.yml",
  "config/database.yml",
  "config/mongoid.yml",
  "config/secrets.yml",
  "config/sidekiq.yml",
  ```

## Run
* Running 
  `./rb_deploy [options]`

## Setup For All Users
* Uncomment line in file `make-rb_deploy`
  ```
  # sudo gcc -o /usr/local/bin/rb_deploy rb_deploy.c     // Using Compile, or 
  # sudo ln -s rb_deploy /usr/local/bin/rb_deploy        // Using Symlink
  # sudo ln -s /usr/local/bin/rb_deploy /bin/rb_deploy
  ```
* Running 
  ```
  rb_deploy [options]
  ```

## Road Map
* - [X] Nginx `[restart│reload]`
* - [X] Assets Precompile
* - [X] Assets Clobber (Cleanup Compiled)
* - [X] MongoDB `[restart│stop]`
* - [X] Unicorn `[restart│stop]`
* - [X] Faye `[restart│stop]`
* - [X] Pushr `[restart│stop]`
* - [X] Sidekiq `[restart│stop]`
* - [X] Redis `[restart│stop]`
* - [X] Deploy

## License
[**MIT License**](https://github.com/zeroc0d3/ruby-deploy-gcc/blob/master/LICENSE)
