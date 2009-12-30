
require "sfcc.so" # native
require 'enumerator'
require 'uri'

module Sfcc

  VERSION = "0.1.0"

  module Flags
    LocalOnly          = 1
    DeepInheritance    = 2
    IncludeQualifiers  = 4
    IncludeClassOrigin = 8
  end
    
  module Cim

    class ErrorFailed < RuntimeError; end
    class ErrorAccessDenied < RuntimeError; end
    class ErrorInvalidNamespace < RuntimeError; end
    class ErrorInvalidParameter < RuntimeError; end
    class ErrorInvalidClass < RuntimeError; end
    class ErrorNotFound < RuntimeError; end
    class ErrorNotSupported < RuntimeError; end
    class ErrorClassHasChildren < RuntimeError; end
    class ErrorClassHasInstances < RuntimeError; end
    class ErrorInvalidSuperClass < RuntimeError; end
    class ErrorAlreadyExists < RuntimeError; end
    
    # The property you are trying to retrieve does not exist
    class ErrorNoSuchProperty < RuntimeError; end
    class ErrorTypeMisMatch < RuntimeError; end
    class ErrorQueryLanguageNotSupported < RuntimeError; end
    class ErrorMethodNotAvailable < RuntimeError; end
    class ErrorMethodNotFound < RuntimeError; end
    class ErrorDoNotUnload < RuntimeError; end
    class ErrorErrorSystem < RuntimeError; end
    class ErrorRcError < RuntimeError; end
    
    class Args
      include Enumerable
      # args.each do |name, value|
      #   ...
      # end
      #
      # enumerate all arguments
      def each
        Enumerable::Enumerator.new(to_hash)
      end

      # args[key] => value
      # retrieves a named argument
      def [](key)
        to_hash[key.to_sym]
      end

      # args[key] = value
      # sets a named argument
      def []=(key, value)
        add_arg(key.to_sym, value)
      end
      
      # alias for arg_count
      def size
        arg_count
      end
    end

    class Client
      # Client.connect(:host => "localhost",
      #                :scheme => "http",
      #                :port => 5988,
      #                :user => "root",
      #                :password => "foo") do |client|
      #   # do something with client
      # end
      #
      # client is both returned and passed to the given block.
      #
      # Alternatively you can pass a uri:
      #
      # client = Client.connect(uri)
      #
      def self.connect(args={})
        params = {}
        uri = nil
        case args
        when Hash
          params.merge!(args)
        when String
          uri = URI.parse(arg)
        when URI
          params[:host] = uri.host
          params[:scheme] = uri.scheme
          params[:port] = uri.port
          params[:user] = uri.user
          params[:password] = uri.password          
        end

        native_connect(params[:host],
                       params[:scheme],
                       params[:port],
                       params[:user],
                       params[:password]);
      end
    end
    
    class Instance
      # returns the string representation of
      # the object path for this instance
      def to_s
        object_path.to_s
      end
    end

    class ObjectPath
    end

    # foo bar ne ne
    class Enumeration
    end
    
  end
  
end

