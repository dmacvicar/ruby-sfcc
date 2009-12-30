require File.join(File.dirname(__FILE__), 'helper')
require 'pp'

class SfccCimcClient < SfccTestCase
    
  context "a running CIMOM with no auth" do

    setup do
      setup_cim_client
      @op = Sfcc::Cim::ObjectPath.new("root/cimv2", "")
    end
    
    should "be running" do
      assert cimom_running?
    end

    
    should "be of class Client" do
      assert_kind_of(Sfcc::Cim::Client, @client)
    end
    
    should "allow for query" do
      result = @client.query(@op, "select * from CIM_OperatingSystem", "wql")
      result.each do |instance|
        puts "query result: #{instance}"
      end
    end
    
    should "be able to get set properties using an object path" do
      @op = Sfcc::Cim::ObjectPath.new("root/cimv2", "Linux_OperatingSystem")
      @client.instance_names(@op).each do |path|
        assert_kind_of Sfcc::Cim::ObjectPath, path
        # FAILS
        # assert ! @client.property(path, "PrimaryOwnerContact").empty?        
      end

    end

    should "be able to invoke methods using an object path" do
      @op = Sfcc::Cim::ObjectPath.new("root/cimv2", "Linux_OperatingSystem")
      @client.query(@op, "select * from Linux_OperatingSystem", "wql").each do |instance|
      # @client.instances(@op, Sfcc::Flags::IncludeClassOrigin | Sfcc::Flags::IncludeQualifiers, nil).each do |instance|
        path = instance.object_path
        assert_kind_of Sfcc::Cim::ObjectPath, path
        out = {}
        ret = @client.invoke_method(path, :execCmd, {:cmd => "cat /etc/SuSE-release"}, out)
        assert out.has_key?(:out), "output parameter is present"
        assert out[:out].match(/VERSION/)
        assert_equal 0, ret, "execCmd returns 0"
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

