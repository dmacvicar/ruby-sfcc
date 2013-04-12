#!/usr/bin/env ruby

%w(../ext/sfcc ../lib).each do |path|
  $LOAD_PATH.unshift(File.expand_path(File.join(File.dirname(__FILE__), path)))
end

require 'sfcc'

namespace = ARGV[0] || "root/cimv2"
classname = ARGV[1] || nil

client = Sfcc::Cim::Client.connect(:uri => 'https://wsman:secret@localhost:5989', :verify=>false)

op = Sfcc::Cim::ObjectPath.new(namespace, classname)

#puts "Class names for #{op}"
client.class_names(op).each do |name|
  puts "  #{namespace}:#{name}"
end
