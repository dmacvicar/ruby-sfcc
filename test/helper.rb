require 'minitest/autorun'
require 'tempfile'

%W(../lib ../tmp/#{RUBY_PLATFORM}/sfcc/#{RUBY_VERSION}).each do |path|
  $LOAD_PATH.unshift(File.expand_path(File.join(File.dirname(__FILE__), path)))
end

require 'sfcc'

# Util methods shared across specs
module SfccTestUtils
  ASSETS = File.expand_path(File.join(File.dirname(__FILE__), 'assets'))

  def cimom_running?
    `ps -e`.each_line do |line|
      return true if line =~ /sfcbd/
    end
    false
  end

  def setup_cim_client
    @client = Sfcc::Cim::Client.connect(
      uri: 'https://wsman:secret@localhost:5989', verify: false)
  end
end

# Same for unit tests
class SfccTestCase < MiniTest::Test
  include SfccTestUtils
end
