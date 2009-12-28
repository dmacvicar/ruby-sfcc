require File.join(File.dirname(__FILE__), 'helper')
require 'pp'

class SfccCimcClass < SfccTestCase

  context "a CIM environment and client" do
    setup do
      setup_cim_environment_and_client
    end

    should "be running" do
      assert cimom_running?
    end
    
    context "class Linux_ComputerSystem" do
      setup do
        @op = @env.new_object_path("root/cimv2", "")
      end

      
      
    end
    
  end
end

