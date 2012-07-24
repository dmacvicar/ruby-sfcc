require File.expand_path(File.join(File.dirname(__FILE__), 'helper'))
#require 'pp'

class SfccCimcClass < SfccTestCase

  context "with a Cim::Class it" do
    setup do
      setup_cim_client
      op = Sfcc::Cim::ObjectPath.new("root/cimv2", "CIM_ComputerSystem")
      @cimclass = @client.get_class(op, Sfcc::Flags::IncludeQualifiers|Sfcc::Flags::IncludeClassOrigin)
    end

    should "have a name" do
      @cimclass.class_name
    end

    should "get a class" do
      assert @cimclass
    end

    context "with a cimclass" do
      should "have a string name" do
        assert @cimclass.class_name
        assert_kind_of String, @cimclass.class_name
      end

      should "have properties" do
        assert @cimclass.property_count > 0
      end

      should "be able to enumerate properties" do
        @cimclass.each_property do |k, d|
          assert_not_nil(k)
          assert_kind_of(Sfcc::Cim::Data, d)
        end

        properties = @cimclass.properties
        assert !properties.empty?
        assert_equal properties.size, @cimclass.property_count
      end

      should "get a property by name" do
        assert @cimclass.property "Name"
      end

      should "have qualifiers" do
        assert @cimclass.qualifier_count > 0
      end

      should "be able to enumerate qualifiers" do
        @cimclass.each_qualifier do |k, d|
          assert_not_nil(k)
          assert_kind_of(Sfcc::Cim::Data, d)
        end

        qualifiers = @cimclass.qualifiers
        assert_equal qualifiers.size, @cimclass.qualifier_count
      end

      should "get a qualifier by name" do
        assert @cimclass.qualifier "Description"
      end

      should "have property qualifiers" do
        assert @cimclass.property_qualifier_count("Status") > 0
      end

      should "get a property qualifier" do
        assert @cimclass.property_qualifier("Status","Description")
      end

      should "be able to enumerate qualifiers for a property" do
        @cimclass.each_property_qualifier("Status") do |k, d|
          assert_not_nil(k)
          assert_kind_of(Sfcc::Cim::Data, d)
        end
      end
    end # context

    context "and XML connect without extension callbacks" do
      should "have a string superclass name" do
        if Sfcc::Cim::CIMC_ENV_TYPE == "XML"
          assert_raise Sfcc::Cim::ErrorNotSupported do
            @cimclass.superclass_name
          end
        else
          assert @cimclass.superclass_name
          assert_kind_of String, @cimclass.superclass_name
        end
      end

      should "have key properties" do
        if Sfcc::Cim::CIMC_ENV_TYPE == "XML"
          assert_raise Sfcc::Cim::ErrorNotSupported do
            assert @cimclass.keys
          end
        else
          assert @cimclass.keys
          assert_kind_of Array, @cimclass.keys
        end
      end

      should "answer if its an association" do
        assert_raise Sfcc::Cim::ErrorNotSupported do
          assert !@cimclass.association?
        end
      end
      should "answer if its abstract" do
        assert_raise Sfcc::Cim::ErrorNotSupported do
          assert !@cimclass.abstract?
        end
      end
      should "answer if its an indication" do
        assert_raise Sfcc::Cim::ErrorNotSupported do
          assert !@cimclass.indication?
        end
      end

    end # context

  end

end

