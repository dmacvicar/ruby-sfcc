require 'test/unit'
require 'tempfile'
 
%w(../lib ../ext).each do |path|
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
  
end
