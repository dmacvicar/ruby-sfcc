require_relative 'helper'

def check_data(d)
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

describe Sfcc::Cim::Data do
  include SfccTestUtils

  before do
    setup_cim_client
    op = Sfcc::Cim::ObjectPath.new('root/cimv2', 'CIM_ComputerSystem')
    @cimclass = @client.get_class(op)
  end

  it 'should provide Cim::Data for all its properties' do
    @cimclass.each_property do |_k, d|
      assert_kind_of(Sfcc::Cim::Data, d)
      check_data d
    end
  end

  it 'should provide Cim::Data for all its qualifiers' do
    @cimclass.each_qualifier do |_k, d|
      assert_kind_of(Sfcc::Cim::Data, d)
      check_data d
    end
  end

  it 'should provide Cim::Data for all qualifiers for a property' do
    @cimclass.each_property_qualifier('Status') do |k, d|
      refute_nil(k)
      assert_kind_of(Sfcc::Cim::Data, d)
      check_data d
    end
  end

  it 'should allow to instantiate Cim::Data from value' do
    d = Sfcc::Cim::Data.from_value('abcd')
    assert_equal(d.type.to_i, Sfcc::Cim::Type::String)
    assert_equal(d.state, Sfcc::Cim::Data::Good)
    assert_kind_of(String, d.value)
    assert_equal(d.value, 'abcd')
    d = Sfcc::Cim::Data.from_value(45)
    assert_equal(d.type.to_i, Sfcc::Cim::Type::SInt64)
    assert_equal(d.state, Sfcc::Cim::Data::Good)
    assert_kind_of(Fixnum, d.value)
    assert_equal(d.value, 45)
    str = Sfcc::Cim::String.new('sfcc string')
    d = Sfcc::Cim::Data.from_value(str)
    assert_equal(d.type.to_i, Sfcc::Cim::Type::String)
    assert_equal(d.state, Sfcc::Cim::Data::Good)
    assert_kind_of(String, d.value)
    assert_equal(d.value, 'sfcc string')
    op = Sfcc::Cim::ObjectPath.new('namespace', 'classname')
    d = Sfcc::Cim::Data.from_value(op)
    assert_equal(d.type.to_i, Sfcc::Cim::Type::Reference)
    assert_equal(d.state, Sfcc::Cim::Data::Good)
    assert_kind_of(Sfcc::Cim::ObjectPath, d.value)
    d = Sfcc::Cim::Data.from_value(nil)
    assert_equal(d.type.to_i, Sfcc::Cim::Type::Null)
    assert_equal(d.state, Sfcc::Cim::Data::Null)
    assert_equal(d.value, nil)
    d = Sfcc::Cim::Data.from_value(4.32)
    assert_equal(d.type.to_i, Sfcc::Cim::Type::Real64)
    assert_equal(d.state, Sfcc::Cim::Data::Good)
    assert_equal(d.value, 4.32)
  end

  it 'should allow to instantiate integer from string' do
    d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::SInt8, '-2')
    assert_equal(d.type.to_i, Sfcc::Cim::Type::SInt8)
    assert_equal(d.state, Sfcc::Cim::Data::Good)
    assert_equal(d.value, -2)
    d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::SInt8, '266')
    assert_equal(d.type.to_i, Sfcc::Cim::Type::SInt8)
    assert_equal(d.state, Sfcc::Cim::Data::Good)
    assert_equal(d.value, 10)
  end

  it 'should allow to instantiate Cim::Data with type as string' do
    d = Sfcc::Cim::Data.new('String', 'abc')
    assert_equal(d.type.to_i, Sfcc::Cim::Type::String)
    assert_equal(d.value, 'abc')
    d = Sfcc::Cim::Data.new('UInt16', '432')
    assert_equal(d.type.to_i, Sfcc::Cim::Type::UInt16)
    assert_equal(d.value, 432)
    op = Sfcc::Cim::ObjectPath.new('namespace', 'classname')
    d = Sfcc::Cim::Data.new('Reference', op)
    assert_equal(d.type.to_i, Sfcc::Cim::Type::Reference)
    assert_kind_of(Sfcc::Cim::ObjectPath, d.value)
    assert_raises NameError do
      d = Sfcc::Cim::Data.new('unknown type', 'some value')
    end
  end

  it 'should allow to instantiate Cim::Data with type' do
    d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::String, 'abc')
    assert_equal(d.type.to_i, Sfcc::Cim::Type::String)
    assert_equal(d.state, Sfcc::Cim::Data::Good)
    assert_equal(d.value, 'abc')
    d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::UInt8, 10)
    assert_equal(d.type.to_i, Sfcc::Cim::Type::UInt8)
    assert_equal(d.state, Sfcc::Cim::Data::Good)
    assert_equal(d.value, 10)
    assert_raises TypeError do
      d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::Reference, 'not a reference')
    end
    assert_raises TypeError do
      d = Sfcc::Cim::Data.new(
        'String', Sfcc::Cim::ObjectPath.new('namespace', 'classname'))
    end
  end

  it 'should allow to change type of Cim::Data' do
    d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::String, 'abc')
    assert_equal(d.type.to_i, Sfcc::Cim::Type::String)
    d.type = Sfcc::Cim::Type::SInt16
    assert_equal(d.type.to_i, Sfcc::Cim::Type::SInt16)
    assert_equal(d.state, Sfcc::Cim::Data::Null)
    assert_equal(d.value, nil)
    d.type = 'UInt16'
    assert_equal(d.type.to_i, Sfcc::Cim::Type::UInt16)
    d2 = Sfcc::Cim::Data.new(Sfcc::Cim::Type::String, 'abcd')
    assert_equal(d2.type.to_i, Sfcc::Cim::Type::String)
    d2.type = d.type
    assert_equal(d2.type.to_i, Sfcc::Cim::Type::UInt16)
  end

  it 'should allow to change value of Cim::Data' do
    d = Sfcc::Cim::Data.new(Sfcc::Cim::Type::String, 'abc')
    assert_equal(d.value, 'abc')
    d.value = Sfcc::Cim::String.new('tmp string')
    assert_equal(d.value, 'tmp string')
    assert_raises TypeError do
      d.value = Sfcc::Cim::ObjectPath.new('namespace', 'classname')
    end
    d.type = Sfcc::Cim::Type::Real64
    assert_equal(d.state, Sfcc::Cim::Data::Null)
    d.value = 15.2
    assert_equal(d.state, Sfcc::Cim::Data::Good)
    assert_equal(d.value, 15.2)
  end

  it 'should allow to instantiate Cim::Data from array' do
    d = Sfcc::Cim::Data.new('StringA', %w(abc def))
    assert_equal(d.type.to_i, Sfcc::Cim::Type::StringA)
    assert_kind_of(Array, d.value)
    assert_equal(d.value, %w(abc def))
    d = Sfcc::Cim::Data.from_value([
      Sfcc::Cim::ObjectPath.new('namespace', 'classname'),
      Sfcc::Cim::ObjectPath.new('nm2', 'cls2')])
    assert_equal(d.type.to_i, Sfcc::Cim::Type::ReferenceA)
    assert_kind_of(Array, d.value)
    assert_equal(d.value.size, 2)
    assert_equal(d.value[0].namespace, 'namespace')
    assert_equal(d.value[0].classname, 'classname')
    # forbid array creation from array with different value types
    assert_raises TypeError do
      d = Sfcc::Cim::Data.from_value([1, 'string'])
    end
  end

  it 'should allow to instantiate Cim::Data from cimc wrapped objects' do
    d = Sfcc::Cim::Data.from_value(@cimclass)
    assert_equal(d.type.to_i, Sfcc::Cim::Type::Class)
    assert_kind_of(Sfcc::Cim::Class, d.value)
    assert_equal(d.value.class_name, @cimclass.class_name)
    d = Sfcc::Cim::Data.from_value(Sfcc::Cim::String.new('cimstring'))
    assert_equal(d.type.to_i, Sfcc::Cim::Type::String)
    assert_kind_of(String, d.value)
    assert_equal(d.value, 'cimstring')
    @client.instances(
      Sfcc::Cim::ObjectPath.new('root/cimv2', 'CIM_ComputerSystem'),
      Sfcc::Flags::DeepInheritance).each do |i|
      d = Sfcc::Cim::Data.from_value(i)
      assert_equal(d.type.to_i, Sfcc::Cim::Type::Instance)
      assert_kind_of(Sfcc::Cim::Instance, d.value)
    end
  end
end
