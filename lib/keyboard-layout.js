'use strict'

const Emitter = require('event-kit').Emitter
const KeyboardLayoutManager = require('../build/Release/keyboard-layout-manager.node')

const emitter = new Emitter()

let currentKeymap = null
let currentKeyboardLayout = null
let currentKeyboardLanguage = null

const manager = new KeyboardLayoutManager(() => {
  currentKeymap = null
  currentKeyboardLayout = null
  currentKeyboardLanguage = null
  emitter.emit('did-change-current-keyboard-layout', getCurrentKeyboardLayout())
})

function getCurrentKeymap () {
  if (!currentKeymap) {
    currentKeymap = manager.getCurrentKeymap()
  }
  return currentKeymap
}

function getCurrentKeyboardLayout () {
  if (!currentKeyboardLayout) {
    currentKeyboardLayout = manager.getCurrentKeyboardLayout()
  }
  return currentKeyboardLayout
}

function getCurrentKeyboardLanguage () {
  if (!currentKeyboardLanguage) {
    currentKeyboardLanguage = manager.getCurrentKeyboardLanguage()
  }
  return currentKeyboardLanguage
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
