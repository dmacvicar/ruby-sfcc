# sfcc/data.rb

module Sfcc

  module Cim

    class Data
      def to_s
        state = if good? then "good"
      elsif bad? then "bad"
      elsif null? then "null"
      elsif not_found? then "not found"
      elsif key? then "key"
      else "???" end
        "<#{state}>[#{type}]: #{value}"
      end
      def good?
        state_is Good
      end
      def bad?
        state_is Bad
      end
      def null?
        state_is Null
      end
      def not_found?
        state_is NotFound
      end
      def key?
        state_is Key
      end
    end
    
  end # module Cim
  
end # module Sfcc
