
require "sfcc.so" # native
require 'enumerator'

module Sfcc
  CIMC_FLAG_LocalOnly          = 1
  CIMC_FLAG_DeepInheritance    = 2
  CIMC_FLAG_IncludeQualifiers  = 4
  CIMC_FLAG_IncludeClassOrigin = 8

  module Cimc
    
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
    class ErrorNoSuchProperty < RuntimeError; end
    class ErrorTypeMisMatch < RuntimeError; end
    class ErrorQueryLanguageNotSupported < RuntimeError; end
    class ErrorMethodNotAvailable < RuntimeError; end
    class ErrorMethodNotFound < RuntimeError; end
    class ErrorDoNotUnload < RuntimeError; end
    class ErrorErrorSystem < RuntimeError; end
    class ErrorRcError < RuntimeError; end

    class Instance
      # returns the string representation of
      # the object path for this instance
      def to_s
        object_path.to_s
      end
    end

    class Client
      def each_class_name
        
      end
      
    end
    
  end
  
end

