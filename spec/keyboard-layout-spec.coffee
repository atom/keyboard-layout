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

  describe ".getCurrentKeyboardLanguage()", ->
    it "returns the current keyboard language", ->
      language = KeyboardLayout.getCurrentKeyboardLanguage()
      expect(typeof language).toBe 'string'
      expect(language.length).toBeGreaterThan 0

  describe ".getInstalledKeyboardLanguages()", ->
    it "returns an array of string keyboard languages", ->
      languages = KeyboardLayout.getInstalledKeyboardLanguages()
      expect(Array.isArray(languages)).toBe true
      expect(languages.length).toBeGreaterThan 0

      for language in languages
        expect(typeof language).toBe 'string'
        expect(language.length).toBeGreaterThan 0
