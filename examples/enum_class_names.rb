#!/usr/bin/env ruby

%w(../ext/sfcc ../lib).each do |path|
  $LOAD_PATH.unshift(File.expand_path(File.join(File.dirname(__FILE__), path)))
end

require 'sfcc'

namespace = ARGV[0] || "root/cimv2"

client = Sfcc::Cim::Client.connect('http://root@localhost:5988')

op = Sfcc::Cim::ObjectPath.new(namespace, nil)

client.class_names(op).each do |name|
  puts "#{namespace}:#{name}"
end
