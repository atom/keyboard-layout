'use strict'

const Emitter = require('event-kit').Emitter
const NativeKeymap = require('native-keymap')
const KeyboardLayoutManager = require('../build/Release/keyboard-layout-manager.node')
const KEY_CODES_TO_DOM3_CODES = require('./key-codes-to-dom3-codes')

const emitter = new Emitter()

let charactersByDOM3KeyCode = null

const observer = new KeyboardLayoutManager(() => {
  charactersByDOM3KeyCode = null
  emitter.emit('did-change-current-keyboard-layout', getCurrentKeyboardLayout())
})

function getCurrentKeymap () {
  if (!charactersByDOM3KeyCode) charactersByDOM3KeyCode = buildCharactersByDOM3KeyCode()
  return charactersByDOM3KeyCode
}

function buildCharactersByDOM3KeyCode () {
  let map = {}
  for (let entry of NativeKeymap.getKeyMap()) {
    const dom3Code = KEY_CODES_TO_DOM3_CODES[entry.key_code]
    if (dom3Code) {
      map[dom3Code] = {
        unmodified: entry.value,
        withShift: entry.withShift,
        withAltGr: entry.withAltGr,
        withShiftAltGr: entry.withShiftAltGr
      }
    }
  }
  return map
}

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
  getCurrentKeymap: getCurrentKeymap,
  getCurrentKeyboardLayout: getCurrentKeyboardLayout,
  getCurrentKeyboardLanguage: getCurrentKeyboardLanguage,
  getInstalledKeyboardLanguages: getInstalledKeyboardLanguages,
  onDidChangeCurrentKeyboardLayout: onDidChangeCurrentKeyboardLayout,
  observeCurrentKeyboardLayout: observeCurrentKeyboardLayout
}
