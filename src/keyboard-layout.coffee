{Emitter} = require 'event-kit'
{KeyboardLayoutObserver} = require '../build/Release/keyboard-layout-observer.node'

emitter = new Emitter
observer = new KeyboardLayoutObserver -> emitter.emit 'did-change-current-keyboard-layout', getCurrentKeyboardLayout()

getCurrentKeyboardLayout = ->
  observer.getCurrentKeyboardLayout()

getCurrentKeyboardLanguage = ->
  observer.getCurrentKeyboardLanguage()

getInstalledKeyboardLanguages = ->
  # NB: This method returns one language per input method, and users can have
  # >1 layout that matches a given language (i.e. Japanese probably has Hiragana
  # and Katakana, both would correspond to the language "ja"), so we need to
  # dedupe this list.
  languages = {}
  for language in (observer.getInstalledKeyboardLanguages() or [])
    languages[language] = true
  Object.keys(languages)

onDidChangeCurrentKeyboardLayout = (callback) ->
  emitter.on 'did-change-current-keyboard-layout', callback

observeCurrentKeyboardLayout = (callback) ->
  callback(getCurrentKeyboardLayout())
  onDidChangeCurrentKeyboardLayout(callback)

module.exports = {getCurrentKeyboardLayout, getCurrentKeyboardLanguage, getInstalledKeyboardLanguages, onDidChangeCurrentKeyboardLayout, observeCurrentKeyboardLayout}
