# config/unicorn/production.rb

root = "/home/zeroc0d3/zeroc0d3-deploy/current"
working_directory root

Unicorn::HttpServer::START_CTX[0] = "#{ root }/bin/unicorn"

pid               "#{ root }/tmp/pids/unicorn.pid"
stderr_path       "#{ root }/log/unicorn.log"
stdout_path       "#{ root }/log/unicorn.log"
listen            "#{ root }/tmp/sockets/unicorn-staging.socket"

worker_processes  4
timeout           90

before_fork do |server, worker|
  Signal.trap 'TERM' do
    puts 'Unicorn master intercepting TERM and sending myself QUIT instead'
    Process.kill 'QUIT', Process.pid
end

# If you are using Redis but not Resque, change this
if defined?(Resque)
  Resque.redis.quit
  Rails.logger.info('Disconnected from Redis')
end

defined?(ActiveRecord::Base) and
  ActiveRecord::Base.connection.disconnect!
end

after_fork do |server, worker|
  Signal.trap 'TERM' do
  puts 'Unicorn worker intercepting TERM and doing nothing. Wait for master to sent QUIT'
end

defined?(ActiveRecord::Base) and
  ActiveRecord::Base.establish_connection
end