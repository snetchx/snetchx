#include "DatabaseConnection.h"
#include "AdminModule.h"
#include "StaffModule.h"
#include "MenuModule.h"
#include "TableModule.h"
#include "OrderModule.h"
#include "BillingModule.h"
#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include <conio.h>  // For _getch() on Windows

using namespace std;

// Global modules
DatabaseConnection db;
AdminModule* adminModule = nullptr;
StaffModule* staffModule = nullptr;
MenuModule* menuModule = nullptr;
TableModule* tableModule = nullptr;
OrderModule* orderModule = nullptr;
BillingModule* billingModule = nullptr;

// Utility functions
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void clearScreen() {
    system("cls");  // Clear screen on Windows
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
    clearScreen();  // Clear screen after user presses enter
}

void printHeader(const string& title) {
    cout << "\n" << string(60, '=') << endl;
    cout << title << endl;
    cout << string(60, '=') << endl;
}

// Function to read password with masking (shows * instead of characters)
string getHiddenPassword() {
    string password = "";
    char ch;
    
    while (true) {
        ch = _getch();  // Get character without echoing
        
        if (ch == 13) {  // Enter key
            cout << endl;
            break;
        }
        else if (ch == 8) {  // Backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";  // Move back, print space, move back again
            }
        }
        else if (ch >= 32 && ch <= 126) {  // Printable characters
            password += ch;
            cout << '*';  // Display asterisk
        }
    }
    
    return password;
}

// Function to get password with confirmation (for new users)
string getPasswordWithConfirmation() {
    string password1, password2;
    
    do {
        cout << "Enter password: ";
        password1 = getHiddenPassword();
        
        if (password1.length() < 4) {
            cout << "[WARNING] Password should be at least 4 characters!" << endl;
            cout << "Try again.\n" << endl;
            continue;
        }
        
        cout << "Confirm password: ";
        password2 = getHiddenPassword();
        
        if (password1 != password2) {
            cout << "[ERROR] Passwords do not match! Try again.\n" << endl;
        }
        else {
            cout << "[SUCCESS] Password confirmed!" << endl;
            return password1;
        }
    } while (true);
}

// ============================================
// LOGIN MENU
// ============================================
int showLoginMenu() {
    printHeader("RESTAURANT MANAGEMENT SYSTEM");
    cout << "1. Admin Login" << endl;
    cout << "2. Staff Login" << endl;
    cout << "3. Exit" << endl;
    cout << string(60, '-') << endl;
    cout << "Enter your choice: ";
    int choice;
    cin >> choice;
    clearInputBuffer();
    return choice;
}

// ============================================
// ADMIN MENU FUNCTIONS
// ============================================
void showAdminMainMenu() {
    printHeader("ADMIN DASHBOARD - Welcome, " + adminModule->getAdminName());
    cout << "1.  Staff Management" << endl;
    cout << "2.  Menu Management" << endl;
    cout << "3.  Table Management" << endl;
    cout << "4.  View All Orders" << endl;
    cout << "5.  View All Bills" << endl;
    cout << "6.  Daily Sales Report" << endl;
    cout << "7.  View Daily Sales History" << endl;
    cout << "8.  Logout" << endl;
    cout << string(60, '-') << endl;
    cout << "Enter your choice: ";
}

void adminStaffManagement() {
    int choice;
    do {
        clearScreen();
        printHeader("STAFF MANAGEMENT");
        cout << "1. View All Staff" << endl;
        cout << "2. View Active Staff" << endl;
        cout << "3. Add New Staff" << endl;
        cout << "4. Update Staff Status" << endl;
        cout << "5. Delete Staff" << endl;
        cout << "6. Back to Admin Menu" << endl;
        cout << string(60, '-') << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        clearInputBuffer();

        switch (choice) {
        case 1:
            staffModule->viewAllStaff();
            pressEnterToContinue();
            break;
        case 2:
            staffModule->viewActiveStaff();
            pressEnterToContinue();
            break;
        case 3: {
            string name, email, address, password;
            cout << "Enter staff name: ";
            getline(cin, name);
            cout << "Enter email: ";
            getline(cin, email);
            cout << "Enter address: ";
            getline(cin, address);
            cout << endl;
            password = getPasswordWithConfirmation();
            staffModule->addStaff(name, email, address, password);
            pressEnterToContinue();
            break;
        }
        case 4: {
            staffModule->viewAllStaff();
            string staffID, status;
            cout << "\nEnter Staff ID to update: ";
            getline(cin, staffID);
            cout << "Enter new status (Active/Inactive): ";
            getline(cin, status);
            staffModule->updateStaffStatus(staffID, status);
            pressEnterToContinue();
            break;
        }
        case 5: {
            staffModule->viewAllStaff();
            string staffID;
            cout << "\nEnter Staff ID to delete: ";
            getline(cin, staffID);
            staffModule->deleteStaff(staffID);
            pressEnterToContinue();
            break;
        }
        case 6:
            break;
        default:
            cout << "Invalid choice!" << endl;
        }
    } while (choice != 6);
}

void adminMenuManagement() {
    int choice;
    do {
        clearScreen();
        printHeader("MENU MANAGEMENT");
        cout << "1. View All Menu Items" << endl;
        cout << "2. View Available Menu" << endl;
        cout << "3. View by Category" << endl;
        cout << "4. Search Menu" << endl;
        cout << "5. Add Menu Item" << endl;
        cout << "6. Update Price" << endl;
        cout << "7. Update Availability" << endl;
        cout << "8. Delete Menu Item" << endl;
        cout << "9. Back to Admin Menu" << endl;
        cout << string(60, '-') << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        clearInputBuffer();

        switch (choice) {
        case 1:
            menuModule->viewAllMenu();
            pressEnterToContinue();
            break;
        case 2:
            menuModule->viewAvailableMenu();
            pressEnterToContinue();
            break;
        case 3: {
            string category;
            cout << "Enter category (Food/Beverage/Dessert): ";
            getline(cin, category);
            menuModule->viewMenuByCategory(category);
            pressEnterToContinue();
            break;
        }
        case 4: {
            string searchTerm;
            cout << "Enter search term: ";
            getline(cin, searchTerm);
            menuModule->searchMenu(searchTerm);
            pressEnterToContinue();
            break;
        }
        case 5: {
            string name, category;
            double price;
            cout << "Enter item name: ";
            getline(cin, name);
            cout << "Enter price (RM): ";
            cin >> price;
            clearInputBuffer();
            cout << "Enter category (Food/Beverage/Dessert): ";
            getline(cin, category);
            menuModule->addMenuItem(name, price, category);
            pressEnterToContinue();
            break;
        }
        case 6: {
            menuModule->viewAllMenu();
            string menuID;
            double newPrice;
            cout << "\nEnter Menu ID to update: ";
            getline(cin, menuID);
            cout << "Enter new price (RM): ";
            cin >> newPrice;
            clearInputBuffer();
            menuModule->updateMenuPrice(menuID, newPrice);
            pressEnterToContinue();
            break;
        }
        case 7: {
            menuModule->viewAllMenu();
            string menuID, availability;
            cout << "\nEnter Menu ID to update: ";
            getline(cin, menuID);
            cout << "Enter availability (Available/Unavailable): ";
            getline(cin, availability);
            menuModule->updateMenuAvailability(menuID, availability);
            pressEnterToContinue();
            break;
        }
        case 8: {
            menuModule->viewAllMenu();
            string menuID;
            cout << "\nEnter Menu ID to delete: ";
            getline(cin, menuID);
            menuModule->deleteMenuItem(menuID);
            pressEnterToContinue();
            break;
        }
        case 9:
            break;
        default:
            cout << "Invalid choice!" << endl;
        }
    } while (choice != 9);
}

void adminTableManagement() {
    int choice;
    do {
        clearScreen();
        printHeader("TABLE MANAGEMENT");
        cout << "1. View All Tables" << endl;
        cout << "2. View Vacant Tables" << endl;
        cout << "3. View Occupied Tables" << endl;
        cout << "4. View Reserved Tables" << endl;
        cout << "5. Add New Table" << endl;
        cout << "6. Update Table Status" << endl;
        cout << "7. Update Table Capacity" << endl;
        cout << "8. Delete Table" << endl;
        cout << "9. Back to Admin Menu" << endl;
        cout << string(60, '-') << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        clearInputBuffer();

        switch (choice) {
        case 1:
            tableModule->viewAllTables();
            pressEnterToContinue();
            break;
        case 2:
            tableModule->viewTablesByStatus("Vacant");
            pressEnterToContinue();
            break;
        case 3:
            tableModule->viewTablesByStatus("Occupied");
            pressEnterToContinue();
            break;
        case 4:
            tableModule->viewTablesByStatus("Reserved");
            pressEnterToContinue();
            break;
        case 5: {
            string tableNumber;
            int capacity;
            cout << "Enter table number (e.g., T11): ";
            getline(cin, tableNumber);
            cout << "Enter capacity: ";
            cin >> capacity;
            clearInputBuffer();
            tableModule->addTable(tableNumber, capacity);
            pressEnterToContinue();
            break;
        }
        case 6: {
            tableModule->viewAllTables();
            string tableID, status;
            cout << "\nEnter Table ID to update: ";
            getline(cin, tableID);
            cout << "Enter new status (Vacant/Occupied/Reserved): ";
            getline(cin, status);
            tableModule->updateTableStatus(tableID, status);
            pressEnterToContinue();
            break;
        }
        case 7: {
            tableModule->viewAllTables();
            string tableID;
            int capacity;
            cout << "\nEnter Table ID to update: ";
            getline(cin, tableID);
            cout << "Enter new capacity: ";
            cin >> capacity;
            clearInputBuffer();
            tableModule->updateTableCapacity(tableID, capacity);
            pressEnterToContinue();
            break;
        }
        case 8: {
            tableModule->viewAllTables();
            string tableID;
            cout << "\nEnter Table ID to delete: ";
            getline(cin, tableID);
            tableModule->deleteTable(tableID);
            pressEnterToContinue();
            break;
        }
        case 9:
            break;
        default:
            cout << "Invalid choice!" << endl;
        }
    } while (choice != 9);
}

void adminDashboard() {
    int choice;
    do {
        clearScreen();
        showAdminMainMenu();
        cin >> choice;
        clearInputBuffer();

        switch (choice) {
        case 1:
            adminStaffManagement();
            break;
        case 2:
            adminMenuManagement();
            break;
        case 3:
            adminTableManagement();
            break;
        case 4:
            orderModule->viewAllOrders();
            pressEnterToContinue();
            break;
        case 5:
            billingModule->viewAllBills();
            pressEnterToContinue();
            break;
        case 6: {
            string date;
            cout << "Enter date (YYYY-MM-DD) or press Enter for today: ";
            getline(cin, date);
            if (date.empty()) {
                // Get current date
                time_t now = time(0);
                tm ltm;
                localtime_s(&ltm, &now);
                char buffer[11];
                strftime(buffer, 11, "%Y-%m-%d", &ltm);
                date = buffer;
            }
            adminModule->generateDailySalesReport(date);
            pressEnterToContinue();
            break;
        }
        case 7:
            adminModule->viewAllDailySales();
            pressEnterToContinue();
            break;
        case 8:
            adminModule->logout();
            break;
        default:
            cout << "Invalid choice!" << endl;
        }
    } while (choice != 8);
}

// ============================================
// STAFF MENU FUNCTIONS
// ============================================
void showStaffMainMenu() {
    printHeader("STAFF DASHBOARD - Welcome, " + staffModule->getStaffName());
    cout << "1.  View Menu" << endl;
    cout << "2.  View Tables" << endl;
    cout << "3.  Create New Order" << endl;
    cout << "4.  Add Items to Order" << endl;
    cout << "5.  View Order Details" << endl;
    cout << "6.  View Active Orders" << endl;
    cout << "7.  Cancel Order" << endl;
    cout << "8.  Process Payment (Auto-generates bill if needed)" << endl;
    cout << "9.  View Unpaid Bills" << endl;
    cout << "10. Logout" << endl;
    cout << string(60, '-') << endl;
    cout << "Enter your choice: ";
}

void staffCreateOrder() {
    tableModule->viewAllTables();
    cout << "\n--- CREATE NEW ORDER ---" << endl;

    string tableID;
    cout << "Enter Table ID (e.g., TBL001): ";
    getline(cin, tableID);

    // Check table status
    string status = tableModule->getTableStatus(tableID);
    if (status.empty()) {
        cout << "[FAILED] Table not found!" << endl;
        return;
    }

    if (status == "Vacant") {
        cout << "[INFO] Table is vacant. Setting to Occupied..." << endl;
        tableModule->updateTableStatus(tableID, "Occupied");
    }

    // Check if table already has active order
    if (tableModule->hasActiveOrder(tableID)) {
        cout << "[FAILED] Table already has an active order!" << endl;
        string existingOrder = orderModule->getActiveOrderForTable(tableID);
        if (!existingOrder.empty()) {
            cout << "[INFO] Existing Order ID: " << existingOrder << endl;
        }
        return;
    }

    string orderID = orderModule->createOrder(tableID, staffModule->getStaffID());

    if (!orderID.empty()) {
        cout << "\n[SUCCESS] Order created: " << orderID << endl;
        cout << "\n--- ADD ITEMS TO ORDER ---" << endl;
        cout << "Enter '0' or 'STOP' to finish adding items\n" << endl;
        
        menuModule->viewAvailableMenu();

        while (true) {
            string menuID;
            int quantity;

            cout << "\nEnter Menu ID (or '0'/'STOP' to finish): ";
            getline(cin, menuID);
            
            // Check for stop commands
            if (menuID == "0" || menuID == "STOP" || menuID == "stop" || menuID == "Stop") {
                break;
            }
            
            // Validate menu ID is not empty
            if (menuID.empty()) {
                cout << "[WARNING] Please enter a valid Menu ID or '0' to stop." << endl;
                continue;
            }

            cout << "Enter quantity: ";
            cin >> quantity;
            clearInputBuffer();
            
            // Validate quantity
            if (quantity <= 0) {
                cout << "[WARNING] Quantity must be greater than 0!" << endl;
                continue;
            }

            orderModule->addOrderItem(orderID, menuID, quantity);
        }

        // Show order summary
        cout << "\n--- ORDER SUMMARY ---" << endl;
        orderModule->viewOrderDetails(orderID);
    }
}

void staffAddItemsToOrder() {
    orderModule->viewActiveOrders();
    cout << "\n--- ADD ITEMS TO ORDER ---" << endl;

    string orderID;
    cout << "Enter Order ID: ";
    getline(cin, orderID);

    if (!orderModule->isOrderActive(orderID)) {
        cout << "[FAILED] Order not found or not active!" << endl;
        return;
    }

    cout << "\nEnter '0' or 'STOP' to finish adding items\n" << endl;
    menuModule->viewAvailableMenu();

    while (true) {
        string menuID;
        int quantity;

        cout << "\nEnter Menu ID (or '0'/'STOP' to finish): ";
        getline(cin, menuID);
        
        // Check for stop commands
        if (menuID == "0" || menuID == "STOP" || menuID == "stop" || menuID == "Stop") {
            break;
        }
        
        // Validate menu ID is not empty
        if (menuID.empty()) {
            cout << "[WARNING] Please enter a valid Menu ID or '0' to stop." << endl;
            continue;
        }

        cout << "Enter quantity: ";
        cin >> quantity;
        clearInputBuffer();
        
        // Validate quantity
        if (quantity <= 0) {
            cout << "[WARNING] Quantity must be greater than 0!" << endl;
            continue;
        }

        orderModule->addOrderItem(orderID, menuID, quantity);
    }

    // Show updated order
    cout << "\n--- UPDATED ORDER SUMMARY ---" << endl;
    orderModule->viewOrderDetails(orderID);
}

void staffProcessPayment() {
    orderModule->viewActiveOrders();
    cout << "\n--- PROCESS PAYMENT ---" << endl;

    string orderID;
    cout << "Enter Order ID: ";
    getline(cin, orderID);

    if (!orderModule->isOrderActive(orderID)) {
        cout << "[FAILED] Order not found or not active!" << endl;
        return;
    }

    // Check if order has items
    double total = orderModule->getOrderTotal(orderID);
    if (total <= 0) {
        cout << "[FAILED] Order has no items!" << endl;
        return;
    }

    // Show order details first
    cout << "\n--- Order Summary ---" << endl;
    orderModule->viewOrderDetails(orderID);

    // Check if bill already exists
    string billID = billingModule->getBillForOrder(orderID);
    
    if (billID.empty()) {
        // Bill doesn't exist, generate it first
        cout << "\nBill not found. Generating bill..." << endl;
        
        cout << "Select payment method:" << endl;
        cout << "1. Cash" << endl;
        cout << "2. Card" << endl;
        cout << "3. E-Wallet" << endl;
        cout << "Enter choice: ";

        int methodChoice;
        cin >> methodChoice;
        clearInputBuffer();

        string paymentMethod;
        switch (methodChoice) {
        case 1: paymentMethod = "Cash"; break;
        case 2: paymentMethod = "Card"; break;
        case 3: paymentMethod = "E-Wallet"; break;
        default:
            cout << "[FAILED] Invalid payment method!" << endl;
            return;
        }

        billID = billingModule->generateBill(orderID, staffModule->getStaffID(), paymentMethod);
        
        if (billID.empty()) {
            cout << "[FAILED] Failed to generate bill!" << endl;
            return;
        }
    } else {
        // Bill exists, show it
        cout << "\n--- Existing Bill ---" << endl;
        billingModule->viewBillDetails(billID);
    }

    // Process payment
    cout << "\nProcessing payment..." << endl;
    if (billingModule->processPayment(billID)) {
        cout << "\n--- Payment Receipt ---" << endl;
        billingModule->viewBillDetails(billID);
        cout << "\n[SUCCESS] Payment processed successfully!" << endl;
        cout << "[INFO] Table has been set to Vacant." << endl;
    }
}

void staffDashboard() {
    int choice;
    do {
        clearScreen();
        showStaffMainMenu();
        cin >> choice;
        clearInputBuffer();

        switch (choice) {
        case 1:
            menuModule->viewAvailableMenu();
            pressEnterToContinue();
            break;
        case 2:
            tableModule->viewAllTables();
            pressEnterToContinue();
            break;
        case 3:
            staffCreateOrder();
            pressEnterToContinue();
            break;
        case 4:
            staffAddItemsToOrder();
            pressEnterToContinue();
            break;
        case 5: {
            string orderID;
            cout << "Enter Order ID: ";
            getline(cin, orderID);
            orderModule->viewOrderDetails(orderID);
            pressEnterToContinue();
            break;
        }
        case 6:
            orderModule->viewActiveOrders();
            pressEnterToContinue();
            break;
        case 7: {
            orderModule->viewActiveOrders();
            string orderID;
            cout << "\nEnter Order ID to cancel: ";
            getline(cin, orderID);
            cout << "Are you sure? (Y/N): ";
            char confirm;
            cin >> confirm;
            clearInputBuffer();
            if (toupper(confirm) == 'Y') {
                orderModule->cancelOrder(orderID);
            }
            pressEnterToContinue();
            break;
        }
        case 8:
            staffProcessPayment();
            pressEnterToContinue();
            break;
        case 9:
            billingModule->viewUnpaidTables();
            pressEnterToContinue();
            break;
        case 10:
            staffModule->logout();
            break;
        default:
            cout << "Invalid choice!" << endl;
        }
    } while (choice != 10);
}

// ============================================
// MAIN FUNCTION
// ============================================
int main() {
    cout << "\n";
    cout << "============================================" << endl;
    cout << "   RESTAURANT MANAGEMENT SYSTEM" << endl;
    cout << "============================================" << endl;
    cout << "Initializing system...\n" << endl;

    // Connect to database
    if (!db.connect()) {
        cout << "\n[CRITICAL] Cannot connect to database!" << endl;
        cout << "Please ensure:" << endl;
        cout << "1. MySQL/XAMPP is running" << endl;
        cout << "2. Database 'restaurant_management' exists" << endl;
        cout << "3. Run the SQL script to create tables" << endl;
        pressEnterToContinue();
        return 1;
    }

    // Initialize modules
    adminModule = new AdminModule(db);
    staffModule = new StaffModule(db);
    menuModule = new MenuModule(db);
    tableModule = new TableModule(db);
    orderModule = new OrderModule(db);
    billingModule = new BillingModule(db);

    cout << "[SUCCESS] System initialized successfully!" << endl;

    // Main loop
    int choice;
    do {
        clearScreen();
        choice = showLoginMenu();

        switch (choice) {
        case 1: {
            // Admin Login
            string email, password;
            cout << "\n--- ADMIN LOGIN ---" << endl;
            cout << "Email: ";
            getline(cin, email);
            cout << "Password: ";
            password = getHiddenPassword();

            if (adminModule->login(email, password)) {
                adminDashboard();
            }
            pressEnterToContinue();
            break;
        }
        case 2: {
            // Staff Login
            string email, password;
            cout << "\n--- STAFF LOGIN ---" << endl;
            cout << "Email: ";
            getline(cin, email);
            cout << "Password: ";
            password = getHiddenPassword();

            if (staffModule->login(email, password)) {
                staffDashboard();
            }
            pressEnterToContinue();
            break;
        }
        case 3:
            cout << "\nThank you for using Restaurant Management System!" << endl;
            break;
        default:
            cout << "Invalid choice! Please try again." << endl;
        }

    } while (choice != 3);

    // Cleanup
    delete adminModule;
    delete staffModule;
    delete menuModule;
    delete tableModule;
    delete orderModule;
    delete billingModule;

    db.disconnect();

    cout << "\n============================================" << endl;
    cout << "   SYSTEM SHUTDOWN COMPLETE" << endl;
    cout << "============================================" << endl;

    return 0;
}
