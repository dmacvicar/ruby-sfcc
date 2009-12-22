require 'helper'
require 'pp'

class BasicTest < SfccTestCase
  
  def setup
  end

  def teardown
  end

  def test_cimom_running?
    assert cimom_running?, "CIMOM has to be running"
  end

  def test_basic
    env = Sfcc::Cimc::Environment.new("XML")
    assert_kind_of(Sfcc::Cimc::Environment, env)
    client = env.connect("localhost", "http", "5988", "root", "")
    assert_kind_of(Sfcc::Cimc::Client, client)

    op = env.new_object_path("root/cimv2", "")
    assert_kind_of(Sfcc::Cimc::ObjectPath, op)

    result = []
    client.each_class_name(op, Sfcc::CIMC_FLAG_DeepInheritance) { |name| result << name }
    assert result.include?("CIM_ManagedElement"), "class names include base CIM_ManagedElement"

    result = []
    op = env.new_object_path("root/cimv2", "")
    client.each_class(op, Sfcc::CIMC_FLAG_DeepInheritance) { |name| result << name }
    assert_kind_of(Sfcc::Cimc::Class, result.first)

    op = env.new_object_path("root/cimv2", "CIM_ManagedElement")
    cim_managed_element = client.get_class(op, 0, [])
    assert_kind_of(Sfcc::Cimc::Class, cim_managed_element)
    assert_equal(cim_managed_element.class_name, "CIM_ManagedElement")

    op = env.new_object_path("root/cimv2", "CIM_ComputerSystem")
    result = []
    client.each_instance_name(op) { |name| result << name }
    assert ! result.empty?, "At least one CIM_ComputerSystem as an instance"
  end
  
end
