# config/puma/production.rb
require "active_record"
  
root = "/home/deploy/rb_deploy/current"

# Default to production
rails_env = ENV['RAILS_ENV'] || "production"
environment rails_env

pidfile           "#{ root }/tmp/pids/puma.pid"
# stdout_redirect "#{shared_dir}/log/puma.stdout.log", "#{shared_dir}/log/puma.stderr.log", true
stdout_redirect   "#{ root }/log/puma.log"
bind              "unix://#{ root }/tmp/sockets/puma-production.socket"
state_path        "#{ root }/tmp/pids/puma.state"
rackup            "#{ root }/config.ru"

daemonize         true
workers           2
threads           1, 4
worker_timeout    90

# bind 'tcp://0.0.0.0:9292'
# bind 'unix:///var/run/puma.sock'
# bind 'unix:///var/run/puma.sock?umask=0111'
# bind 'ssl://127.0.0.1:9292?key=path_to_key&cert=path_to_cert'

# ssl_bind '127.0.0.1', '9292', {
#   key: path_to_key,
#   cert: path_to_cert
# }

# before_fork do
#   Signal.trap 'TERM' do
#     puts 'Puma master intercepting TERM and sending myself QUIT instead'
#     Process.kill 'QUIT', Process.pidfile
#   end
# end

# if defined?(Resque)
#   Resque.redis.quit
#   Rails.logger.info('Disconnected from Redis')
# end

# defined?(ActiveRecord::Base) and
#   ActiveRecord::Base.connection.disconnect!
# end

# on_worker_fork do
# end

# after_worker_fork do
#   Signal.trap 'TERM' do
#   puts 'Puma worker intercepting TERM and doing nothing. Wait for master to sent QUIT'
#   end
# end

activate_control_app

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