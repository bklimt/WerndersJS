
#include <v8.h>
#include <node.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
 
v8::Handle<v8::Value> V8StringFromCFString(CFStringRef cfString) {
  if (!cfString) {
    return v8::Null();
  }
  CFIndex length = CFStringGetLength(cfString);
  // This is the most number of UTF-8 bytes a string that length could be.
  CFIndex bufferSize = length * 6 + 1;
  char *cString = (char *)malloc(bufferSize);
  CFStringGetCString(cfString, cString, bufferSize, kCFStringEncodingUTF8);
  v8::Handle<v8::String> v8String = v8::String::New(cString);
  free(cString);
  return v8String;
}

v8::Handle<v8::Value> V8NumberFromCFNumber(CFNumberRef cfNumber) {
  if (!cfNumber) {
    return v8::Null();
  }
  double value = 0.0;
  if (!CFNumberGetValue(cfNumber, kCFNumberDoubleType, &value)) {
    return v8::Null();
  }
  return v8::NumberObject::New(value);
}

v8::Handle<v8::Value> V8ObjectFromCGRect(CGRect cgRect) {
  v8::Handle<v8::Object> v8Rect = v8::Object::New();
  v8Rect->Set(v8::String::New("x"), v8::NumberObject::New(CGRectGetMinX(cgRect)));
  v8Rect->Set(v8::String::New("y"), v8::NumberObject::New(CGRectGetMinY(cgRect)));
  v8Rect->Set(v8::String::New("width"), v8::NumberObject::New(CGRectGetWidth(cgRect)));
  v8Rect->Set(v8::String::New("height"), v8::NumberObject::New(CGRectGetHeight(cgRect)));
  return v8Rect;
}

static v8::Handle<v8::Value> GetAllWindows(const v8::Arguments& args) {
  v8::Handle<v8::Array> result = v8::Array::New();

  CFArrayRef windowList = CGWindowListCopyWindowInfo(
      kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements,
      kCGNullWindowID);
  CFIndex windowListCount = CFArrayGetCount(windowList);
  for (CFIndex i = 0; i < windowListCount; ++i) {
    CFDictionaryRef windowData = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);
    v8::Handle<v8::Object> object = v8::Object::New();

#define COPY_STRING(name, constant) { \
      CFStringRef temp = (CFStringRef)CFDictionaryGetValue(windowData, constant); \
      if (temp) { \
        object->Set(v8::String::New(name), V8StringFromCFString(temp)); \
      } \
    }
    COPY_STRING("owner", kCGWindowOwnerName)
    COPY_STRING("name", kCGWindowName)
#undef COPY_STRING

#define COPY_NUMBER(name, constant) { \
      CFNumberRef temp = (CFNumberRef)CFDictionaryGetValue(windowData, constant); \
      if (temp) { \
        object->Set(v8::String::New(name), V8NumberFromCFNumber(temp)); \
      } \
    }
    COPY_NUMBER("number", kCGWindowNumber);
#undef COPY_NUMBER

    {
      CFDictionaryRef temp = (CFDictionaryRef)CFDictionaryGetValue(windowData, kCGWindowBounds);
      CGRect cgRect;
      CGRectMakeWithDictionaryRepresentation(temp, &cgRect);
      object->Set(v8::String::New("bounds"), V8ObjectFromCGRect(cgRect));
    }

    result->Set(v8::NumberObject::New(i), object);
  }
  CFRelease(windowList);

  return result;
}
 
extern "C" void init(v8::Handle<v8::Object> target) {
  NODE_SET_METHOD(target, "GetAllWindows", GetAllWindows);
}

