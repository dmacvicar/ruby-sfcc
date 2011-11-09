require 'sfcc/version'
require "sfcc.so" # native
require 'enumerator'
require 'uri'

module Sfcc

  module Flags
    LocalOnly          = 1
    DeepInheritance    = 2
    IncludeQualifiers  = 4
    IncludeClassOrigin = 8
  end
    
  module Cim

    # Base class for errors
    class Error < RuntimeError; end
  
    # A general error occurred that is not covered by a more specific error code
    class ErrorFailed < Sfcc::Cim::Error; end
    # Access to a CIM resource was not available to the client
    class ErrorAccessDenied < Sfcc::Cim::Error; end
    # The target namespace does not exist
    class ErrorInvalidNamespace < Sfcc::Cim::Error; end
    # One or more parameter values passed to the method were invalid
    class ErrorInvalidParameter < Sfcc::Cim::Error; end
    # The specified Class does not exist
    class ErrorInvalidClass < Sfcc::Cim::Error; end
    # The requested object could not be found
    class ErrorNotFound < Sfcc::Cim::Error; end
    # The requested operation is not supported
    class ErrorNotSupported < Sfcc::Cim::Error; end
    # Operation cannot be carried out on this class since it has subclasses
    class ErrorClassHasChildren < Sfcc::Cim::Error; end
    # Operation cannot be carried out on this class since it has instances
    class ErrorClassHasInstances < Sfcc::Cim::Error; end
    # Operation cannot be carried out since the specified superclass does not exist
    class ErrorInvalidSuperClass < Sfcc::Cim::Error; end
    # Operation cannot be carried out because an object already exists
    class ErrorAlreadyExists < Sfcc::Cim::Error; end
    # The specified Property does not exist
    class ErrorNoSuchProperty < Sfcc::Cim::Error; end
    # The value supplied is incompatible with the type
    class ErrorTypeMisMatch < Sfcc::Cim::Error; end
    # The query language is not recognized or supported
    class ErrorQueryLanguageNotSupported < Sfcc::Cim::Error; end
    # The query is not valid for the specified query language
    class ErrorInvalidQuery < Sfcc::Cim::Error; end
    # The extrinsic Method could not be executed
    class ErrorMethodNotAvailable < Sfcc::Cim::Error; end
    # The specified extrinsic Method does not exist
    class ErrorMethodNotFound < Sfcc::Cim::Error; end
    
    class ErrorDoNotUnload < RuntimeError; end
    class ErrorErrorSystem < RuntimeError; end
    class ErrorRcError < RuntimeError; end
    
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
        when ::String
          uri = URI.parse(args)
	  return Client.connect uri
        when URI
          params[:host] = args.host
          params[:scheme] = args.scheme
          params[:port] = args.port
          params[:user] = args.user
          params[:password] = args.password          
	else
	  raise "Bad argument #{args.inspect}:#{args.class}"
        end

        native_connect(params[:host],
                       params[:scheme],
                       params[:port].to_s,
                       params[:user],
                       params[:password])
      end
    end

    class Class
      # properties => Hash
      #
      # return a hash with all properties
      def properties
        ret = {}
        each_property { |key, value| ret[key] = value }
        ret
      end

      # qualifiers => Hash
      #
      # return a hash with all qualifiers
      def qualifiers
        ret = {}
        each_qualifier { |key, value| ret[key] = value }
        ret
      end

      # property_qualifiers(property_name) => Hash
      #
      # return a hash with all qualifiers for +property_name+
      def property_qualifiers(property_name)
        ret = {}
        each_property_qualifier(property_name) { |key, value| ret[key] = value }
        ret
      end

    end
    
    class Instance
      # properties => Hash
      #
      # return a hash with all properties
      def properties
        ret = {}
        each_property { |key, value| ret[key] = value }
        ret
      end

      # qualifiers => Hash
      #
      # return a hash with all qualifiers
      def qualifiers
        ret = {}
        each_qualifier { |key, value| ret[key] = value }
        ret
      end

      # property_qualifiers(property_name) => Hash
      #
      # return a hash with all qualifiers for +property_name+
      def property_qualifiers(property_name)
        ret = {}
        each_property_qualifier(property_name) { |key, value| ret[key] = value }
        ret
      end
      
      # returns the string representation of
      # the object path for this instance
      def to_s
        object_path.to_s
      end
    end

    class ObjectPath
      def [] name
	self.key(name)
      end
      def method_missing name, *args
	self[name]
      end
    end

    class Enumeration
    end
    
  end
  
end

