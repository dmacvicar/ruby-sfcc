
require File.join(File.dirname(__FILE__), 'helper')
require 'pp'

class SfccCimcObjectPathTest < SfccTestCase

  context "object path for CIM_ComputerSystem and running CIMOM with no auth" do

    setup do
      @env = Sfcc::Cimc::Environment.new("XML")
      @op_computer_system = @env.new_object_path("root/cimv2", "CIM_ComputerSystem")
      @client = @env.connect("localhost", "http", "5988", "root", "")
    end
    
    should "have CIMOM running" do
      assert cimom_running?
    end


    context "CIM_ComputerSystem class" do
      setup do
        @cim_computer_system = @client.get_class(@op_computer_system, 0, nil)
      end

      should "be of class Cimc::Class" do
        assert_kind_of(Sfcc::Cimc::Class, @cim_computer_system)
      end

      should "have CIM_ComputerSystem as class name attribute" do
        assert_equal(@cim_computer_system.class_name, "CIM_ComputerSystem")
      end

      context "it instance names" do
        setup do
          @instance_names = @client.instance_names(@op_computer_system)
        end
        should "not be empty, at least one CIM_ComputerSystem has to be there" do
          assert @instance_names.any?
        end

        should "have all elements of type ObjectPath" do
          @instance_names.each { |i| assert_kind_of(Sfcc::Cimc::ObjectPath, i) }
        end
      end

      context "instances of CIM_ComputerSystem" do
        setup do
          @instances = @client.instances(@op_computer_system, 0, nil)
        end

        should "have all elements of type Cimc::Instance" do
          @instances.each { |i| assert_kind_of(Sfcc::Cimc::Instance, i) }
        end

        should "respond to property Name" do
          @instances.each do |i|
            assert_kind_of(String, i.property('Name'))
            #puts i.property('Name')
          end
        end

        should "be able to iterate over properties" do
          @instances.each do |i|
            i.each_property do |name, value|              
              puts "#{name} -> #{value}"
              puts "---"
            end
          end
        end

        should "be able to set and retrieve stringproperties" do
          @instances.each do |i|
            assert_raises Sfcc::Cimc::ErrorNoSuchProperty do
              i.property("foobar");
            end
            assert_nothing_raised do
              # can't work, sfcc bug 2921255 see:
              # https://sourceforge.net/tracker/?func=detail&aid=2921255&group_id=128809&atid=712784
              #i.set_property("Name", "newname");
              i.set_property("Name", @env.new_string("newname"));
            end
            assert_equal "newname", i.property("Name")
          end
        end
                        
      end            
    end
  end
end
