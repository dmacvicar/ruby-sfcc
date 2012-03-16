%w(../ext/sfcc ../lib).each do |path|
  $LOAD_PATH.unshift(File.expand_path(File.join(File.dirname(__FILE__), path)))
end
 
require 'rubygems'
require 'sfcc'

#
# enumerate CIMOM namespaces
#
# modeled along 'EnumNamespaces' from YaWN (http://pywbem.wiki.sourceforge.net/YAWN)

client = Sfcc::Cim::Client.connect('http://wsman:secret@localhost:5988')

op = Sfcc::Cim::ObjectPath.new("root/interop", "CIM_Namespace")
client.instance_names(op).each do |path|
  n = path.Name
  puts "Namespace: #{path.Name}"
end

client = nil

sleep 1

GC.start

sleep 1
