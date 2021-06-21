# App_locker

This application locker locks the wallet access for Electra Protocol blockchain GUI wallet. 

- PIN code from 6 to 9 digits.
- Derive a key using the PBKDF2-algorithm as defined in RFC-8018
- Active the privacy mode.
- Disable the wallet menu actions.

### Signals

- `quitAppFromWalletLocker()` is emitted when the application locker is forced to stop.

- `lockingApp(bool)` is emitted when the app is locking or unlocking, to update the application UI.

### requirements

- The application locker recquires QT 5.12 to work.