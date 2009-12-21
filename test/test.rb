
require 'helper'
require 'pp'

env = Sfcc::Cimc::Environment.new("XML")
client = env.connect("localhost", "http", "5988", "root", "")
op = env.new_object_path("root/cimv2", "")
client.each_class_name(op, 0) do |name|
  puts name
end


