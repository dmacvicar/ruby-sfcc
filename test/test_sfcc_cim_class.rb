require File.expand_path(File.join(File.dirname(__FILE__), 'helper'))

describe Sfcc::Cim::Class do
  include SfccTestUtils

  before do
    setup_cim_client
    op = Sfcc::Cim::ObjectPath.new('root/cimv2', 'CIM_ComputerSystem')
    @cimclass = @client.get_class(
      op, Sfcc::Flags::IncludeQualifiers | Sfcc::Flags::IncludeClassOrigin)
  end

  it 'should have a name' do
    @cimclass.class_name
  end

  it 'should get a class' do
    assert @cimclass
  end

  describe 'with a cimclass' do
    it 'should have a string name' do
      assert @cimclass.class_name
      assert_kind_of String, @cimclass.class_name
    end

    it 'should have properties' do
      assert @cimclass.property_count > 0
    end

    it 'should be able to enumerate properties' do
      @cimclass.each_property do |k, d|
        refute_nil(k)
        assert_kind_of(Sfcc::Cim::Data, d)
      end

      properties = @cimclass.properties
      assert !properties.empty?
      assert_equal properties.size, @cimclass.property_count
    end

    it 'should get a property by name' do
      assert @cimclass.property 'Name'
    end

    it 'should have qualifiers' do
      assert @cimclass.qualifier_count > 0
    end

    it 'should be able to enumerate qualifiers' do
      @cimclass.each_qualifier do |k, d|
        refute_nil(k)
        assert_kind_of(Sfcc::Cim::Data, d)
      end

      qualifiers = @cimclass.qualifiers
      assert_equal qualifiers.size, @cimclass.qualifier_count
    end

    it 'should get a qualifier by name' do
      assert @cimclass.qualifier 'Description'
    end

    it 'should have property qualifiers' do
      assert @cimclass.property_qualifier_count('Status') > 0
    end

    it 'should get a property qualifier' do
      assert @cimclass.property_qualifier('Status', 'Description')
    end

    it 'should be able to enumerate qualifiers for a property' do
      @cimclass.each_property_qualifier('Status') do |k, d|
        refute_nil(k)
        assert_kind_of(Sfcc::Cim::Data, d)
      end
    end
  end

  describe 'and XML connect without extension callbacks' do
    it 'should have a string superclass name' do
      if Sfcc::Cim::CIMC_ENV_TYPE == 'XML'
        assert_raises Sfcc::Cim::ErrorNotSupported do
          @cimclass.superclass_name
        end
      else
        assert @cimclass.superclass_name
        assert_kind_of String, @cimclass.superclass_name
      end
    end

    it 'should have key properties' do
      if Sfcc::Cim::CIMC_ENV_TYPE == 'XML'
        assert_raises Sfcc::Cim::ErrorNotSupported do
          assert @cimclass.keys
        end
      else
        assert @cimclass.keys
        assert_kind_of Array, @cimclass.keys
      end
    end

    it 'should answer if its an association' do
      assert_raises Sfcc::Cim::ErrorNotSupported do
        assert !@cimclass.association?
      end
    end
    it 'should answer if its abstract' do
      assert_raises Sfcc::Cim::ErrorNotSupported do
        assert !@cimclass.abstract?
      end
    end
    it 'should answer if its an indication' do
      assert_raises Sfcc::Cim::ErrorNotSupported do
        assert !@cimclass.indication?
      end
    end
  end
end
