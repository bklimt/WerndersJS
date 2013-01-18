
#include <v8.h>
#include <node.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
 
//using namespace node;
//using namespace v8;

static v8::Handle<v8::Value> foo(const v8::Arguments& args) {
  v8::Handle<v8::Value> v8name;

  CFArrayRef windowList = CGWindowListCopyWindowInfo(
      kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements,
      kCGNullWindowID);
  CFIndex windowListCount = CFArrayGetCount(windowList);
  for (CFIndex i = 0; i < windowListCount; ++i) {
    CFDictionaryRef windowData = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);
    CFStringRef name = (CFStringRef)CFDictionaryGetValue(windowData, kCGWindowOwnerName);

    CFIndex length = CFStringGetLength(name);
    CFIndex bufferSize = length * 2 + 1;
    char *cname = (char *)malloc(bufferSize);
    CFStringGetCString(name, cname, bufferSize, kCFStringEncodingUTF8);

    v8name = v8::String::New(cname);
  }
  CFRelease(windowList);

  return v8name;
}
 
extern "C" void init(v8::Handle<v8::Object> target) {
  NODE_SET_METHOD(target, "foo", foo);
}

