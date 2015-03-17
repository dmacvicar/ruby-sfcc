# -*- encoding: utf-8 -*-
lib = File.expand_path('../lib/', __FILE__)
$:.unshift lib unless $:.include?(lib)
require "sfcc/version"

Gem::Specification.new do |s|
  s.name        = "sfcc"
  s.version     = Sfcc::VERSION
  s.platform    = Gem::Platform::RUBY
  s.authors     = ["Duncan Mac-Vicar"]
  s.email       = ["dmacvicar@suse.de"]
  s.homepage    = "http://www.github.com/dmacvicar/ruby-sfcc"
  s.summary = "WBEM client for ruby based on the sblim-sfcc library"
  s.description = "ruby-sfcc allows to access a CIMOM either with the WBEM protocol or by using the SfcbLocal interface provided by the sblim-sfcb CIMOM implementation from the sblim project."

  s.required_ruby_version = '>= 1.9.3'
  s.required_rubygems_version = ">= 1.3.6"
  s.add_development_dependency("rake-compiler", [">= 0"])
  s.add_development_dependency("mocha", [">= 0.9"])
  s.add_development_dependency("yard", [">= 0.5"])

  s.extensions	<< 'ext/sfcc/extconf.rb'

  s.files        = Dir.glob("lib/**/*.rb") + Dir.glob("ext/**/*.{rb,h,c}") + %w(CHANGELOG.rdoc README.rdoc)
  s.require_path = 'lib'

  s.post_install_message = <<-POST_INSTALL_MESSAGE
  ____
/@    ~-.
\/ __ .- | remember to have fun! 
 // //  @  

  POST_INSTALL_MESSAGE
end

