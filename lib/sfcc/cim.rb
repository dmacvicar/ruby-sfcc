# sfcc/cim.rb

module Sfcc

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
    
  end # module Cim
  
end # module Sfcc
