require File.join(File.dirname(__FILE__), 'helper')
require 'pp'

class SfccCimcClass < SfccTestCase

  context "a CIM environment and client" do
    setup do
      setup_cim_client
    end

    should "be running" do
      assert cimom_running?
    end    
  end
  
end

