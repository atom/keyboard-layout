'use strict'

const Emitter = require('event-kit').Emitter
const KeyboardLayoutObserver = require('../build/Release/keyboard-layout-observer.node').KeyboardLayoutObserver

const emitter = new Emitter()

const observer = new KeyboardLayoutObserver(() => {
  emitter.emit('did-change-current-keyboard-layout', getCurrentKeyboardLayout())
})

function getCurrentKeyboardLayout () {
  return observer.getCurrentKeyboardLayout()
}

function getCurrentKeyboardLanguage () {
  return observer.getCurrentKeyboardLanguage()
}

function getInstalledKeyboardLanguages () {
  var languages = {}

  for (var language of (observer.getInstalledKeyboardLanguages() || [])) {
    languages[language] = true
  }

  return Object.keys(languages)
}

function onDidChangeCurrentKeyboardLayout (callback) {
  return emitter.on('did-change-current-keyboard-layout', callback)
}

function observeCurrentKeyboardLayout (callback) {
  callback(getCurrentKeyboardLayout())
  return onDidChangeCurrentKeyboardLayout(callback)
}

module.exports = {
  getCurrentKeyboardLayout: getCurrentKeyboardLayout,
  getCurrentKeyboardLanguage: getCurrentKeyboardLanguage,
  getInstalledKeyboardLanguages: getInstalledKeyboardLanguages,
  onDidChangeCurrentKeyboardLayout: onDidChangeCurrentKeyboardLayout,
  observeCurrentKeyboardLayout: observeCurrentKeyboardLayout
}
