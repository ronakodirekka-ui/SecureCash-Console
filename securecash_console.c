#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "accounts.dat"
#define MAX_NAME 49
#define MAX_PIN 9999
#define MIN_PIN 1000
#define MAX_DEPOSIT 25000
#define MAX_PIN_ATTEMPTS 3

typedef struct {
    int account_number;
    char last_name[30];
    char first_name[50];
    int pin;
    long balance;
} Account;

static void clear_input(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

static void read_string(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static int read_int(const char *prompt) {
    int value;
    for (;;) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1) {
            clear_input();
            return value;
        }
        printf("Please enter a valid number.\n");
        clear_input();
    }
}

static long read_long(const char *prompt) {
    long value;
    for (;;) {
        printf("%s", prompt);
        if (scanf("%ld", &value) == 1) {
            clear_input();
            return value;
        }
        printf("Please enter a valid amount.\n");
        clear_input();
    }
}

static int find_account(int account_number, Account *account, long *position) {
    FILE *file = fopen(DATA_FILE, "rb");
    Account current;
    long index = 0;

    if (file == NULL) {
        return 0;
    }

    while (fread(&current, sizeof(Account), 1, file) == 1) {
        if (current.account_number == account_number) {
            if (account != NULL) *account = current;
            if (position != NULL) *position = index;
            fclose(file);
            return 1;
        }
        index++;
    }

    fclose(file);
    return 0;
}

static int save_account(const Account *account, long position) {
    FILE *file = fopen(DATA_FILE, "rb+");
    if (file == NULL) return 0;

    if (fseek(file, position * (long)sizeof(Account), SEEK_SET) != 0) {
        fclose(file);
        return 0;
    }

    if (fwrite(account, sizeof(Account), 1, file) != 1) {
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

static void register_account(void) {
    Account account;
    FILE *file;

    printf("\n=== Account Registration ===\n");
    account.account_number = read_int("Enter account number: ");

    if (find_account(account.account_number, NULL, NULL)) {
        printf("An account with that number already exists.\n");
        return;
    }

    read_string("Enter first name: ", account.first_name, sizeof(account.first_name));
    read_string("Enter last name: ", account.last_name, sizeof(account.last_name));

    do {
        account.pin = read_int("Create a 4-digit PIN: ");
        if (account.pin < MIN_PIN || account.pin > MAX_PIN) {
            printf("PIN must contain exactly 4 digits.\n");
        }
    } while (account.pin < MIN_PIN || account.pin > MAX_PIN);

    do {
        account.balance = read_long("Enter opening balance: ");
        if (account.balance < 0) {
            printf("Opening balance cannot be negative.\n");
        }
    } while (account.balance < 0);

    file = fopen(DATA_FILE, "ab");
    if (file == NULL) {
        perror("Unable to open account database");
        return;
    }

    if (fwrite(&account, sizeof(Account), 1, file) == 1) {
        printf("Account created successfully.\n");
    } else {
        printf("Could not save the account.\n");
    }

    fclose(file);
}

static int authenticate(Account *account, long *position) {
    int account_number = read_int("Enter account number: ");
    int attempts = 0;

    if (!find_account(account_number, account, position)) {
        printf("Account not found.\n");
        return 0;
    }

    while (attempts < MAX_PIN_ATTEMPTS) {
        int pin = read_int("Enter PIN: ");
        if (pin == account->pin) {
            printf("Welcome, %s %s!\n", account->first_name, account->last_name);
            return 1;
        }
        attempts++;
        printf("Incorrect PIN. Attempts remaining: %d\n", MAX_PIN_ATTEMPTS - attempts);
    }

    printf("Authentication failed.\n");
    return 0;
}

static void withdraw(Account *account, long position) {
    long amount = read_long("Enter amount to withdraw: ");

    if (amount <= 0) {
        printf("Amount must be greater than zero.\n");
        return;
    }
    if (amount > account->balance) {
        printf("Insufficient balance.\n");
        return;
    }

    account->balance -= amount;
    if (save_account(account, position)) {
        printf("Withdrawal successful. Current balance: %ld\n", account->balance);
    } else {
        printf("Could not save the transaction.\n");
    }
}

static void deposit(Account *account, long position) {
    long amount = read_long("Enter amount to deposit: ");

    if (amount <= 0) {
        printf("Amount must be greater than zero.\n");
        return;
    }
    if (amount >= MAX_DEPOSIT) {
        printf("For this simulation, deposits must be below %d.\n", MAX_DEPOSIT);
        return;
    }

    account->balance += amount;
    if (save_account(account, position)) {
        printf("Deposit successful. Current balance: %ld\n", account->balance);
    } else {
        printf("Could not save the transaction.\n");
    }
}

static void change_pin(Account *account, long position) {
    int old_pin = read_int("Enter current PIN: ");
    if (old_pin != account->pin) {
        printf("Incorrect PIN.\n");
        return;
    }

    int new_pin = read_int("Enter new 4-digit PIN: ");
    int confirm_pin = read_int("Confirm new PIN: ");

    if (new_pin < MIN_PIN || new_pin > MAX_PIN) {
        printf("PIN must contain exactly 4 digits.\n");
        return;
    }
    if (new_pin != confirm_pin) {
        printf("PIN confirmation does not match.\n");
        return;
    }
    if (new_pin == account->pin) {
        printf("New PIN must be different from the current PIN.\n");
        return;
    }

    account->pin = new_pin;
    if (save_account(account, position)) {
        printf("PIN changed successfully.\n");
    } else {
        printf("Could not save the new PIN.\n");
    }
}

static void account_menu(Account *account, long position) {
    int choice;

    do {
        printf("\n=== Account Menu ===\n");
        printf("1. Withdraw\n");
        printf("2. Deposit\n");
        printf("3. Change PIN\n");
        printf("4. Check balance\n");
        printf("5. Logout\n");

        choice = read_int("Choose an option: ");

        switch (choice) {
            case 1: withdraw(account, position); break;
            case 2: deposit(account, position); break;
            case 3: change_pin(account, position); break;
            case 4: printf("Current balance: %ld\n", account->balance); break;
            case 5: printf("Logged out successfully.\n"); break;
            default: printf("Please choose a valid option.\n");
        }
    } while (choice != 5);
}

int main(void) {
    int choice;

    printf("============================================================\n");
    printf("                 SecureCash BANKING SIMULATOR\n");
    printf("============================================================\n");
    printf("Console application demonstrating account and transaction management in C.\n");

    for (;;) {
        printf("\n=== Main Menu ===\n");
        printf("1. Login\n");
        printf("2. Register account\n");
        printf("3. Exit\n");

        choice = read_int("Choose an option: ");

        if (choice == 1) {
            Account account;
            long position;
            if (authenticate(&account, &position)) {
                account_menu(&account, position);
            }
        } else if (choice == 2) {
            register_account();
        } else if (choice == 3) {
            printf("Thank you for using the SecureCash Console.\n");
            return 0;
        } else {
            printf("Please choose a valid option.\n");
        }
    }
}
