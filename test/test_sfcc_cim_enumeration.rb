require File.expand_path(File.join(File.dirname(__FILE__), 'helper'))
#require 'pp'

class SfccCimEnumerationTest < SfccTestCase

  context "enumeration of instances" do
    setup do
      setup_cim_client
      op = Sfcc::Cim::ObjectPath.new("root/cimv2")
      @enm = @client.query(op, "select * from CIM_ComputerSystem", "wql")
    end
    
    should "be running" do
      assert cimom_running?
    end

    should "be able to iterate twice" do
      assert !@enm.to_a.empty?
      assert !@enm.to_a.empty?
    end    
  end   
end            

