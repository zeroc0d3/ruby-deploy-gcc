# config/puma/staging.rb
require "active_record"
  
root = "/home/zeroc0d3/zeroc0d3-deploy/current"

# Default to staging
rails_env     = ENV['RAILS_ENV'] || "staging"
threads_count = Integer(ENV['RAILS_MAX_THREADS'] || 4)
environment rails_env

pidfile           "#{ root }/tmp/pids/puma.pid"
stdout_redirect   "#{ root }/log/puma.log"
bind              "unix://#{ root }/tmp/sockets/puma-production.socket"
state_path        "#{ root }/tmp/pids/puma.state"
rackup            DefaultRackup
port              ENV['PORT'] || 3000

daemonize         true
workers           Integer(ENV['WEB_CONCURRENCY'] || 2)
threads           threads_count, threads_count
worker_timeout    90

preload_app!

on_worker_boot do
  ActiveRecord::Base.connection.disconnect! rescue ActiveRecord::ConnectionNotEstablished
  ActiveRecord::Base.establish_connection(YAML.load_file("#{ root }/config/database.yml")[rails_env])
end

#### NOTES:
# Get CPU Info
# *) Linux
#    grep -c processor /proc/cpuinfo 
# *) OS-X
#    sysctl -n machdep.cpu.brand_string   
#    sysctl -a | grep machdep.cpu | grep core_count  
#    sysctl -a | grep machdep.cpu | grep thread_count

# Troubleshoot
#   objc[1594]: +[__NSPlaceholderDictionary initialize] may have been in progress in another thread when fork() was called.
#   objc[1595]: +[__NSPlaceholderDictionary initialize] may have been in progress in another thread when fork() was called.
#   objc[1594]: +[__NSPlaceholderDictionary initialize] may have been in progress in another thread when fork() was called. # We cannot safely call it or ignore it in the fork() child process. Crashing instead. Set a breakpoint on objc_initializeAfterForkError to debug.
#
# Run: export OBJC_DISABLE_INITIALIZE_FORK_SAFETY=YES