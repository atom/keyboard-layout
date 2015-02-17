{Emitter} = require 'event-kit'
{KeyboardLayoutObserver} = require '../build/Release/keyboard-layout-observer.node'

emitter = new Emitter
observer = new KeyboardLayoutObserver -> emitter.emit 'did-change-current-keyboard-layout', getCurrentKeyboardLayout()

getCurrentKeyboardLayout = ->
  observer.getCurrentKeyboardLayout()

getInstalledKeyboardLayouts = ->
  rawList = observer.getInstalledKeyboardLayouts()

  ret = []
  console.log JSON.stringify(rawList)
  for item in rawList
    continue if ret.indexOf(item) >= 0
    ret.push(item)

  ret

onDidChangeCurrentKeyboardLayout = (callback) ->
  emitter.on 'did-change-current-keyboard-layout', callback

observeCurrentKeyboardLayout = (callback) ->
  callback(getCurrentKeyboardLayout())
  onDidChangeCurrentKeyboardLayout(callback)

module.exports = {getCurrentKeyboardLayout, getInstalledKeyboardLayouts, onDidChangeCurrentKeyboardLayout, observeCurrentKeyboardLayout}
