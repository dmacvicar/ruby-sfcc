require File.expand_path(File.join(File.dirname(__FILE__), 'helper'))
#require 'pp'

class SfccCimcClass < SfccTestCase

  context "a CIM environment and client" do
    setup do
      setup_cim_client
      op = Sfcc::Cim::ObjectPath.new("root/cimv2", "CIM_ComputerSystem")
      @cimclass = @client.get_class(op, Sfcc::Cim::Flags::IncludeQualifiers|Sfcc::Cim::Flags::IncludeClassOrigin)
    end
    
    should "be running" do
      assert cimom_running?
    end
    
    should "get a class" do
      assert @cimclass
    end
    
    context "with a cimclass" do
      should "have a string name" do
        assert @cimclass.class_name
        assert_kind_of String, @cimclass.class_name
      end

      should "have properties" do
        assert @cimclass.property_count > 0
      end
      
      should "be able to enumerate properties" do
        @cimclass.each_property do |k, d|
          assert_not_nil(k)
          assert_kind_of(Sfcc::Cim::Data, d)
        end
      
        properties = @cimclass.properties
        assert !properties.empty?
        assert_equal properties.size, @cimclass.property_count
      end
      
      should "get a property by name" do
        assert @cimclass.property "Name"
      end
      
      should "have qualifiers" do
        assert @cimclass.qualifier_count > 0
      end

      should "be able to enumerate qualifiers" do
        @cimclass.each_qualifier do |k, d|
          assert_not_nil(k)
          assert_kind_of(Sfcc::Cim::Data, d)
        end
      
        qualifiers = @cimclass.qualifiers
        assert_equal qualifiers.size, @cimclass.qualifier_count
      end

      should "get a qualifier by name" do
        assert @cimclass.qualifier "Description"
      end
      
      should "have property qualifiers" do
        assert @cimclass.property_qualifier_count("Status") > 0
      end
      
      should "get a property qualifier" do
        assert @cimclass.property_qualifier("Status","Description")
      end
      
      should "be able to enumerate qualifiers for a property" do
        @cimclass.each_property_qualifier("Status") do |k, d|
          assert_not_nil(k)
          assert_kind_of(Sfcc::Cim::Data, d)
        end
      end
    end # context
    
    context "and have extension callbacks" do
      should "have a string superclass name" do
        assert @cimclass.superclass_name
        assert_kind_of String, @cimclass.superclass_name
      end
      
      should "have key properties" do
        assert @cimclass.keys
        assert_kind_of Array, @cimclass.keys
      end

      should "answer if its an association" do
        assert !@cimclass.association?
      end
      should "answer if its abstract" do
        assert !@cimclass.abstract?
      end
      should "answer if its an indication" do
        assert !@cimclass.indication?
      end

    end # context

  end
  
end

