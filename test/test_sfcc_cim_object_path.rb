
require File.join(File.dirname(__FILE__), 'helper')
require 'pp'

class SfccCimcObjectPathTest < SfccTestCase

  context "an object path for a namespace" do
    setup do
      setup_cim_client
      @op = Sfcc::Cim::ObjectPath.new("root/cimv2")
    end

    should "have a nil class name" do
      assert_nil @op.classname
    end
  end
  
  context "a new object path for an instance of root/cimv2:Linux_OperatingSystem" do
    setup do
      setup_cim_client
      @op = Sfcc::Cim::ObjectPath.new("root/cimv2")
      @op = @client.query(@op, "select * from Linux_OperatingSystem", "wql").first.object_path
    end
    
    should "be running" do
      assert cimom_running?
    end

    should "have root/cimv2 as namespace" do
      assert_equal "root/cimv2", @op.namespace
    end
    
    should "change its namespace after setting it" do
      @op.namespace = "root/cimv3"
      assert_equal "root/cimv3", @op.namespace
    end

    should "not have namespace at the beginning" do
      assert_raise Sfcc::Cim::ErrorNotSupported do
        @op.hostname
      end
    end
    
    should "change its hostname after setting it" do
      assert_raise Sfcc::Cim::ErrorNotSupported do
        @op.hostname = "foo.bar.com"
      end
      assert_raise Sfcc::Cim::ErrorNotSupported do
        assert_equal "foo.bar.com", @op.hostname
      end
    end
    
    should "respond to classname" do
      assert_equal "Linux_OperatingSystem", @op.classname
    end
    
    should "change its class name after setting it" do
      @op.classname = "BarClass"
      assert_equal "BarClass", @op.classname
    end

    should "respond to keys and set them" do
      assert_raise Sfcc::Cim::ErrorNoSuchProperty do
        @op.key("foo")
      end
      assert_equal 4, @op.key_count
            
      assert_nothing_raised do
        @op.add_key("prop0", "hello")
        @op.add_key("prop1", true)
        @op.add_key("prop2", false)
      end
      assert_equal 7, @op.key_count
      
      assert_equal "hello", @op.key("prop0")
      assert_equal true, @op.key("prop1")
      assert_equal false, @op.key("prop2")
    end

    should "be able to set namespace and classname from other object path" do
      op = Sfcc::Cim::ObjectPath.new("root/cimv2", "Linux_OperatingSystem")
      op2 = Sfcc::Cim::ObjectPath.new("root/cimv3", "FooBar")
      assert_equal "Linux_OperatingSystem", op.classname
      op.set_namespace_from(op2)
      assert_equal "FooBar", op2.classname
      assert_equal "root/cimv3", op2.namespace
    end
    
    should "be able to retrieve qualifiers" do
      op = Sfcc::Cim::ObjectPath.new("root/cimv2", "Linux_OperatingSystem")
      # sfcc does not implement ObjectPath#class_qualifier, ObjectPath#property_qualifier
      assert_raise Sfcc::Cim::ErrorNotSupported do
        assert_equal "2.17.1", @op.class_qualifier("Version")
        assert_equal "Number", op.property_qualifier("NumberOfUsers", "Description")
      end
    end    
  end
end
