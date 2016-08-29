V8 JavaScript Engine
=============

V8 is Google's open source JavaScript engine.

V8 implements ECMAScript as specified in ECMA-262.

V8 is written in C++ and is used in Google Chrome, the open source
browser from Google.

V8 can run standalone, or can be embedded into any C++ application.

V8 Project page: https://github.com/v8/v8/wiki


Getting The Dependencies
=============

Checkout [depot tools](http://www.chromium.org/developers/how-tos/install-depot-tools), and run

After adding depot_tools to System `PATH`, then rn

        python download_deps.py

This will download all dependencies of V8 needed.

Build V8 With GN
============

A **Linux build machine** capable of building V8 for Linux. Other (Mac/Windows) platforms are not supported for Android.

First, generate the necessary build files:
```
gn args out.gn/android
```


```
gn gen out.gn/android --args='is_debug=false target_cpu="arm64" v8_target_cpu="arm64" target_os="android"'
```

Contributing
=============

Please follow the instructions mentioned on the
[V8 wiki](https://github.com/v8/v8/wiki/Contributing).
