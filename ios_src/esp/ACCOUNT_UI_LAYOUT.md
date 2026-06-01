# Account UI Layout & Visual Guide

## Menu Structure

```
┌─────────────────────────────────────────────────────────┐
│ @gigitoogood MLBB Internal iOS              [close] [?] │
├──────────────┬──────────────────────────────────────────┤
│ Home         │ Account Status                           │
│ Visual       ├──────────────────────────────────────────┤
│ Auto Retri   │ State: Guest                  [blue]     │
│ Minimap      │ Account: (guest)                         │
│ Account ◄────┤ Login Attempts: 0                        │
│ Settings     │ Failed Attempts: 0/3                     │
│              │                                          │
│              ├──────────────────────────────────────────┤
│              │ Auto-Reset Settings                      │
│              ├──────────────────────────────────────────┤
│              │ ☑ Enable Auto-Reset on Login Fail    [?] │
│              │ Max Login Attempts: [≡========] 3        │
│              │ Reset Delay (sec): [≡=] 5               │
│              │ ☑ Reset to Guest Mode            [?] │
│              │                                          │
│              ├──────────────────────────────────────────┤
│              │ Quick Actions                            │
│              ├──────────────────────────────────────────┤
│              │ ┌─────────────────────────────────────┐ │
│              │ │  Reset to Guest Now                 │ │
│              │ └─────────────────────────────────────┘ │
│              │ ┌─────────────────────────────────────┐ │
│              │ │  Clear Login Failures               │ │
│              │ └─────────────────────────────────────┘ │
│              │                                          │
└──────────────┴──────────────────────────────────────────┘
```

## Status Colors

### Account State Indicators
```
State: LoggedIn        [GREEN] - Successful authentication
State: Guest           [BLUE]  - Playing without account
State: LoginFailed     [YELLOW/ORANGE] - Most recent attempt failed
State: ResetPending    [RED]   - Scheduled for auto-reset
State: Unknown         [RED]   - Uninitialized state
```

### Error Display Colors
```
Error: (error type)    [RED]
Details: (description) [WHITE]
```

### Counter Colors
```
Failed Attempts: X/Y   [YELLOW] - Approaching threshold (yellow when < max)
Failed Attempts: 3/3   [RED]    - At or exceeding threshold (red at max)
```

## UI States

### Initial State (Logged Out)
```
State: Guest
Account: (guest)
Login Attempts: 0
Failed Attempts: 0/3
```

### After Failed Login
```
State: LoginFailed            [ORANGE]
Account: (guest)
Login Attempts: 1
Failed Attempts: 1/3          [YELLOW]
Error: Invalid Credentials
Details: Invalid username or password
```

### After Multiple Failures (2/3)
```
State: LoginFailed            [ORANGE]
Account: (guest)
Login Attempts: 3
Failed Attempts: 2/3          [YELLOW]
Error: Network Error
Details: Could not reach login server
```

### At Threshold (3/3)
```
State: ResetPending           [RED]
Account: (guest)
Login Attempts: 5
Failed Attempts: 3/3          [RED]
Error: Server Error
Details: Service temporarily unavailable
[!] AUTO-RESET PENDING [!]
```

### After Reset
```
State: Guest                  [BLUE]
Account: (guest)
Login Attempts: 5
Failed Attempts: 0/3
```

## UI Interaction Flow

```
┌─────────────────────┐
│   Main Menu Open    │
└──────────┬──────────┘
           │
           ├─→ Click "Account" Tab
           │
           ├─→ View Status Panel
           │   ├─ See current state
           │   ├─ Check login attempts
           │   └─ View errors (if any)
           │
           ├─→ Adjust Settings
           │   ├─ Enable/Disable auto-reset
           │   ├─ Set max attempts
           │   ├─ Set reset delay
           │   └─ Choose reset mode
           │
           ├─→ Manual Actions
           │   ├─ Click "Reset to Guest Now"
           │   │  └─ Immediate reset triggered
           │   │
           │   └─ Click "Clear Login Failures"
           │      └─ Counters reset to 0
           │
           └─→ Settings auto-sync to AccountManager
```

## Feature Tooltips

```
Enable Auto-Reset on Login Fail [?]
  → "Automatically resets account to guest after failed login attempts"

Max Login Attempts [?] (not shown, slider only)
  → Range: 1-10 (default: 3)
  → Number of failed attempts before auto-reset triggers

Reset Delay (seconds) [?] (not shown, slider only)
  → Range: 0-30 (default: 5)
  → Seconds to wait before executing reset

Reset to Guest Mode [?]
  → "When true: reset to guest. When false: logout completely"
```

## Button States

### "Reset to Guest Now" Button
```
[Normal State]
┌─────────────────────────────────────┐
│  Reset to Guest Now                 │
└─────────────────────────────────────┘

[Hover State]
┌─────────────────────────────────────┐
│  Reset to Guest Now                 │  [Brighter]
└─────────────────────────────────────┘

[Click]
→ Triggers AccountManager::AutoResetAccount()
→ Logs: [Account] Manual reset triggered from UI
→ UI updates to show reset progress
```

### "Clear Login Failures" Button
```
[Normal State]
┌─────────────────────────────────────┐
│  Clear Login Failures               │
└─────────────────────────────────────┘

[Click]
→ Triggers AccountManager::ClearLoginFailures()
→ Logs: [Account] Login failures cleared from UI
→ UI counters reset to 0
→ Error messages cleared
```

## Accessibility Features

✓ **Color Coding** - Status visible at a glance
✓ **Tooltips** - Hover for help on settings
✓ **Clear Labels** - Descriptive text for all fields
✓ **Real-time Updates** - Status changes immediately
✓ **Numeric Feedback** - Counters show exact values
✓ **Large Buttons** - Easy to tap on mobile

## Theme Integration

- **Background**: AMOLED Black (#000000)
- **Panels**: Dark Gray (#030303)
- **Accent Color**: Deep Red (#E63434) - used for buttons, errors, titles
- **Text Primary**: Light Gray (#F2F2F2)
- **Text Secondary**: Medium Gray (#B3B3B3)
- **Borders**: Medium Gray (#333333)

## Mobile Optimization

- Panel scrollable if content overflows
- Touch-friendly button sizes (40px+ height)
- Landscape/Portrait responsive
- Safe area padding for notch/Dynamic Island
- No small text that requires zooming

## Future Enhancements

- [ ] Account name input field
- [ ] Login history log
- [ ] Auto-retry countdown timer
- [ ] Account migration features
- [ ] Multi-account support
- [ ] Detailed error troubleshooting
- [ ] Settings persistence to config file
