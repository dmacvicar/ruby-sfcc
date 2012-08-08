require File.expand_path(File.join(File.dirname(__FILE__), 'helper'))
#require 'pp'

def check_data d
  assert d.state
  assert d.type
#  case d.type
#  when 0 then assert d.value # goodValue
#  when (1<<8) then assert_nil d.value # nullValue
#  when (2<<8) then d.value # keyValue
#  else
#    # notFound, badValue
#  end
end

class SfccCimcData < SfccTestCase

  context "a Cim::Class representation" do
    setup do
      setup_cim_client
      op = Sfcc::Cim::ObjectPath.new("root/cimv2", "CIM_ComputerSystem")
      @cimclass = @client.get_class(op)
    end
    
    should "provide Cim::Data for all its properties" do
      @cimclass.each_property do |k, d|
        assert_kind_of(Sfcc::Cim::Data, d)
        check_data d
      end
    end

    should "provide Cim::Data for all its qualifiers" do
      @cimclass.each_qualifier do |k, d|
        assert_kind_of(Sfcc::Cim::Data, d)
        check_data d
      end
    end

    should "provide Cim::Data for all qualifiers for a property" do
      @cimclass.each_property_qualifier("Status") do |k, d|
        assert_not_nil(k)
        assert_kind_of(Sfcc::Cim::Data, d)
        check_data d
      end      
    end

    should "allow to instantiate Cim::Data from value" do
      d = Sfcc::Cim::Data.from_value("abcd")
      assert_equal(d.type.to_i, Sfcc::Cim::Type::String)
      assert_equal(d.state, Sfcc::Cim::Data::Good)
      assert_kind_of(String, d.value)
      assert_equal(d.value, "abcd")
      d = Sfcc::Cim::Data.from_value(45)
      assert_equal(d.type.to_i, Sfcc::Cim::Type::SInt64)
      assert_equal(d.state, Sfcc::Cim::Data::Good)
      assert_kind_of(Fixnum, d.value)
      assert_equal(d.value, 45)
      str = Sfcc::Cim::String.new("sfcc string")
      d = Sfcc::Cim::Data.from_value(str)
      assert_equal(d.type.to_i, Sfcc::Cim::Type::String)
      assert_equal(d.state, Sfcc::Cim::Data::Good)
      assert_kind_of(String, d.value)
      assert_equal(d.value, "sfcc string")
      d = Sfcc::Cim::Data.from_value(nil)
      assert_equal(d.type.to_i, Sfcc::Cim::Type::Null)
      assert_equal(d.state, Sfcc::Cim::Data::Null)
      assert_equal(d.value, nil)
      d = Sfcc::Cim::Data.from_value(4.32)
      assert_equal(d.type.to_i, Sfcc::Cim::Type::Real64)
      assert_equal(d.state, Sfcc::Cim::Data::Good)
      assert_equal(d.value, 4.32)
    end

    should "allow to instantiate integer from string" do
      d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::SInt8, "-2")
      assert_equal(d.type.to_i, Sfcc::Cim::Type::SInt8)
      assert_equal(d.state, Sfcc::Cim::Data::Good)
      assert_equal(d.value, -2)
      d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::SInt8, "266")
      assert_equal(d.type.to_i, Sfcc::Cim::Type::SInt8)
      assert_equal(d.state, Sfcc::Cim::Data::Good)
      assert_equal(d.value, 10)
    end

    should "allow to instantiate Cim::Data with type as string" do
      d = Sfcc::Cim::Data.new("String", "abc")
      assert_equal(d.type.to_i, Sfcc::Cim::Type::String)
      assert_equal(d.value, "abc")
      d = Sfcc::Cim::Data.new("UInt16", "432")
      assert_equal(d.type.to_i, Sfcc::Cim::Type::UInt16)
      assert_equal(d.value, 432)
      assert_raise NameError do
        d = Sfcc::Cim::Data.new("unknown type", "some value")
      end
    end

    should "allow to instantiate Cim::Data with type" do
      d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::String, "abc")
      assert_equal(d.type.to_i, Sfcc::Cim::Type::String)
      assert_equal(d.state, Sfcc::Cim::Data::Good)
      assert_equal(d.value, "abc")
      d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::UInt8, 10)
      assert_equal(d.type.to_i, Sfcc::Cim::Type::UInt8)
      assert_equal(d.state, Sfcc::Cim::Data::Good)
      assert_equal(d.value, 10)
      assert_raise TypeError do
        d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::Reference, "not a reference")
      end
    end

    should "allow to change type of Cim::Data" do
      d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::String, "abc")
      assert_equal(d.type.to_i, Sfcc::Cim::Type::String)
      d.type = Sfcc::Cim::Type::SInt16
      assert_equal(d.type.to_i, Sfcc::Cim::Type::SInt16)
      assert_equal(d.state, Sfcc::Cim::Data::Null)
      assert_equal(d.value, nil)
      d.type = "UInt16"
      assert_equal(d.type.to_i, Sfcc::Cim::Type::UInt16)
      d2 = Sfcc::Cim::Data.new(Sfcc::Cim::Type::String, "abcd")
      assert_equal(d2.type.to_i, Sfcc::Cim::Type::String)
      d2.type = d.type
      assert_equal(d2.type.to_i, Sfcc::Cim::Type::UInt16)
    end

    should "allow to change value of Cim::Data" do
      d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::String, "abc")
      assert_equal(d.value, "abc")
      d.value = Sfcc::Cim::String.new("tmp string")
      assert_equal(d.value, "tmp string")
      d.type = Sfcc::Cim::Type::Real64
      assert_equal(d.state, Sfcc::Cim::Data::Null)
      d.value = 15.2
      assert_equal(d.state, Sfcc::Cim::Data::Good)
      assert_equal(d.value, 15.2)
    end

    should "allow to instantiate Cim::Data from array" do
      d = Sfcc::Cim::Data.new("StringA", ["abc", "def"])
      assert_equal(d.type.to_i, Sfcc::Cim::Type::StringA)
      assert_kind_of(Array, d.value)
      assert_equal(d.value, ["abc", "def"])
      # forbid array creation from array with different value types
      assert_raise TypeError do
        d = Sfcc::Cim::Data.from_value([1, "string"])
      end
    end

  end
  
end
