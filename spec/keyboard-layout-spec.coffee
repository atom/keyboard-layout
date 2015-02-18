KeyboardLayout = require '../src/keyboard-layout'

describe "Keyboard Layout", ->
  describe ".getCurrentKeyboardLayout()", ->
    it "returns the current keyboard layout", ->
      layout = KeyboardLayout.getCurrentKeyboardLayout()
      expect(typeof layout).toBe 'string'
      expect(layout.length).toBeGreaterThan 0
