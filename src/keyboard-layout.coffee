{Emitter} = require 'event-kit'
{KeyboardLayoutObserver} = require '../build/Release/keyboard-layout-observer.node'

emitter = new Emitter
observer = new KeyboardLayoutObserver -> emitter.emit 'did-change-current-keyboard-layout', getCurrentKeyboardLayout()

getCurrentKeyboardLayout = ->
  observer.getCurrentKeyboardLayout()

onDidChangeCurrentKeyboardLayout = (callback) ->
  emitter.on 'did-change-current-keyboard-layout', callback

observeCurrentKeyboardLayout = (callback) ->
  callback(getCurrentKeyboardLayout())
  onDidChangeCurrentKeyboardLayout(callback)

module.exports = {getCurrentKeyboardLayout, onDidChangeCurrentKeyboardLayout, observeCurrentKeyboardLayout}
