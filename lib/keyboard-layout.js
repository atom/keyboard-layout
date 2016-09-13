'use strict'

const Emitter = require('event-kit').Emitter
const KeyboardLayoutManager = require('../build/Release/keyboard-layout-manager.node')

const emitter = new Emitter()

let currentKeymap = null

const manager = new KeyboardLayoutManager(() => {
  currentKeymap = null
  emitter.emit('did-change-current-keyboard-layout', getCurrentKeyboardLayout())
})

function getCurrentKeymap () {
  if (!currentKeymap) {
    currentKeymap = manager.getCurrentKeymap()
  }
  return currentKeymap
}

function getCurrentKeyboardLayout () {
  return manager.getCurrentKeyboardLayout()
}

function getCurrentKeyboardLanguage () {
  return manager.getCurrentKeyboardLanguage()
}

function getInstalledKeyboardLanguages () {
  var languages = {}

  for (var language of (manager.getInstalledKeyboardLanguages() || [])) {
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
  getCurrentKeymap: getCurrentKeymap,
  getCurrentKeyboardLayout: getCurrentKeyboardLayout,
  getCurrentKeyboardLanguage: getCurrentKeyboardLanguage,
  getInstalledKeyboardLanguages: getInstalledKeyboardLanguages,
  onDidChangeCurrentKeyboardLayout: onDidChangeCurrentKeyboardLayout,
  observeCurrentKeyboardLayout: observeCurrentKeyboardLayout
}
