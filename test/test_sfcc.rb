require 'helper'
require 'pp'

class BasicTest < SfccTestCase

  context "a running CIMOM with no auth" do
    setup do
    end

    should "be running" do
      assert cimom_running?
    end

    context "a new CIM environment" do
      setup do
        @env = Sfcc::Cimc::Environment.new("XML")
      end

      should "be of class Environment" do
        assert_kind_of(Sfcc::Cimc::Environment, @env)
      end

      context "a connection to the CIMOM" do
        setup do
          @client = @env.connect("localhost", "http", "5988", "root", "")
        end

        should "be of class Client" do
          assert_kind_of(Sfcc::Cimc::Client, @client)
        end

        context "a new object path for root/cimv2" do
          setup do
            @op = @env.new_object_path("root/cimv2", "")
          end

          should "be of class ObjectPath" do
            assert_kind_of(Sfcc::Cimc::ObjectPath, @op)
          end

          context "class names" do
            setup do
              @class_names = []
              @client.each_class_name(@op, Sfcc::CIMC_FLAG_DeepInheritance) { |name| @class_names << name }
            end

            should "include CIM_ManagedElement string" do
              assert @class_names.include?("CIM_ManagedElement")
            end

            should "have every element of type String" do
              @class_names.each { |n| assert_kind_of(String, n) }
            end            
          end

          context "classes" do
            setup do
              @cimclasses = []
              @client.each_class(@op, Sfcc::CIMC_FLAG_DeepInheritance) { |cimclass| @cimclasses << cimclass }
            end

            should "have every alement of type Cimc::Class" do
              @cimclasses.each { |c| assert_kind_of(Sfcc::Cimc::Class, c) }
            end            
          end

          context "CIM_ComputerSystem class" do
            setup do
              @op_computer_system = @env.new_object_path("root/cimv2", "CIM_ComputerSystem")
              @cim_computer_system = @client.get_class(@op_computer_system, 0, [])
            end

            should "be of class Cimc::Class" do
              assert_kind_of(Sfcc::Cimc::Class, @cim_computer_system)
            end

            should "have CIM_ComputerSystem as class name attribute" do
              assert_equal(@cim_computer_system.class_name, "CIM_ComputerSystem")
            end

            context "it instance names" do
              setup do
                @cim_instance_names = []
                @client.each_instance_name(@op_computer_system) { |i| @cim_instance_names << i }
              end
              should "not be empty, at least one CIM_ComputerSystem has to be there" do
                assert ! @cim_instance_names.empty?
              end

              should "have all elements of type String" do
                @cim_instance_names.each { |i| assert_kind_of(String, i) }
              end

            end

            context "instances of CIM_ComputerSystem" do
              setup do
                @cim_instances = []
                @client.each_instance(@op_computer_system, 0, []) { |i| @cim_instances << i }
              end

              should "have all elements of type Cimc::Instance" do
                @cim_instances.each { |i| assert_kind_of(Sfcc::Cimc::Instance, i) }
              end

              #should "have the same object path as the first instance name" do
              # assert_equal @cim_instance_names.first, @cim_instances.first.object_path.to_s
              #end
              
            end
            
          end

        end
        
      end
    end
    
  end  
end
