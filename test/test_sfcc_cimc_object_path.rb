
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
          @op = @env.new_object_path("root/cimv2", "FooClass")
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
          assert_equal "FooClass", @op.class_name
        end

        should "change its class name after setting it" do
          @op.class_name = "BarClass"
          assert_equal "BarClass", @op.class_name
        end

        should "respond to keys and set them" do
          assert_raise Sfcc::Cimc::ErrorNoSuchProperty do
            @op.key("foo")
          end
          assert_equal 0, @op.key_count

          assert_raise TypeError do
            @op.add_key("prop_hash", Hash.new)
          end
          
          assert_nothing_raised do
            @op.add_key("prop0", "hello")
            @op.add_key("prop1", true)
            @op.add_key("prop2", false)
          end
          assert_equal 3, @op.key_count
          # can't work, sfcc bug 2921255 see:
          # https://sourceforge.net/tracker/?func=detail&aid=2921255&group_id=128809&atid=712784
          # assert_equal "hello", @op.key("prop0")
          assert_equal true, @op.key("prop1")
          assert_equal false, @op.key("prop2")
        end

      end            
    end
  end
end
