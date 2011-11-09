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

['CIM_Namespace', '__Namespace'].each do |classname|
  ['root/cimv2', 'Interop', 'interop', 'root', 'root/interop'].each do |namespace|
    op = Sfcc::Cim::ObjectPath.new(namespace, classname)
#    puts "Checking #{op}"
    begin
      client.instance_names(op).each do |path|
	n = path.Name
	puts "Namespace: #{path.Name}"
      end
    rescue Sfcc::Cim::ErrorInvalidClass, Sfcc::Cim::ErrorInvalidNamespace
    end
  end
end

client = nil

sleep 5

GC.start

sleep 5
