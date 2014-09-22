# keyboard-layout

Read and observe the current keyboard layout on OS X.

To get the current keyboard layout, call `getCurrentKeyboardLayout`. It returns
the string identifier of the current layout based on the value returned by the
operating system.

```coffee
{getCurrentKeyboardLayout} = require 'keyboard-layout'
getCurrentKeyboardLayout() # => "com.apple.keylayout.Dvorak"
```

If you want to watch for layout changes, use `onDidChangeCurrentKeyboardLayout`
or `observeCurrentKeyboardLayout`. They work the same, except
`observeCurrentKeyboardLayout` invokes the given callback immediately with the
current layout value and then again next time it changes, whereas
`onDidChangeCurrentKeyboardLayout` only invokes the callback on the next change.

```coffee
{observeCurrentKeyboardLayout} = require 'keyboard-layout'
subscription = observeCurrentKeyboardLayout (layout) -> console.log layout
subscription.dispose() # to unsubscribe later
```
