#
# sfcc/object_path.rb
#
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
  end
end