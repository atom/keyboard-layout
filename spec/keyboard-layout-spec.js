'use strict'

const KeyboardLayout = require('../lib/keyboard-layout')

describe('Keyboard Layout', () => {
  describe('.charactersForKeyCode(keyCode)', function () {
    it('returns characters corresponding to the given DOM 3 keyboard event code based on the current keyboard layout', function () {
      const currentLayout = KeyboardLayout.getCurrentKeyboardLayout()
      switch (currentLayout) {
        case 'com.apple.keylayout.Dvorak':
          expect(KeyboardLayout.charactersForKeyCode('KeyS')).toEqual({
            unmodified: 'o',
            withShift: 'O',
            withAltGr: 'ø',
            withShiftAltGr: 'Ø'
          })
          break;
        case 'com.apple.keylayout.US':
          expect(KeyboardLayout.charactersForKeyCode('KeyS')).toEqual({
            unmodified: 's',
            withShift: 'S',
            withAltGr: 'ß',
            withShiftAltGr: 'Í'
          })
          break;
        default:
          throw new Error('No assertion defined for current keyboard layout: ' + currentLayout)
      }
    })
  })

  describe('.getCurrentKeyboardLayout()', () => {
    it('returns the current keyboard layout', () => {
      const layout = KeyboardLayout.getCurrentKeyboardLayout()
      expect(typeof layout).toBe('string')
      expect(layout.length).toBeGreaterThan(0)
    })
  })

  describe('.observeCurrentKeyboardLayout(callback)', () => {
    it('calls back immediately with the current keyboard layout', () => {
      const callback = jasmine.createSpy('observeCurrentKeyboardLayout')
      const disposable = KeyboardLayout.observeCurrentKeyboardLayout(callback)
      disposable.dispose()
      expect(callback.callCount).toBe(1)
      const layout = callback.argsForCall[0][0]
      expect(typeof layout).toBe('string')
      expect(layout.length).toBeGreaterThan(0)
    })
  })

  describe('.getCurrentKeyboardLanguage()', () => {
    it('returns the current keyboard language', () => {
      const language = KeyboardLayout.getCurrentKeyboardLanguage()
      expect(typeof language).toBe('string')
      expect(language.length).toBeGreaterThan(0)
    })
  })

  describe('.getInstalledKeyboardLanguages()', () => {
    it('returns an array of string keyboard languages', () => {
      const languages = KeyboardLayout.getInstalledKeyboardLanguages()
      expect(Array.isArray(languages)).toBe(true)

      if (!(process.platform === 'win32' && process.env.CI)) {
        expect(languages.length).toBeGreaterThan(0)
      }

      return (() => {
        for (const language of languages) {
          expect(typeof language).toBe('string')
          expect(language.length).toBeGreaterThan(0)
        }
      })()
    })
  })
})
