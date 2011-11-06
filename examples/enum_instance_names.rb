%w(../lib).each do |path|
  $LOAD_PATH.unshift(File.expand_path(File.join(File.dirname(__FILE__), path)))
end
 
require 'rubygems'
require 'sfcc'

client = Sfcc::Cim::Client.connect('http://root@localhost:5988')

op = Sfcc::Cim::ObjectPath.new("root/cimv2", "CIM_OperatingSystem")
client.instance_names(op).each do |instance|
  puts "CIM_OperatingSystem: #{instance}"
end
