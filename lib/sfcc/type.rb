#
# sfcc/type.rb
#
module Sfcc
  module Cim
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
  end
end
