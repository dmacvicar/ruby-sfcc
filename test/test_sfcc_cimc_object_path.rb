
require File.join(File.dirname(__FILE__), 'helper')
require 'pp'

class SfccCimcObjectPathTest < SfccTestCase

  context "a running CIMOM with no auth" do

    should "be running" do
      assert cimom_running?
    end

    context "and a new CIM environment" do
      setup do
        @env = Sfcc::Cimc::Environment.new("XML")
      end

      context "and a new object path for root/cimv2" do
        setup do
          @op = @env.new_object_path("root/cimv2", "")
        end

        should "have root/cimv2 as namespace" do
          assert_equal "root/cimv2", @op.namespace
        end

        should "change its namespace after setting it" do
          @op.namespace = "root/cimv3"
          assert_equal "root/cimv3", @op.namespace
        end

        should "have respond to hostname" do
          assert_nil @op.hostname
        end

        should "change its hostname after setting it" do
          @op.namespace = "foo.bar.com"
          assert_equal "foo.bar.com", @op.namespace
        end

        should "respond to class_name" do
          assert_equal "", @op.class_name
        end

        should "change its class name after setting it" do
          @op.class_name = "ClassName"
          assert_equal "ClassName", @op.class_name
        end
        
      end            
    end
  end
end
