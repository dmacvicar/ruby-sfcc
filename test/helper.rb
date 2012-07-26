require 'rubygems'
require 'test/unit'
require 'shoulda'
require 'tempfile'

$schema = "http"
$user = nil
$pw = nil
$hostname = "kvm-fedora17"
$port = "5988"

tmpdir = "../tmp/#{RUBY_PLATFORM}/sfcc/#{RUBY_VERSION}"

%w(../lib tmpdir).each do |path|
  $LOAD_PATH.unshift(File.expand_path(File.join(File.dirname(__FILE__), path)))
end
 
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
    #@client = Sfcc::Cim::Client.connect(:uri => $url, :verify => false)
    @client = Sfcc::Cim::Client.connect(:host => $hostname, :scheme => $schema,
                                        :user => $user, :password => $pw, :port => '5988')
  end
  
end
