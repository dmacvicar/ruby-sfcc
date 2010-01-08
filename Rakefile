$: << File.join(File.dirname(__FILE__), "test")
require 'rubygems'
gem 'hoe', '>= 2.1.0'
require 'hoe'

#require 'rake/gempackagetask'
#require 'rake/rdoctask'
#require 'rake/testtask'

task :default => [:compile, :docs, :test]

HOE = Hoe.spec 'sfcc' do
  developer('Duncan Mac-Vicar P.', 'dmacvicar@suse.de')
  self.summary = "sblim client ruby 
bindings"
  self.description = "sblim client ruby bindings"
  self.readme_file = ['README', ENV['HLANG'], 'rdoc'].compact.join('.')
  self.history_file = ['CHANGELOG', ENV['HLANG'], 'rdoc'].compact.join('.')
  self.extra_rdoc_files = FileList['*.rdoc']
  self.clean_globs = [
    'lib/sfcc/*.{o,so,bundle,a,log,dll}',
    'lib/sfcc/sfcc.rb',
  ]
 
  %w{ rake-compiler }.each do |dep|
    self.extra_dev_deps << [dep, '>= 0']
  end
 
  self.spec_extras = { :extensions => ["ext/sfcc/extconf.rb"] }
end

gem 'rake-compiler', '>= 0.4.1'
require 'rake/extensiontask'
Rake::ExtensionTask.new('sfcc')
