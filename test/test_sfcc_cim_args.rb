require File.join(File.dirname(__FILE__), 'helper')
require 'pp'

class SfccCimcClass < SfccTestCase

  context "a CIM arguments object" do
    setup do
      setup_cim_client
      @args = Sfcc::Cim::Args.new
    end

    should "be running" do
      assert cimom_running?
    end

    should "have initial size of 0" do
      assert_equal 0, @args.size
    end

    should "be able to add args" do
      @args.add_arg("foo", "bar")
      @args.add_arg("hoo", 2)
      assert_equal 2, @args.size

      assert_kind_of Hash, @args.to_hash
      @args.map { |name, val| puts "#{name} -> #{val}" }
      
    end

    should "be able to add args using hash style" do
      @args["faa"] = "foo"
      @args["haa"] = 3
      assert_equal 2, @args.size

      assert_kind_of Hash, @args.to_hash
      @args.map { |name, val| puts "#{name} -> #{val}" }
      
    end

    
  end
  
end

