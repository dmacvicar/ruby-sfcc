require "rake"
require "rake/testtask"

$LOAD_PATH.unshift File.expand_path("../lib", __FILE__)
require "sfcc/version"

task :build do
  system "gem build sfcc.gemspec"
end

task :install => :build do
  system "sudo gem install sfcc-#{Sfcc::VERSION}.gem"
end

Rake::TestTask.new do |t|
  t.libs << File.expand_path('../test', __FILE__)	
  t.libs << File.expand_path('../', __FILE__)
  t.test_files = FileList['test/test*.rb']
  t.verbose = true
end

extra_docs = ['README*', 'CHANGELOG*']

begin
 require 'yard'
  YARD::Rake::YardocTask.new(:doc) do |t|
    t.files   = ['lib/**/*.h', 'lib/**/*.c', 'lib/**/*.rb', *extra_docs]
  end
rescue LoadError
  # WARNING: 'require 'rake/rdoctask'' is deprecated.  Please use 'require 'rdoc/task' (in RDoc 2.4.2+)' instead.
  begin
    require 'rdoc/task' # try Rdoc 2.4.2+ first
  rescue
    require 'rake/rdoctask' # fallback
  end
  STDERR.puts "Install yard if you want prettier docs"
  Rake::RDocTask.new(:doc) do |rdoc|
    if File.exist?("VERSION.yml")
      config = File.read("VERSION")
      version = "#{config[:major]}.#{config[:minor]}.#{config[:patch]}"
    else
      version = ""
    end
    rdoc.rdoc_dir = "doc"
    rdoc.title = "sfcc #{version}"
    extra_docs.each { |ex| rdoc.rdoc_files.include ex }
  end
end

task :default => [:compile, :doc, :test]
gem 'rake-compiler', '>= 0.4.1'
require 'rake/extensiontask'
Rake::ExtensionTask.new('sfcc')

