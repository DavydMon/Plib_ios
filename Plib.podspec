#
# Be sure to run `pod lib lint Plib.podspec' to ensure this is a
# valid spec and remove all comments before submitting the spec.
#
# Any lines starting with a # are optional, but encouraged
#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html
#

Pod::Spec.new do |s|
  s.name             = "Plib"
  s.version          = "0.2.0"
  s.summary          = "Messenger iOS application backend"
  s.description      = <<-DESC
                       Lib for connecting to messenger server
                       DESC
  s.homepage         = "https://github.com/DavydMon"
  s.license          = 'MIT'
  s.author           = { "David" => "DavydMon@gmail.com" }
  s.source           = { :git => "https://github.com/DavydMon/plib_ios.git" }
  s.social_media_url = 'DavydMon@gmail.com'

  s.platform     = :ios, '9.0'

  s.source_files = 'd-mon/*{c,h,m,cpp}', 'def/*{c,h,m,cpp}', 'plib/**/*{c,h,m,cpp}', 'sip/**/*{c,h,m,cpp}'
  s.preserve_paths = 'd-mon/*{c,h,m,cpp}', 'def/*{c,h,m,cpp}', 'plib/**/*{c,h,m,cpp}', 'sip/**/*{c,h,m,cpp}'

  s.xcconfig = {
    'GCC_PREPROCESSOR_DEFINITIONS' => '$(inherited) LINUX=1 IOS=1',
    'HEADER_SEARCH_PATHS'  => '$(inherited) $(PODS_ROOT)/Plib $(PODS_ROOT)/Plib/plib $(PODS_ROOT)/Plib/def $(PODS_ROOT)/Plib/d-mon $(PODS_ROOT)/Plib/sip'
  }

end
