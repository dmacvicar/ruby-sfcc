require File.join(File.dirname(__FILE__), 'helper')
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

          should "allow for query" do
            @client.each_for_query(@op, "select * from CIM_ComputerSystem", "wql") do |instance|
              puts instance
            end
          end

          
          context "class names" do
            setup do
              @class_names = []
              @client.each_class_name(@op, Sfcc::CIMC_FLAG_DeepInheritance) { |name| @class_names << name }
            end

            should "include CIM_ManagedElement" do
              assert !@class_names.select { |x| x.to_s == "CIM_ManagedElement" }.empty?
            end

            should "have every element of type Sfcc::Cimc::ObjectPath" do
              @class_names.each { |n| assert_kind_of(Sfcc::Cimc::ObjectPath, n) }
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

        end
        
      end
    end
    
  end  
end
