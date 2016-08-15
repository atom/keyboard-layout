(function() {
  var Emitter, KeyboardLayoutObserver, emitter, getCurrentKeyboardLanguage, getCurrentKeyboardLayout, getInstalledKeyboardLanguages, observeCurrentKeyboardLayout, observer, onDidChangeCurrentKeyboardLayout;

  Emitter = require('event-kit').Emitter;

  KeyboardLayoutObserver = require('../build/Release/keyboard-layout-observer.node').KeyboardLayoutObserver;

  emitter = new Emitter;

  observer = new KeyboardLayoutObserver(function() {
    return emitter.emit('did-change-current-keyboard-layout', getCurrentKeyboardLayout());
  });

  getCurrentKeyboardLayout = function() {
    return observer.getCurrentKeyboardLayout();
  };

  getCurrentKeyboardLanguage = function() {
    return observer.getCurrentKeyboardLanguage();
  };

  getInstalledKeyboardLanguages = function() {
    var language, languages, _i, _len, _ref;
    languages = {};
    _ref = observer.getInstalledKeyboardLanguages();
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      language = _ref[_i];
      languages[language] = true;
    }
    return Object.keys(languages);
  };

  onDidChangeCurrentKeyboardLayout = function(callback) {
    return emitter.on('did-change-current-keyboard-layout', callback);
  };

  observeCurrentKeyboardLayout = function(callback) {
    callback(getCurrentKeyboardLayout());
    return onDidChangeCurrentKeyboardLayout(callback);
  };

  module.exports = {
    getCurrentKeyboardLayout: getCurrentKeyboardLayout,
    getCurrentKeyboardLanguage: getCurrentKeyboardLanguage,
    getInstalledKeyboardLanguages: getInstalledKeyboardLanguages,
    onDidChangeCurrentKeyboardLayout: onDidChangeCurrentKeyboardLayout,
    observeCurrentKeyboardLayout: observeCurrentKeyboardLayout
  };

}).call(this);
