
require File.join(File.dirname(__FILE__), 'helper')
require 'pp'

class SfccCimInstanceTest < SfccTestCase

  context "object path for CIM_ComputerSystem and running CIMOM with no auth" do
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
    
    context "CIM_ComputerSystem class" do
      setup do
        @cim_computer_system = @client.get_class(@op_computer_system)
      end

      should "be of class Cimc::Class" do
        assert_kind_of(Sfcc::Cim::Class, @cim_computer_system)
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
          @instance_names.each { |i| assert_kind_of(Sfcc::Cim::ObjectPath, i) }
        end
      end

      context "instances of CIM_ComputerSystem" do
        setup do
          @instances = @client.instances(@op_computer_system)
        end

        should "have all elements of type Cimc::Instance" do
          @instances.each { |i| assert_kind_of(Sfcc::Cim::Instance, i) }
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
            assert_raises Sfcc::Cim::ErrorNoSuchProperty do
              i.property("foobar");
            end
            assert_nothing_raised do
              i.set_property("Name", "newname");
            end
            assert_equal "newname", i.property("Name")
          end
        end
                        
      end            
    end
  end
end
