FAYE = {
  host:   'dev.zeroc0d3lab.com',
  port:   9292,
  mount:  'pub-sub',
  token:  ''
}

home_path = '/home/deploy/zeroc0d3lab/current'

if defined? Rails
  if Rails.env == 'development'
    FAYE[:url] = "http://#{ FAYE[:host] }:#{ FAYE[:port] }/#{ FAYE[:mount] }"
  elsif Rails.env == 'production'
    FAYE[:url] = "http://zeroc0d3lab.com:9292/#{ FAYE[:mount] }"
  else
    FAYE[:url] = "https://#{ FAYE[:host] }/#{ FAYE[:mount] }"
  end

  plist = `ps -eo command  | grep "faye"`.split("\n")

  unless plist.find {|p| p.match('faye.ru') }
    Thread.new do
      command = "RAILS_ENV=tvmu bundle exec rackup #{home_path}/faye.ru -s thin -E #{Rails.env} -o 0.0.0.0 -D -P #{home_path}/tmp/pids/faye.pid"

      sleep(5)
      puts "Rackup Faye with #{ command }"
      puts "Faye uri is #{ FAYE[:url] }"
      system(command)
    end
  end
end
