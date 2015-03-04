# sfcc/instance.rb

module Sfcc

  module Cim
    
    class Instance
      def classname
        self.object_path.classname
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
            begin
              self.object_path.invoke name
            rescue Sfcc::Cim::ErrorMethodNotFound
              nil
            end
          end
        else
          self.object_path.invoke name, *args
        end
      end
    end

  end # module Cim
  
end # module Sfcc
