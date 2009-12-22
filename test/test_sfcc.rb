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

    names = []
    client.each_class_name(op, Sfcc::CIMC_FLAG_DeepInheritance) { |name| names << name }
    assert names.include?("CIM_ManagedElement"), "class names include base CIM_ManagedElement"

    op = env.new_object_path("root/cimv2", "CIM_ManagedElement")
    cim_managed_element = client.get_class(op, 0, [])
    assert_kind_of(Sfcc::Cimc::Class, cim_managed_element)
    assert_equal(cim_managed_element.class_name, "CIM_ManagedElement")

    op = env.new_object_path("root/cimv2", "CIM_ComputerSystem")
    names = []
    client.each_instance_name(op) { |name| names << name }
    assert ! names.empty?, "At least one CIM_ComputerSystem as an instance"
  end
  
end
