$:.unshift File.expand_path(File.dirname($0) + "/../../debug/lib/ruby/use_vfmd")
require 'vfmd'

text = ARGF.read

document = Vfmd::VfmdDocument.new()
document.setContent(text)

htmlRenderer = Vfmd::HtmlRenderer.new()
htmlRenderer.render(document.parseTree())
