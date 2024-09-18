#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <set>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <regex>
#include <codecvt>
#include <conio.h>
#include <thread>

#define MAX 100
using namespace std;

typedef struct ATMAccount{
    int accountNumber;
    int pinCode;
    double balance;
    string accountName;
    string birthday;
    string contactNumber;
} Account;

typedef struct ATMList{
    Account Accounts[MAX];
    int last;
} List;

class ATM {
private:
    List Last;
    set<int> existingAccountNumbers;
    int currentLoggedInAccountNumber;

    bool IsFull(){
        return Last.last == MAX - 1;
    }

    int LocatePosition(int accountNumber){
        for (int i = 0; i <= Last.last; i++) {
            if (Last.Accounts[i].accountNumber == accountNumber) {
                return i;
            }
        }
        return -1;
    }

    void SaveAccountInfoToFile() {
        ofstream outputFile("Accounts.txt");

        if (!outputFile.is_open()) {
            DisplayErrorMessage("Lost Connection");
            return;
        }
        for (int i = 0; i <= Last.last; i++) {
            outputFile << Last.Accounts[i].accountNumber << "\n"
                       << setw(6) << setfill('0') << Last.Accounts[i].pinCode << "\n"
                       << Last.Accounts[i].balance << "\n"
                       << Last.Accounts[i].accountName << "\n"
                       << Last.Accounts[i].birthday << "\n"
                       << Last.Accounts[i].contactNumber << endl;
        }
        outputFile.close();
        Displaytop("Account data saved successfully.");
    }

    void SaveAccountInfoToDrive(int accountNumber, int newPinCode) {
        wchar_t drives[256];
        DWORD drivesSize;
        bool cardDetected = false;
        int timeout = 60;

        while (!cardDetected && timeout > 0) {
            drivesSize = GetLogicalDriveStringsW(256, drives);

            for (wchar_t* drive = drives; *drive; drive += 4) {
                UINT driveType = GetDriveTypeW(drive);
                if (driveType == DRIVE_REMOVABLE) {
                    wstring driveLetterWStr = drive;
                    string driveLetter = wstring_convert<codecvt_utf8<wchar_t>>().to_bytes(driveLetterWStr);
                    string atmAccountFilePath = driveLetter + "ATMAccount.txt";
                    ofstream outputFile(atmAccountFilePath);

                    if (outputFile.is_open()) {
                        outputFile << accountNumber << "\n"
                                   << setw(6) << setfill('0') << newPinCode << "\n";

                        outputFile.close();
                        cout << "PIN code updated on " << driveLetter << " Card" << endl;
                    } else {
                        DisplayErrorMessage("Lost Connection");
                    }
                    cardDetected = true;
                    break;
                }
            }
            if (!cardDetected) {
                Displaytop("Please Insert Card", 1000);
                timeout--;
            }
        }
        if (!cardDetected) {
            Displaytop("Card insertion timeout. Unable to update PIN code.", 2000);
        }
    }

    bool DeleteAccountFromDrive(int accountNumber) {
        wchar_t drives[256];
        DWORD drivesSize;
        bool cardDetected = false;

        while (!cardDetected) {
            drivesSize = GetLogicalDriveStringsW(256, drives);

            for (wchar_t* drive = drives; *drive; drive += 4) {
                UINT driveType = GetDriveTypeW(drive);
                if (driveType == DRIVE_REMOVABLE) {
                    string driveLetter = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(drive);
                    string atmAccountFilePath = driveLetter + "ATMAccount.txt";

                    if (remove(atmAccountFilePath.c_str()) == 0) {
                        return true;
                    } else {
                        DisplayErrorMessage("Unable to Delete Card Data");
                        return false;
                    }
                }
            }
            if (!cardDetected) {
                Displaytop("Please Insert Card"), 1000;
            }
        }
        return false;
    }

    int AccountNumberGenerator() {
        int accountNumber;
        do {
            accountNumber = rand() % 90000 + 10000;
        } while (existingAccountNumbers.count(accountNumber) > 0);
        existingAccountNumbers.insert(accountNumber);
        return accountNumber;
    }

    bool ValidPinChecker(const string& pin) {
        return (pin.length() == 6 && regex_match(pin, regex("^[0-9]{6}$")));
    }

    bool DriveFileChecker(const string& drive) {
        ifstream file(drive + "ATMAccount.txt");
        return file.good();
    }

    void CardLogout() {
        Displaytop("Please Remove your Card!");

        wchar_t drives[256];
        DWORD drivesSize;

        while (true) {
            drivesSize = GetLogicalDriveStringsW(256, drives);
            bool cardDetected = false;

            for (wchar_t* drive = drives; *drive; drive += 4) {
                UINT driveType = GetDriveTypeW(drive);
                if (driveType == DRIVE_REMOVABLE) {
                    cardDetected = true;
                    break;
                }
            }
            if (!cardDetected) {
                Displaytop("Card has been Removed", 1000);
                currentLoggedInAccountNumber = -1;
                LoginMenu();
                return;
            }
            Sleep(1000);
        }
    }

    int EnterPIN() {
        string enteredPinStr;
        for (int i = 0; i < 6; i++) {
            char ch = _getch();
            enteredPinStr += ch;
            cout << "*";
        }
        int enteredPin = stoi(enteredPinStr);
        return enteredPin;
    }

    void ClearScreen() {
        system("cls");
    }

    void DisplayErrorMessage(const string& message, int delay = 1000) {
        ClearScreen();
        int totalWidth = message.length() + 4;
        string border(totalWidth, '*');
        system("cls");
        cout << "\n\n\n" << endl;
        cout << "\t\t\t\t\t" << border << endl;
        cout << "\t\t\t\t\t" <<"* " << message << " *" << endl;
        cout << "\t\t\t\t\t" <<border << endl;


        if (delay > 0) {
            Sleep(delay);
        }
    }

    void DisplayMessage(const string& message, int delay = 1000) {
        ClearScreen();
        cout << message << endl;
        if (delay > 0) {
            Sleep(delay);
        }
    }

    void DisplayMessageNoNewLine(const string& message) {
        ClearScreen();
        cout << message;
    }

    void displayATMArt(){
        cout << "\t\t\t************************************************************************" << endl;
        cout << "\t\t\t************************************************************************" << endl;
        cout << "\t\t\t**                                                                    **" << endl;
        cout << "\t\t\t**          *****        ******************  ******          ******   **" << endl;
        cout << "\t\t\t**         *** ***       ******************  ******          ******   **" << endl;
        cout << "\t\t\t**        ***   ***             ***          ***  ***      ***  ***   **" << endl;
        cout << "\t\t\t**       ***     ***            ***          ***  ***      ***  ***   **" << endl;
        cout << "\t\t\t**      *************           ***          ***    ***  ***    ***   **" << endl;
        cout << "\t\t\t**     ***         ***          ***          ***    ***  ***    ***   **" << endl;
        cout << "\t\t\t**    ***           ***         ***          ***      ***       ***   **" << endl;
        cout << "\t\t\t**   ***             ***        ***          ***      ***       ***   **" << endl;
        cout << "\t\t\t**                                                                    **" << endl;
        cout << "\t\t\t**                                                                    **" << endl;
        cout << "\t\t\t************************************************************************" << endl;
        cout << "\t\t\t************************************************************************" << endl;
    }

  void LoadingAni() {
    int delay = 50;
    int iterations = 15;

    cout << "\n\n\n\t\t\t\t\tProcessing: 8";
    for (int i = 0; i < iterations; i++) {
        cout << "=";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(delay));
    }
    cout << "D" << endl;
}

    void SetColorBlue() {
        system("color 17");
    }

    void space(){
    cout << "\n\n\n";
    }

    void Displaytop(const string& message, int delay = 1000){
        ClearScreen();
        int totalWidth = message.length() + 4;
        string border(totalWidth, '*');
        system("cls");
        cout << "\n\n\n" << endl;
        cout << "\t\t\t\t\t" << border << endl;
        cout << "\t\t\t\t\t" <<"* " << message << " *" << endl;
        cout << "\t\t\t\t\t" <<border << endl;
            if (delay > 0) {
                Sleep(delay);
            }
    }

    int MainCardDetector() {
        wchar_t drives[256];
        DWORD drivesSize;
        bool cardDetected = false;
        int timeout = 60;

        while (!cardDetected && timeout > 0) {
            drivesSize = GetLogicalDriveStringsW(256, drives);

            for (wchar_t* drive = drives; *drive; drive += 4) {
                UINT driveType = GetDriveTypeW(drive);
                if (driveType == DRIVE_REMOVABLE) {
                    system("cls");
                    cout << "Card Detected: " << drive << endl;

                    string driveLetter = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(drive);

                    ifstream atmAccountFile(driveLetter + "ATMAccount.txt");
                    if (atmAccountFile) {
                        string atmAccountNumberStr;
                        getline(atmAccountFile, atmAccountNumberStr);

                        int atmAccountNumber = stoi(atmAccountNumberStr);

                        atmAccountFile.close();
                        Displaytop("Account");
                        cout << "\t\t\t\tAccount Number: " << atmAccountNumber << endl;
                        currentLoggedInAccountNumber = atmAccountNumber;

                        Sleep(2000);
                        return 1;
                    } else {
                        DisplayErrorMessage("Card not Registered", 2000);
                        CardLogout();
                    }
                }
            }
            if (!cardDetected) {
                DisplayMessage("Please Insert Card", 1000);
                timeout--;
            }
        }
        return 0;
    }

    bool NewAccountCardDetector() {
        wchar_t drives[256];
        DWORD drivesSize;
        bool cardDetected = false;
        int timeout = 60;

        while (!cardDetected && timeout > 0) {
            drivesSize = GetLogicalDriveStringsW(256, drives);

            for (wchar_t* drive = drives; *drive; drive += 4) {
                UINT driveType = GetDriveTypeW(drive);
                if (driveType == DRIVE_REMOVABLE) {
                    cout << "Card Detected: " << drive << endl;

                    if (DriveFileChecker(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(drive))) {
                        Displaytop("Card Already Registered", 2000);
                        CardLogout();
                    }
                    cardDetected = true;
                    break;
                }
            }
            if (!cardDetected) {
                Displaytop("Please Insert Clean Card", 1000);
                timeout--;
            }
        }
        return cardDetected;
    }

public:
    ATM() {
        currentLoggedInAccountNumber = -1;
    }

    void MakeNull(){
        Last.last = -1;
    }

    void RetrieveAccounts() {
        ifstream fileptr("Accounts.txt");

        if (!fileptr.is_open()) {
            DisplayErrorMessage("Error opening file.");
            return;
        }

        Account account;
        string line;

        while (getline(fileptr, line)) {
            int accountNumber = stoi(line);

            getline(fileptr, line);
            int pinCode = stoi(line);

            getline(fileptr, line);
            double balance = stod(line);

            getline(fileptr, account.accountName);
            getline(fileptr, account.birthday);
            getline(fileptr, account.contactNumber);

            account.accountNumber = accountNumber;
            account.pinCode = pinCode;
            account.balance = balance;

            if (!IsFull()) {
                Last.Accounts[++Last.last] = account;
            } else {
                DisplayErrorMessage("No Accounts Yet");
                break;
            }
        }
        fileptr.close();
        SetColorBlue();
        Displaytop("ATM is Online");
        cout << endl;
        LoadingAni();
    }

    void CreateAccount() {
        srand(static_cast<unsigned>(time(nullptr)));

        bool cardInserted = NewAccountCardDetector();
        if (!cardInserted) {
            DisplayMessage("Card Timeout", 1000);
            DisplayMessage("Returning to Login Menu", 1000);
            return;
        }
        if (DriveFileChecker("")) {
            DisplayErrorMessage("Card already Registered", 2000);
            LoginMenu();
            return;
        }
        Account newAccount;
        newAccount.accountNumber = AccountNumberGenerator();

        cin.ignore();
        Displaytop("REGISTRATION");
        cout << "\t\t\tEnter Account Holder's Full Name: ";
        getline(cin, newAccount.accountName);

        cout << "\t\t\tEnter Birthday (MM/DD/YYYY): ";
        getline(cin, newAccount.birthday);

        cout << "\t\t\tEnter Contact Number: +63 ";
        getline(cin, newAccount.contactNumber);

        cout << "\t\t\tEnter Initial Deposit (Min. 5000): ";
        cin >> newAccount.balance;
        string pin;
        do {
            cout << "\t\t\tEnter 6 PIN code: ";
            cin >> pin;
        } while (!ValidPinChecker(pin));

        newAccount.pinCode = stoi(pin);

        if (!IsFull()) {
            Last.Accounts[++Last.last] = newAccount;
            SaveAccountInfoToFile();
            SaveAccountInfoToDrive(newAccount.accountNumber, newAccount.pinCode);
            Displaytop("Account Successfully Created", 1000);
            Displaytop("Card is now Ready to Use");
            LoadingAni();
            LoginMenu();
        } else {
            DisplayErrorMessage("System Timeout");
        }
    }

    void Withdraw(){
        if (currentLoggedInAccountNumber == -1) {
            Displaytop("You are not logged in.");
            LoginMenu();
        }
        Displaytop("Withdrawal");
        cout << "\t\t\t\tEnter your 6 PIN: ";
        int enteredPin = EnterPIN();
        int accountIndex = LocatePosition(currentLoggedInAccountNumber);

        if (accountIndex == -1 || enteredPin != Last.Accounts[accountIndex].pinCode) {
            DisplayErrorMessage("Invalid PIN.");
            return;
        }
        double currentBalance = Last.Accounts[accountIndex].balance;
        Displaytop("Your current balance: $" + to_string(currentBalance));

        double amount;
        do {
            cout << "\t\t\t\t\tEnter the amount to withdraw: $";
            cin >> amount;
            if (amount <= 0) {
                DisplayErrorMessage("Invalid amount. Please enter a positive value.");
            } else if (amount > currentBalance) {
                DisplayErrorMessage("Insufficient funds.");
            }
        } while (amount <= 0 || amount > currentBalance);
        Displaytop("Withdrawal");
        cout << "\t\t\tEnter your PIN again to confirm the transaction: ";
        int confirmPin = EnterPIN();

        if (confirmPin != enteredPin) {
            DisplayErrorMessage("PIN confirmation failed", 1000);
            DisplayErrorMessage("Transaction canceled.", 2000);
            return;
        }
        LoadingAni();
        Last.Accounts[accountIndex].balance -= amount;
        SaveAccountInfoToFile();
        Displaytop("Withdrawal successful.", 2000);
        Displaytop("Your new balance: $" + to_string(Last.Accounts[accountIndex].balance));
        system("pause");
    }

    void Deposit(){
        if (currentLoggedInAccountNumber == -1) {
            Displaytop("You are not logged in.");
            LoginMenu();
        }
        Displaytop("Deposit");
        cout << "\t\t\t\tEnter your 6 PIN: ";
        int enteredPin = EnterPIN();
        int accountIndex = LocatePosition(currentLoggedInAccountNumber);

        if (accountIndex == -1 || enteredPin != Last.Accounts[accountIndex].pinCode) {
            DisplayErrorMessage("Invalid PIN.");
            return;
        }
        double currentBalance = Last.Accounts[accountIndex].balance;

        Displaytop("Your current balance: $" + to_string(currentBalance));

        double amount;
        do {
            cout << "\t\t\t\t\tEnter the amount to deposit: $";
            cin >> amount;
            if (amount <= 0) {
                DisplayErrorMessage("Invalid amount. Please enter a positive value.");
            }
        } while (amount <= 0);
        Displaytop("Deposit");
        cout <<"\t\t\tEnter your PIN again to confirm the transaction: ";
        int confirmPin = EnterPIN();

        if (confirmPin != enteredPin) {
            DisplayErrorMessage("PIN confirmation failed", 1000);
            DisplayErrorMessage("Transaction canceled.", 2000);
            return;
        }
        LoadingAni();
        Last.Accounts[accountIndex].balance += amount;
        SaveAccountInfoToFile();
        Displaytop("Deposit successful.", 2000);
        Displaytop("Your new balance: $" + to_string(Last.Accounts[accountIndex].balance));
        system("pause");
    }

    void BalanceInquiry() {
        if (currentLoggedInAccountNumber == -1) {
            DisplayMessage("You are not logged in.");
            LoginMenu();
        }
        Displaytop("Balance Inquiry");
        cout << "\t\t\t\t\tEnter your 6 PIN: ";
        int enteredPin = EnterPIN();
        int accountIndex = LocatePosition(currentLoggedInAccountNumber);
        if (accountIndex == -1 || enteredPin != Last.Accounts[accountIndex].pinCode) {
            DisplayErrorMessage("Invalid PIN.");
            return;
        }
        Displaytop("Your account balance: $" + to_string(Last.Accounts[accountIndex].balance));
        system("Pause");
    }

    void DeleteAccount() {
        if (currentLoggedInAccountNumber == -1) {
            Displaytop("You are not logged in.");
            LoginMenu();
        }
        Displaytop("Deletion of Account");
        cout << "\t\t\t\t\tEnter your 6 PIN: ";
        int enteredPin = EnterPIN();
        int accountIndex = LocatePosition(currentLoggedInAccountNumber);

        if (accountIndex == -1 || enteredPin != Last.Accounts[accountIndex].pinCode) {
            DisplayErrorMessage("Invalid PIN.");
            AccountMenu();
        }
        Displaytop("Deletion of Account");
        cout << "Are you sure you want to delete your account? (Y/N): ";
        char confirmation;
        cin >> confirmation;

        if (confirmation == 'Y' || confirmation == 'y') {
            bool driveDeletionSuccess = DeleteAccountFromDrive(currentLoggedInAccountNumber);

            if (driveDeletionSuccess) {
                for (int i = accountIndex; i < Last.last; i++) {
                    Last.Accounts[i] = Last.Accounts[i + 1];
                }
                LoadingAni();
                Last.last--;
                SaveAccountInfoToFile();
                Displaytop("Account deleted successfully.", 1000);
                CardLogout();
            } else {
                DisplayErrorMessage("Failed to delete Account Data from Card.");
            }
        } else {
            Displaytop("Account deletion canceled.");
        }
    }

    void ChangePin() {
        if (currentLoggedInAccountNumber == -1) {
            Displaytop("You are not logged in.");
            LoginMenu();
        }
        Displaytop("Change Pin");
        cout << "\t\t\t\tEnter your CURRENT 6-digit PIN: ";
        int currentPin = EnterPIN();

        int accountIndex = LocatePosition(currentLoggedInAccountNumber);

        if (accountIndex == -1 || currentPin != Last.Accounts[accountIndex].pinCode) {
            DisplayErrorMessage("Invalid CURRENT PIN.");
            AccountMenu();
        }
        Displaytop("Change Pin");
        cout << "\t\t\t\tEnter your NEW 6-digit PIN: ";
        string newPinStr;
        cin >> newPinStr;

        if (!ValidPinChecker(newPinStr)) {
            DisplayErrorMessage("Invalid NEW PIN. It must be a 6-digit number.");
            AccountMenu();
        }
        int newPin = stoi(newPinStr);
        LoadingAni();
        Last.Accounts[accountIndex].pinCode = newPin;
        SaveAccountInfoToFile();
        SaveAccountInfoToDrive(Last.Accounts[accountIndex].accountNumber, newPin);
        Displaytop("PIN change successful.");
        system("pause");
    }

    void TransferToAnotherAccount() {
        if (currentLoggedInAccountNumber == -1) {
            Displaytop("You are not logged in.");
            LoginMenu();
        }
        Displaytop("Transfer to Another Account");
        cout << "\t\t\t\t\tEnter your 6 PIN: ";
        int enteredPin = EnterPIN();
        int senderAccountIndex = LocatePosition(currentLoggedInAccountNumber);

        if (senderAccountIndex == -1 || enteredPin != Last.Accounts[senderAccountIndex].pinCode) {
            DisplayErrorMessage("Invalid PIN.");
            FundTransferMenu();
        }
        double currentBalance = Last.Accounts[senderAccountIndex].balance;
        Displaytop("Your current balance: $" + to_string(currentBalance));

        Displaytop("Transfer to Another Account");
        cout << "\t\t\t\t\tEnter the recipient's account number: ";
        int recipientAccountNumber;
        cin >> recipientAccountNumber;
        int recipientAccountIndex = LocatePosition(recipientAccountNumber);

        if (recipientAccountIndex == -1) {
            DisplayErrorMessage("Recipient account not found.", 2000);
            FundTransferMenu();
        }
        if (currentLoggedInAccountNumber == recipientAccountNumber) {
            DisplayErrorMessage("Cannot transfer funds to your own account.", 2000);
            FundTransferMenu();
        }
        double transferAmount;
        Displaytop("Transfer to Another Account");
        cout << "Enter the amount to transfer: $";
        cin >> transferAmount;

        if (transferAmount <= 0) {
            DisplayErrorMessage("Invalid transfer amount.", 1000);
            FundTransferMenu();
        }
        if (transferAmount > Last.Accounts[senderAccountIndex].balance) {
            DisplayErrorMessage("Insufficient funds for the transfer.", 1000);
            FundTransferMenu();
        }
        Displaytop("Transfer to Another Account");
        cout << "Enter your PIN again to confirm the transaction: ";
        int confirmPin = EnterPIN();

        if (confirmPin != enteredPin) {
            DisplayErrorMessage("PIN confirmation failed");
            DisplayErrorMessage("Transaction canceled", 2000);
            FundTransferMenu();
        }
        Last.Accounts[senderAccountIndex].balance -= transferAmount;
        Last.Accounts[recipientAccountIndex].balance += transferAmount;
        SaveAccountInfoToFile();
        Displaytop("Transfer successful.");
        Displaytop("Your updated balance: $" + to_string(Last.Accounts[senderAccountIndex].balance));
        system("Pause");
    }

    void LoginMenu() {
        int LMchoice;
            do {
                system("cls");
                SetColorBlue();
                displayATMArt();
                cout << endl << endl;
                cout << "\t\t\t\t\t\t1. Make A Transaction" << endl;
                cout << "\t\t\t\t\t\t2. Enroll New Account" << endl;
                cout << "\t\t\t\t\t\t3. Exit ATM" << endl;
                cout << endl << "\t\t\t\t\t\tEnter your Choice: ";

                cin >> LMchoice;
                switch (LMchoice) {
                    case 1: {
                        int cardResult = MainCardDetector();
                        if (cardResult == 1) {
                            TransactionMenu();
                        } else if (cardResult == 0) {
                            DisplayErrorMessage("Card not Registered", 2000);
                        } else {
                            DisplayMessage("Returning to Login Menu", 1000);
                        }
                        break;
                    }
                    case 2:
                        LoadingAni();
                        CreateAccount();
                        break;
                    case 3:
                        Displaytop("Thank you for Banking with us!", 2000);
                        exit(0);
                    default:
                        DisplayMessage("Unavailable Input", 1000);
                        LoginMenu();
            }
        } while (LMchoice != 4);
    }

    void TransactionMenu() {
        int TMchoice;
        do {
            system("cls");
            SetColorBlue();
            displayATMArt();
            cout << endl << endl;
            cout << "\t\t\t\t\t\tTransaction Menu" << endl;
            cout << "\t\t\t\t\t\t1. Withdraw" << endl;
            cout << "\t\t\t\t\t\t2. Deposit" << endl;
            cout << "\t\t\t\t\t\t3. Balance Inquiry" << endl;
            cout << "\t\t\t\t\t\t4. Fund Transfer" << endl;
            cout << "\t\t\t\t\t\t5. Manage Account" << endl;
            cout << "\t\t\t\t\t\t6. Exit" << endl;
            cout << endl << "\t\t\t\t\t\tEnter your Choice: ";

            cin >> TMchoice;

            switch (TMchoice) {
                case 1:
                    LoadingAni();
                    Withdraw();
                    break;
                case 2:
                    LoadingAni();
                    Deposit();
                    break;
                case 3:
                    LoadingAni();
                    BalanceInquiry();
                    break;
                case 4:
                    FundTransferMenu();
                    break;
                case 5:
                    AccountMenu();
                    break;
                case 6:
                    Displaytop("Thank you for Banking with us!", 2000);
                    CardLogout();
                    break;
                default:
                    DisplayMessage("Invalid Choice", 1000);
                    TransactionMenu();
            }
        } while (TMchoice != 6);
    }

    void AccountMenu() {
        int AMchoice;
        do {
            system("cls");
            SetColorBlue();
            displayATMArt();
            cout << endl << endl;
            cout << "\t\t\t\t\t\tAccount Menu" << endl;
            cout << "\t\t\t\t\t\t1. Update Pin Code" << endl;
            cout << "\t\t\t\t\t\t2. Delete Account" << endl;
            cout << "\t\t\t\t\t\t3. Make a Transaction" << endl;
            cout << "\t\t\t\t\t\t4. Exit" << endl;
            cout << endl << "\t\t\t\t\t\tEnter your Choice: ";

            cin >> AMchoice;
            switch (AMchoice) {;
                case 1:
                    LoadingAni();
                    ChangePin();
                    break;
                case 2:
                    LoadingAni();
                    DeleteAccount();
                    break;
                case 3:
                    TransactionMenu();
                    break;
                case 4:
                    Displaytop("Thank you for Banking with us!", 2000);
                    CardLogout();
                    break;
                default:
                    DisplayMessage("Invalid Choice", 1000);
                    AccountMenu();
            }
        } while (AMchoice != 4);
    }

    void FundTransferMenu() {
        system("cls");
        SetColorBlue();
        displayATMArt();
        cout << endl << endl;
        cout << "\t\t\t\t\t\tFund Transfer Menu" << endl;
        cout << "\t\t\t\t\t\t1. Transfer Funds to Another Account" << endl;
        cout << "\t\t\t\t\t\t2. Make a Transaction" << endl;
        cout << "\t\t\t\t\t\tEnter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                LoadingAni();
                TransferToAnotherAccount();
                break;
            case 2:
                TransactionMenu();
                break;
            default:
                Displaytop("Invalid Choice", 1000);
                FundTransferMenu();
        }
    }
};

int main() {
    ATM atm;
    atm.MakeNull();
    atm.RetrieveAccounts();
    atm.LoginMenu();
    return 0;
}