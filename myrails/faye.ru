require 'faye'
require File.expand_path('../../current/config/initializers/faye.rb', __FILE__)

class ServerAuth

  def incoming(message, callback)
    if message['channel'] !~ %r{^/meta/}
      if message['ext']['authToken'] != FAYE[:token]
        message['error'] = 'Invalid authentication token.'
      end
    end

    callback.call(message)
  end

end

Faye::WebSocket.load_adapter('thin')

server = Faye::RackAdapter.new(mount: "/#{ FAYE[:mount] }", timeout: 25)
server.add_extension(ServerAuth.new)
run server