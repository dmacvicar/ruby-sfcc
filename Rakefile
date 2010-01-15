$: << File.join(File.dirname(__FILE__), "test")
require 'rubygems'
gem 'hoe', '>= 2.1.0'
require 'hoe'

task :default => [:compile, :docs, :test]

Hoe.plugin :yard

HOE = Hoe.spec 'sfcc' do
  developer('Duncan Mac-Vicar P.', 'dmacvicar@suse.de')
  self.summary = "WBEM client for ruby based on the sblim-sfcc library"
  self.description = "ruby-sfcc allows to access a CIMOM either with the WBEM protocol or by using the SfcbLocal interface provided by the sblim-sfcb CIMOM implementation from the sblim project."
  self.readme_file = ['README', ENV['HLANG'], 'rdoc'].compact.join('.')
  self.history_file = ['CHANGELOG', ENV['HLANG'], 'rdoc'].compact.join('.')
  self.extra_rdoc_files = FileList['*.rdoc']
  self.clean_globs = [
    'lib/sfcc/*.{o,so,bundle,a,log,dll}',
  ]
 
  %w{ rake-compiler }.each do |dep|
    self.extra_dev_deps << [dep, '>= 0']
  end
  self.extra_deps << ['shoulda', '>= 0']
  self.extra_deps << ['yard', '>= 0']
  self.spec_extras = { :extensions => ["ext/sfcc/extconf.rb"] }
end


gem 'rake-compiler', '>= 0.4.1'
require 'rake/extensiontask'
Rake::ExtensionTask.new('sfcc')
