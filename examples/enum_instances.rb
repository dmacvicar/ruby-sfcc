#!/usr/bin/env ruby

%w(../ext/sfcc ../lib).each do |path|
  $LOAD_PATH.unshift(File.expand_path(File.join(File.dirname(__FILE__), path)))
end

require 'sfcc'

namespace = "root/cimv2"
classname = nil

until ARGV.empty?
  arg = ARGV.shift
  if arg == "-n"
    namespace = ARGV.shift
    next
  end
  classname ||= arg
end

raise "Usage: #{$0} [-n <namespace>] <classname>" if classname.nil?

client = Sfcc::Cim::Client.connect('http://root@localhost:5988')

op = Sfcc::Cim::ObjectPath.new(namespace, classname)
      
client.instances(op).each do |instance|
  puts "Instance #{instance}"
  instance.each_property do |name, value|              
    printf "%+32s : %s\n", name, value
  end
end
