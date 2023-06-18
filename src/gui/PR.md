This PR fixes #13691.

Before this PR, TouchScreenGUI simply emitted a keypress event for one of the number keys when a hotbar item was pressed. After the 9th hotbar item (the last one for which there is a number key), it started pressing "random" keys.

After this PR, all hotbar items are selectable (except if they are hidden behind other controls, but that's a different issue). 

This PR also fixes TouchScreenGUI compilation on (my) desktop after #13640. The compiler errors were:

```
/usr/include/c++/12/bits/stl_vector.h: In instantiation of ‘class std::vector<const std::__cxx11::basic_string<char> >’:
/mt/src/gui/touchscreengui.h:93:33:   required from here
/usr/include/c++/12/bits/stl_vector.h:435:66: error: static assertion failed: std::vector must have a non-const, non-volatile value_type
  435 |       static_assert(is_same<typename remove_cv<_Tp>::type, _Tp>::value,
      |                                                                  ^~~~~
/usr/include/c++/12/bits/stl_vector.h:435:66: note: ‘std::integral_constant<bool, false>::value’ evaluates to false
```

```
In file included from /mt/src/gui/touchscreengui.cpp:21:
/mt/src/gui/touchscreengui.h:139:26: error: ‘shared_ptr’ is not a member of ‘std’
  139 |         std::vector<std::shared_ptr<button_info>> m_buttons;
      |                          ^~~~~~~~~~
/mt/src/gui/touchscreengui.h:32:1: note: ‘std::shared_ptr’ is defined in header ‘<memory>’; did you forget to ‘#include <memory>’?
   31 | #include "client/game.h"
  +++ |+#include <memory>
   32 | 
```

## To do

This PR is a Ready for Review.

## How to test

Install the CI-built APK on your Android device. Start a Devtest world and execute `/hotbar 20`. Verify that you can select all hotbar slots. Verify that you *cannot* use the 19th hotbar slot to change your camera mode.