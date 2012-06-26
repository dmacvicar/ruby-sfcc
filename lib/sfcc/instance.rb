# sfcc/instance.rb

require 'cim'

module Sfcc

  module Cim
    
    class Instance
      def classname
        self.object_path.classname
      end

      def invoke name, *args
        raise "Cannot invoke, Instance has no client associated" unless self.client
	classname = self.object_path.classname
        # get method input parameter information
	s = "mof/#{classname}"
	begin
	  require s
	rescue LoadError
	  STDERR.puts "Cannot load #{s} for type information"
	  return
	end
	methods = MOF.class_eval "#{classname}::METHODS"
	method = methods[name.to_s]
	raise "Unknown method #{name} for #{classname}" unless method
	type = method[:type]
	parameters = method[:parameters] || {}
	input = parameters[:in]
	output = parameters[:out]
	argsin = {}
	i = 0
        if input
          while i < input.size
            value = args.shift
            raise "Argument for #{input[i]} is nil, not allowed !" unless value
            argsin[input[i]] = value
            # FIXME more typecheck of args ?
            i += 2
          end
        end
        argsout = nil
	if output
	  if args.empty?
	    raise "Function #{name} has output arguments, please add an empty hash to the call"
	  end
	  argsout = args.shift
	  unless argsout.kind_of? Hash
	    raise "Function #{name} has output arguments, last argument must be a Hash"
	  end
	  unless args.empty?
	    raise "Function call to #{name} has excess arguments"
	  end
	end
        res = self.client.invoke_method(self.object_path, name, argsin, argsout)
        return res
      end
      
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

      def method_missing name, *args
        if args.empty?
          begin
            self.property name
          rescue Sfcc::Cim::ErrorNoSuchProperty
            self.invoke name
          end
        else
          self.invoke name, *args
        end
      end
    end

  end # module Cim
  
end # module Sfcc