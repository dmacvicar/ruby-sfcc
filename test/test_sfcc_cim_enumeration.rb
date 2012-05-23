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

    should "be an enumeration" do
      assert_kind_of(Sfcc::Cim::Enumeration, @enm)
    end

    should "be able to iterate twice" do
      assert !@enm.to_a.empty?
      assert !@enm.to_a.empty?
    end

    should "have a size" do
      assert @enm.size > 0
    end

    should "have a simple type" do
      assert @enm.simple_type > 0
    end

    should "has an array representation" do
      assert_kind_of(Array, @enm.to_a)
    end
  end   
end            

