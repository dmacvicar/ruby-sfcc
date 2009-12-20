
begin
require 'helper'
rescue LoadError
  puts $:
end

client = Sfcc::Cmci::Client.connect("localhost", "http", "5988", "dd", "dd")

