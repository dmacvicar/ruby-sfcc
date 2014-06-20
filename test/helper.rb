require 'rubygems'
gem 'minitest' if RUBY_VERSION >= "2.1.0"
require 'test/unit'
gem 'shoulda'
require 'shoulda/context'
require 'tempfile'

tmpdir = "../tmp/#{RUBY_PLATFORM}/sfcc/#{RUBY_VERSION}"

%w(../lib tmpdir).each do |path|
  $LOAD_PATH.unshift(File.expand_path(File.join(File.dirname(__FILE__), path)))
end
 
require 'rubygems'
require 'sfcc'
 
class SfccTestCase < Test::Unit::TestCase
  ASSETS = File.expand_path(File.join(File.dirname(__FILE__), 'assets'))
 
  unless RUBY_VERSION >= '1.9'
    undef :default_test
  end

  def cimom_running?
    `ps -e`.each_line do |line|
      return true if line =~ /sfcbd/
    end
    return false
  end

  def setup_cim_client
    @client = Sfcc::Cim::Client.connect(:uri => 'https://wsman:secret@localhost:5989', :verify => false)
    # @client = Sfcc::Cim::Client.connect(:host => 'localhost', :scheme => 'http', :user => 'root', :port => '5988')
  end
  
end
