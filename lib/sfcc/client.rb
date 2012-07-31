# sfcc/client.rb

module Sfcc

  module Cim
    
    class Client
    private
      def self.uri_to_params(uri, params)
        params[:host] ||= uri.host
        params[:scheme] ||= uri.scheme
        params[:port] ||= uri.port
        params[:user] ||= uri.user
        params[:password] ||= uri.password
      end
    public
      # Client.connect(:host => "localhost",
      #                :scheme => "http",
      #                :port => 5988,
      #                :user => "root",
      #                :password => "foo"
      #                :verify => true,
      #                :trust_store => "/path/to/trust/store",
      #                :cert_file => "/path/to/cert_file",
      #                :key_file => "/path/to/key_file") do |client|
      #   # do something with client
      # end
      #
      # client is both returned and passed to a given block.
      #
      # :verify, :trust_store, :cert_file, and :key_file are only evaluated
      # if :scheme is 'https' and verify != false
      #
      # If :verify=>false, :trust_store, :cert_file and :key_file are unused
      # If :verify is ommitted, it defaults to true
      #
      # Alternatively you can pass a uri:
      #
      #   client = Client.connect(uri)
      #
      # or a mix like
      #
      #   client = Client.connect(:uri => uri, :verify => false)
      # Explicit parameters (like :user) always override :uri data
      #
      def self.connect(args={})
        params = {}
        uri = nil
        case args
        when Hash
          params.merge!(args)
          uri = params[:uri]
          case uri
          when URI
            self.uri_to_params(uri, params)
          else
            self.uri_to_params(URI.parse(uri.to_s), params)
          end
        when ::String
          uri = URI.parse(args)
	  return Client.connect uri
        when URI
          self.uri_to_params(args, params)
	else
	  raise "Bad argument #{args.inspect}:#{args.class}"
        end

        verify = params[:verify]
        trust_store = params[:trust_store]
        cert_file = params[:cert_file]
        key_file = params[:key_file]

        case params[:scheme]
        when "http"
        when "https"
          unless verify == false
            raise "Must either specify :verify=>false or :cert_file for https URLs."
          end
        else
          raise "Unsupported connection scheme #{scheme.inspect}"
        end

        native_connect(params[:host],
                       params[:scheme],
                       params[:port].to_s,
                       params[:user],
                       params[:password],
                       verify,
                       trust_store,
                       cert_file,
                       key_file)
      end
    end

  end # module Cim
  
end # module Sfcc
