$: << File.join(File.dirname(__FILE__), "test")
require 'rubygems'
require 'rake/gempackagetask'
require 'rake/extensiontask'

task :default => :test

#task :test do
#  require File.dirname(__FILE__) + '/test/all_tests.rb'
# end

spec = Gem::Specification.new do |s|
    s.platform  =   Gem::Platform::RUBY
    s.name      =   "sfcc"
    s.version   =   "0.1"
    s.author    =   "Duncan Mac-Vicar P"
    s.email     =   "dmacvicar@suse.de"
    s.summary   =   "sblim client"
    s.files     =   FileList['lib/*.rb', 'test/*'].to_a
    s.require_path  =   "lib"
 #   s.autorequire   =   "ip_admin"
    s.test_files = Dir.glob('tests/*.rb')
    s.has_rdoc  =   true
    s.extra_rdoc_files  =   ["README"]
end

Rake::GemPackageTask.new(spec) do |pkg|
    pkg.need_tar = true
end

Rake::ExtensionTask.new('sfcc')