require 'sfcc/version'
require "sfcc.so" # native
require 'enumerator'
require 'uri'

require 'sfcc/cim'
require 'sfcc/client'
require 'sfcc/class'
require 'sfcc/instance'
require 'sfcc/data'

module Sfcc

  module Cim

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
    
    class Type
      def array?
        (self.to_i & Type::ARRAY) != 0
      end
      def integer?
        (self.to_i & Type::INTEGER) != 0
      end
      def string?
        self.to_i == Type::String
      end
    end
  end # module Cim
  
end # module Sfcc
