#
# sfcc/object_path.rb
#
module Sfcc
  module Cim
    class ObjectPath
      def [] name
	self.key(name)
      end

      def invoke name, *args
        raise "Cannot invoke, ObjectPath has no client associated" unless self.client
	classname = self.classname
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
        res = self.client.invoke_method(self, name, argsin, argsout)
        return res
      end

      def method_missing name, *args
        if args.empty?
          begin
            self.key name
          rescue Sfcc::Cim::ErrorNoSuchProperty
            self.invoke name
          end
        else
          self.invoke name, *args
        end
      end
    end
  end
end
