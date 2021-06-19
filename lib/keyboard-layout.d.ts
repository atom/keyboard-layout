declare module 'keyboard-layout' {
    /**
     * Returns characters for various modifier states based on a DOM 3
     * KeyboardEvent.code value and the current system keyboard layout
     */
    export function getCurrentKeymap(): any;

    /**
     * Get the current keyboard layout
     *
     * @returns The identifier of the current layout based on the value returned
     * by the operating system
     */
    export function getCurrentKeyboardLayout(): string;

    /**
     * Get the current keyboard language
     *
     * @returns The keyboard language
     */
    export function getCurrentKeyboardLanguage(): string;

    /**
     * List all installed keyboard languages
     *
     * @returns All keyboard languages
     */
    export function getInstalledKeyboardLanguages(): string[];
    
    /**
     * Get the current keyboard layout, invoking the callback once immediately
     * with the current layout value and then again next time it changes
     */
    export function observeCurrentKeyboardLayout(callback: Function): any;

    /**
     * Get the current keyboard layout, invoking the callback once the keyboard
     * changes, not changing it for the current set upt
     */
    export function onDidChangeCurrentKeyboardLayout(callback: Function): any;
}
