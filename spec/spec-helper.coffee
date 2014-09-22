require 'coffee-cache'
KeyboardLayout = require '../src/keyboard-layout'

describe "KeyboardLayout", ->
  describe "getCurrentKeyboardLayout()", ->
    it "returns the current keyboard layout", ->
      KeyboardLayout.onDidChangeCurrentKeyboardLayout (layout) -> console.log "CHANGED!!", layout
      waits 10000
