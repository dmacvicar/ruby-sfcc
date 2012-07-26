require File.expand_path(File.join(File.dirname(__FILE__), 'helper'))
#require 'pp'

class SfccCimInstanceTest < SfccTestCase

  context "object path for CIM_ComputerSystem and running CIMOM" do
    setup do
      setup_cim_client
      @op_computer_system = Sfcc::Cim::ObjectPath.new("root/cimv2", "CIM_ComputerSystem")
    end
    
    should "be running" do
      assert cimom_running?
    end

    should "be able to create an instance" do
      instance = Sfcc::Cim::Instance.new(@op_computer_system)
    end
  end
    
  context "an instance of CIM_ComputerSystem" do
    setup do
      setup_cim_client
      op = Sfcc::Cim::ObjectPath.new("root/cimv2")
      @instance = @client.query(op, "select * from CIM_ComputerSystem", "wql").to_a.first
    end
    
    should "be running" do
      assert cimom_running?
    end

    should "respond to property Name" do
      assert_kind_of(String, @instance.property('Name'))
    end

    should "be able to iterate over properties" do
      count = 0
      @instance.each_property do |name, value|
        count += 1
        assert name
        # value can be nil
      end
      assert count > 0
    end

    should "be able to set and retrieve stringproperties" do
      assert_raises Sfcc::Cim::ErrorNoSuchProperty do
        @instance.property("foobar");
      end
      assert_nothing_raised do
        @instance.set_property("Name", "newname");
      end
      assert_equal "newname", @instance.property("Name")
    end

    should "be able to enumerate qualifiers" do
      @instance.each_qualifier do |k, v|
        assert_not_nil(k)
      end
      
      qualifiers = @instance.qualifiers
      assert qualifiers.empty?
      assert_equal qualifiers.size, @instance.qualifier_count
#      pp qualifiers
    end
    
    should "be able to enumerate qualifiers for a property" do
      @instance.each_property_qualifier("Status") do |k, v|
        assert_not_nil(k)
      end
      
      qualifiers = @instance.property_qualifiers("Status")
      assert qualifiers.empty?
      assert_equal qualifiers.size, @instance.qualifier_count
#      pp qualifiers
    end
    
    
  end   
end            

