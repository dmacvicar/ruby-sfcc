require_relative 'helper'

describe "enumeration of instances" do
  include SfccTestUtils

  before do
    setup_cim_client
    op = Sfcc::Cim::ObjectPath.new("root/cimv2")
    @enm = @client.query(op, "select * from CIM_ComputerSystem", "wql")
  end
  
  it "should be running" do
      assert cimom_running?
  end
  
  it "should be an enumeration" do
    assert_kind_of(Sfcc::Cim::Enumeration, @enm)
  end
  
  it "should be able to iterate twice" do
    assert !@enm.to_a.empty?
    assert !@enm.to_a.empty?
  end
  
  it "should have a size" do
    assert @enm.size > 0
  end
  
  it "should have a simple type" do
    assert @enm.simple_type > 0
  end
  
  it "should has an array representation" do
    assert_kind_of(Array, @enm.to_a)
  end
end   


