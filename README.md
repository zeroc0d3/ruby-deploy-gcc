# ruby-deploy-gcc
Alternative Ruby & Rails Deployment Tools Build With GNU C (GCC)

![Deploy Process](https://github.com/zeroc0d3/ruby-deploy-gcc/blob/master/snapshot/deploy_process1.png)

## Dashboard
* $VERSION = The Latest Version Stable
* $WEB_SERVER = PUMA (default)

```
==========================================================================
  ZeroC0D3 Ruby Deploy :: ver-$VERSION                                          
  (c) 2017 ZeroC0D3 Team                                                  
==========================================================================
  ### ENVIRONMENT DEPLOY ###                                              
--------------------------------------------------------------------------
  # ENVIRONMENT   : staging                                         
  # PATH ROOT     : /home/zeroc0d3/zeroc0d3-deploy                                         
  # RAILS VERSION : 5  
  # WEB SERVER    : $WEB_SERVER                                       
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
  # ./rb_deploy -ru            --> Restart $WEB_SERVER                        
--------------------------------------------------------------------------
  ### STOP SERVICES ###                                                   
--------------------------------------------------------------------------
  # ./rb_deploy -df            --> Stop Faye                              
  # ./rb_deploy -dm            --> Stop MongoDB                              
  # ./rb_deploy -dp            --> Stop Pushr                             
  # ./rb_deploy -dq            --> Stop Sidekiq                           
  # ./rb_deploy -ds            --> Stop Redis                             
  # ./rb_deploy -du            --> Stop $WEB_SERVER                           
--------------------------------------------------------------------------
  ### VIEW LOGS ###                                                       
--------------------------------------------------------------------------
  # ./rb_deploy -l-env         --> View Environment's Log                   
  # ./rb_deploy -l-memcached   --> View Memcached Log                     
  # ./rb_deploy -l-mongodb     --> View MongoDB Log                       
  # ./rb_deploy -l-pushr       --> View Pushr Log                         
  # ./rb_deploy -l-redis       --> View Redis Log                         
  # ./rb_deploy -l-sidekiq     --> View Sidekiq Log                       
  # ./rb_deploy -l-puma        --> View $WEB_SERVER Log                       
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

## Documentation
* Configuration & How-To, see
[**Wiki Documentation**](https://github.com/zeroc0d3/ruby-deploy-gcc/wiki)

## Roadmap
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
