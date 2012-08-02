%w(../ext/sfcc ../lib).each do |path|
  $LOAD_PATH.unshift(File.expand_path(File.join(File.dirname(__FILE__), path)))
end
 
require 'rubygems'
require 'sfcc'

include Sfcc

#
# instantiate Cim::Data class
#

# creating data from rb value
data = Cim::Data.from_value("Data created from string:")
puts "#{data.value} #{data}"
data = Cim::Data.from_value(15)
puts "Data created from integer(#{data.value}): #{data}"
data = Cim::Data.from_value([1, 4, 9, 10])
puts "Data created from array(from #{data.value.inspect}): #{data}"
data = Cim::Data.from_value(Cim::ObjectPath.new("namespace", "class_name"))
puts "Data created from object path: #{data}"

# creating data from type and value
data = Cim::Data.new(Cim::Type::String, "Data(type=string):")
puts "#{data.value} #{data}"
data = Cim::Data.new(Cim::Type::UInt8, 10)
puts "Data(type=UInt8): #{data}"
data = Cim::Data.new(Cim::Type::UInt8, -40)
puts "Data(type=UInt8) from value -40 (overflow): #{data}"
data = Cim::Data.new(Cim::Type::SInt16, 2**15)
puts "Data(type=SInt16) from value (2**15) (overflow): #{data}"
# type can be passed also as string
data = Cim::Data.new("String", "created with type passed as string")
puts "Data(type=String): #{data}"
data = Cim::Data.new("Reference", Cim::ObjectPath.new("namespace", "ClassName"))
puts "Data(type=Reference): #{data}"

# changing type and value of created data
data.value = Cim::ObjectPath.new("root/cimv2", "Cls2")
begin # this will raise a TypeError
    data.value = ["abcd"]
rescue TypeError
    puts("you can not change the value of data with different type"
         " before you set the corresponding type")
end
data.type = "StringA" # this will destroy kept value
puts "Data(type=StringA): #{data}"
data.value = ["abcd", "efgh"] # this will pass

