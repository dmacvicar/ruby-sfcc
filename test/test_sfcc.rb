require_relative 'helper'

describe "a running CIMOM" do
  include SfccTestUtils
  
  before do
    setup_cim_client
  end
  
  it "should be running" do
    assert cimom_running?
  end
  
  it "should be of class Client" do
    assert_kind_of(Sfcc::Cim::Client, @client)
  end
  
  describe "a new object path for root/cimv2" do
    before do
      @op = Sfcc::Cim::ObjectPath.new("root/cimv2")
    end
    
    it "should be of class ObjectPath" do
      assert_kind_of(Sfcc::Cim::ObjectPath, @op)
    end
    
    it "should allow for query" do
      result = @client.query(@op, "select * from CIM_ComputerSystem", "wql")
      count = 0
      result.each do |instance|
        assert instance
        count += 1
      end
      assert count > 0
    end
    
    describe "class names" do
      before do
        @class_names = @client.class_names(@op, Sfcc::Flags::DeepInheritance)
      end
      
      it "should be a Cimc::Enumeration" do
        assert_kind_of(Sfcc::Cim::Enumeration, @class_names)
      end
      
      it "should include CIM_ManagedElement" do
        assert !@class_names.select { |x| x.to_s == "CIM_ManagedElement" }.empty?
      end
      
      it "should have every element of type Sfcc::Cim::ObjectPath" do
        @class_names.each { |n| assert_kind_of(Sfcc::Cim::ObjectPath, n) }
      end            
    end
    
    describe "classes" do
      before do
        @classes = @client.classes(@op, Sfcc::Flags::DeepInheritance)
      end
      
      it "should be a Cimc::Enumeration" do
        assert_kind_of(Sfcc::Cim::Enumeration, @classes)
      end
      
      it "should have every alement of type Cimc::Class" do
        @classes.each { |c| assert_kind_of(Sfcc::Cim::Class, c) }
      end            
    end
    
  end
end  

