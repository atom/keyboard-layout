KeyboardLayout = require '../src/keyboard-layout'

describe "Keyboard Layout", ->
  describe ".getCurrentKeyboardLayout()", ->
    it "returns the current keyboard layout", ->
      layout = KeyboardLayout.getCurrentKeyboardLayout()
      expect(typeof layout).toBe 'string'
      expect(layout.length).toBeGreaterThan 0

  describe ".observeCurrentKeyboardLayout(callback)", ->
    it "calls back immediately with the current keyboard layout", ->
      callback = jasmine.createSpy('observeCurrentKeyboardLayout')
      disposable = KeyboardLayout.observeCurrentKeyboardLayout(callback)
      disposable.dispose()
      expect(callback.callCount).toBe 1

      layout = callback.argsForCall[0][0]
      expect(typeof layout).toBe 'string'
      expect(layout.length).toBeGreaterThan 0
