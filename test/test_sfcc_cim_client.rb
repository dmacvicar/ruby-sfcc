require_relative 'helper'

describe 'a running CIMOM' do
  include SfccTestUtils

  before do
    setup_cim_client
    @op = Sfcc::Cim::ObjectPath.new('root/cimv2')
  end

  it 'should be running' do
    assert cimom_running?
  end

  it 'should be of class Client' do
    assert_kind_of(Sfcc::Cim::Client, @client)
  end

  it 'should allow for query' do
    result = @client.query(@op, 'select * from CIM_OperatingSystem', 'wql')
    count = 0
    result.each do |instance|
      assert instance
      count += 1
    end
    assert count > 0
  end

  it 'should be able to get set properties using an object path' do
    @op = Sfcc::Cim::ObjectPath.new('root/cimv2', 'Linux_OperatingSystem')
    # @client.instance_names(@op).each do |path|
    @client.query(@op, 'select * from Linux_OperatingSystem', 'wql').each do |_instance|
      #        assert ! @client.property(instance.object_path, "PrimaryOwnerContact").empty?
    end
  end

  it 'should be able to get a class from the object path' do
    @op = Sfcc::Cim::ObjectPath.new('root/cimv2', 'Linux_OperatingSystem')
    cimclass = @client.get_class(@op)
    assert_kind_of Sfcc::Cim::Class, cimclass
  end

  it 'should report error when getting invalid class' do
    @op = Sfcc::Cim::ObjectPath.new('root/cimv2', 'NotExistingClass')
    assert_raises Sfcc::Cim::ErrorNotFound do
      cimclass = @client.get_class(@op)
    end
  end

  it 'should be able to get an instance from the object path' do
    @op = Sfcc::Cim::ObjectPath.new('root/cimv2', 'Linux_OperatingSystem')
    instance = @client.query(@op, 'select * from Linux_OperatingSystem', 'wql').to_a.first
    instance2 = @client.get_instance(instance.object_path)
    assert_kind_of Sfcc::Cim::Instance, instance2
  end

  it 'should be able to create an instance' do
    op = Sfcc::Cim::ObjectPath.new('root/cimv2', 'Linux_OperatingSystem')
    instance = Sfcc::Cim::Instance.new(op)
    assert instance
    assert_raises Sfcc::Cim::ErrorNotSupported do
      new_op = @client.create_instance(op, instance)
    end
  end

  it 'should be able to set an instance' do
    @op = Sfcc::Cim::ObjectPath.new('root/cimv2', 'Linux_OperatingSystem')
    instance = @client.query(@op, 'select * from Linux_OperatingSystem', 'wql').to_a.first
    assert instance
    assert_raises Sfcc::Cim::ErrorNotSupported do
      instance = @client.set_instance(instance.object_path, instance)
    end
  end

  it 'should be able to delete an instance' do
    instance = @client.query(@op, 'select * from Linux_OperatingSystem', 'wql').to_a.first
    assert instance
    assert_raises Sfcc::Cim::ErrorNotSupported do
      @client.delete_instance(instance.object_path)
    end
  end

  it 'should be able to get associator for an instance' do
    op = Sfcc::Cim::ObjectPath.new('root/cimv2')
    op = @client.query(op, 'select * from CIM_ComputerSystem', 'wql').to_a.first.object_path
    associators = @client.associators(op, 'CIM_RunningOS').to_a
    assert !associators.empty?
    associators.each { |assoc| assert_kind_of Sfcc::Cim::Instance, assoc }
    #      pp associators
  end

  it 'should be able to get associator names for an instance' do
    op = Sfcc::Cim::ObjectPath.new('root/cimv2')
    op = @client.query(op, 'select * from CIM_ComputerSystem', 'wql').to_a.first.object_path
    associators = @client.associator_names(op, 'CIM_RunningOS').to_a
    assert !associators.empty?
    associators.each { |assoc| assert_kind_of Sfcc::Cim::ObjectPath, assoc }
    #      pp associators
  end

  it 'should be able to get references for an instance' do
    op = Sfcc::Cim::ObjectPath.new('root/cimv2')
    op = @client.query(op, 'select * from CIM_OperatingSystem', 'wql').to_a.first.object_path
    associators = @client.references(op, 'CIM_RunningOS').to_a
    assert !associators.empty?
    associators.each { |assoc| assert_kind_of Sfcc::Cim::Instance, assoc }
    #      pp associators
  end

  it 'should be able to get reference names for an instance' do
    op = Sfcc::Cim::ObjectPath.new('root/cimv2')
    op = @client.query(op, 'select * from CIM_OperatingSystem', 'wql').to_a.first.object_path
    associators = @client.reference_names(op, 'CIM_RunningOS').to_a
    assert !associators.empty?
    associators.each { |assoc| assert_kind_of Sfcc::Cim::ObjectPath, assoc }
    #      pp associators
  end

  it 'should be able to invoke methods using an object path' do
    @op = Sfcc::Cim::ObjectPath.new('root/cimv2', 'Linux_OperatingSystem')
    @client.query(@op, 'select * from Linux_OperatingSystem', 'wql').each do |instance|
      # @client.instances(@op, Sfcc::Flags::IncludeClassOrigin | Sfcc::Flags::IncludeQualifiers, nil).each do |instance|
      path = instance.object_path
      assert_kind_of Sfcc::Cim::ObjectPath, path
      out = {}
      ret = @client.invoke_method(path, :execCmd, { cmd: 'cat /etc/SuSE-release' }, out)
      assert out.key?(:out), 'output parameter is present'
      assert out[:out].match(/VERSION/)
      assert_equal 0, ret, 'execCmd returns 0'
    end
  end

  describe 'class names' do
    before do
      @class_names = @client.class_names(@op, Sfcc::Flags::DeepInheritance)
    end

    it 'should be a Cimc::Enumeration' do
      assert_kind_of(Sfcc::Cim::Enumeration, @class_names)
    end

    it 'should include CIM_ManagedElement' do
      assert !@class_names.select { |x| x.to_s == 'CIM_ManagedElement' }.empty?
    end

    it 'should have every element of type Sfcc::Cim::ObjectPath' do
      @class_names.each { |n| assert_kind_of(Sfcc::Cim::ObjectPath, n) }
    end
  end

  describe 'classes' do
    before do
      @classes = @client.classes(@op, Sfcc::Flags::DeepInheritance)
    end

    it 'should be a Cimc::Enumeration' do
      assert_kind_of(Sfcc::Cim::Enumeration, @classes)
    end

    it 'should have every alement of type Cimc::Class' do
      count = 0
      @classes.each do |c|
        count += 1
        assert_kind_of(Sfcc::Cim::Class, c)
      end
      assert count > 0
    end
  end
end
