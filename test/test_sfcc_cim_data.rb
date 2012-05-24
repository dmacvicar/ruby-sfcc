require File.expand_path(File.join(File.dirname(__FILE__), 'helper'))
#require 'pp'

def check_data d
  assert d.state
  assert d.type
#  case d.type
#  when 0 then assert d.value # goodValue
#  when (1<<8) then assert_nil d.value # nullValue
#  when (2<<8) then d.value # keyValue
#  else
#    # notFound, badValue
#  end
end

class SfccCimcData < SfccTestCase

  context "a Cim::Class representation" do
    setup do
      setup_cim_client
      op = Sfcc::Cim::ObjectPath.new("root/cimv2", "CIM_ComputerSystem")
      @cimclass = @client.get_class(op)
    end
    
    should "provide Cim::Data for all its properties" do
      @cimclass.each_property do |k, d|
        assert_kind_of(Sfcc::Cim::Data, d)
        check_data d
        puts "#{k}: #{d.type}"
      end
    end

    should "provide Cim::Data for all its qualifiers" do
      @cimclass.each_qualifier do |k, d|
        assert_kind_of(Sfcc::Cim::Data, d)
        check_data d
      end
    end

    should "provide Cim::Data for all qualifiers for a property" do
      @cimclass.each_property_qualifier("Status") do |k, d|
        assert_not_nil(k)
        assert_kind_of(Sfcc::Cim::Data, d)
        check_data d
      end      
    end
    
  end
  
end
