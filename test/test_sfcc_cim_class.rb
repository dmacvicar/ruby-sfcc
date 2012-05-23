require File.expand_path(File.join(File.dirname(__FILE__), 'helper'))
#require 'pp'

class SfccCimcClass < SfccTestCase

  context "a CIM environment and client" do
    setup do
      setup_cim_client
      op = Sfcc::Cim::ObjectPath.new("root/cimv2", "CIM_ComputerSystem")
      @cimclass = @client.get_class(op)
    end
    
    should "be running" do
      assert cimom_running?
    end

    should "be able to enumerate properties" do
      @cimclass.each_property do |k, d|
        assert_not_nil(k)
        assert_kind_of(Sfcc::Cim::Data, d)
      end
      
      properties = @cimclass.properties
      assert !properties.empty?
      assert_equal properties.size, @cimclass.property_count
#      pp properties
    end

    should "be able to enumerate qualifiers" do
      @cimclass.each_qualifier do |k, d|
        assert_not_nil(k)
        assert_kind_of(Sfcc::Cim::Data, d)
      end
      
      qualifiers = @cimclass.qualifiers
      assert qualifiers.empty?
      assert_equal qualifiers.size, @cimclass.qualifier_count
#      pp qualifiers
    end

    should "be able to enumerate qualifiers for a property" do
      @cimclass.each_property_qualifier("Status") do |k, d|
        assert_not_nil(k)
        assert_kind_of(Sfcc::Cim::Data, d)
      end
      
      qualifiers = @cimclass.property_qualifiers("Status")
      assert qualifiers.empty?
      assert_equal qualifiers.size, @cimclass.qualifier_count
#      pp qualifiers
    end
    
  end
  
end

