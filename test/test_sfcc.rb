require File.join(File.dirname(__FILE__), 'helper')
require 'pp'

class BasicTest < SfccTestCase

  context "a running CIMOM with no auth" do
    setup do
      setup_cim_client
    end

    should "be running" do
      assert cimom_running?
    end

    should "be of class Client" do
      assert_kind_of(Sfcc::Cim::Client, @client)
    end
        
    context "a new object path for root/cimv2" do
      setup do
        @op = Sfcc::Cim::ObjectPath.new("root/cimv2")
      end
      
      should "be of class ObjectPath" do
        assert_kind_of(Sfcc::Cim::ObjectPath, @op)
      end
      
      should "allow for query" do
        result = @client.query(@op, "select * from CIM_ComputerSystem", "wql")
        result.each do |instance|
          puts instance
        end
          end
            
      context "class names" do
        setup do
          @class_names = @client.class_names(@op, Sfcc::Flags::DeepInheritance)
        end
        
        should "be a Cimc::Enumeration" do
          assert_kind_of(Sfcc::Cim::Enumeration, @class_names)
        end
        
        should "include CIM_ManagedElement" do
          assert !@class_names.select { |x| x.to_s == "CIM_ManagedElement" }.empty?
        end
        
        should "have every element of type Sfcc::Cim::ObjectPath" do
          @class_names.each { |n| assert_kind_of(Sfcc::Cim::ObjectPath, n) }
        end            
      end
      
      context "classes" do
        setup do
          @classes = @client.classes(@op, Sfcc::Flags::DeepInheritance)
        end
        
        should "be a Cimc::Enumeration" do
          assert_kind_of(Sfcc::Cim::Enumeration, @classes)
        end
        
        should "have every alement of type Cimc::Class" do
          @classes.each { |c| assert_kind_of(Sfcc::Cim::Class, c) }
        end            
      end
      
    end
  end  
end  
