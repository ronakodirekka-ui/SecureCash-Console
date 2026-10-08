# SecureCash Console

A console-based banking simulator written in C. The project demonstrates core banking operations such as account creation, secure PIN-based login, balance management, deposits, withdrawals, and account lookup.

## Features

- Create customer accounts
- Unique account-number validation
- PIN-based authentication
- Limited login attempts
- Check account balance
- Deposit funds
- Withdraw funds with balance validation
- Account information lookup
- Persistent local data storage
- Input validation and safer file handling
- Simple menu-driven console interface

## Technology

- C
- Standard C library
- Local binary file storage

## Project Structure

```text
SecureCash-Console/
├── securecash_console.c
├── README.md
├── .gitignore
└── .env.example
```

## Getting Started

### Compile

Using GCC:

```bash
gcc securecash_console.c -o securecash_console
```

### Run

Windows:

```bash
securecash_console.exe
```

Linux/macOS:

```bash
./securecash_console
```

## Data Storage

The application creates a local `accounts.dat` file while running. This file is intentionally ignored by Git because it contains locally generated account data.

Do not use real banking information, passwords, PINs, or personal data with this educational project.

## Educational Purpose

SecureCash Console is a learning project demonstrating C programming concepts including:

- Structures
- Functions
- File handling
- Input validation
- Conditional logic
- Loops
- Menu-driven applications

It is **not a real banking system** and should not be used for financial transactions or production authentication.

## License

Add the license that applies to your project before publishing it publicly.

If this project was originally created by another person, publish it only according to the permission and attribution terms you agreed with them.
