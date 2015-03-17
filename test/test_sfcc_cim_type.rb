require_relative 'helper'

describe "a Cim::Class representation" do
  include SfccTestUtils

  before do
    setup_cim_client
    op = Sfcc::Cim::ObjectPath.new("root/cimv2", "CIM_ComputerSystem")
    @cimclass = @client.get_class(op)
  end
  
  it "should provides Cim::Type for all its properties" do
    @cimclass.each_property do |k, d|
      assert_kind_of(Sfcc::Cim::Type, d.type)
    end
  end
  
  it "should have string and Integer representations of Cim::Type for all its properties" do
    @cimclass.each_property do |k, d|
      assert_kind_of(String, d.type.to_s)
      assert_kind_of(Integer, d.type.to_i)
      if d.type.to_s =~ /\[\]/
        assert d.type.array?
      else
        assert !d.type.array?
        assert d.type.integer? if d.type.to_s =~ /int/
        assert d.type.real? if d.type.to_s =~ /real/
        assert d.type.string? if d.type.to_s =~ /string/
      end
    end
  end
  
end

