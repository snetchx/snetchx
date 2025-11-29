#pragma once
#include "DatabaseConnection.h"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

class MenuModule {
private:
    DatabaseConnection& db;

    string generateMenuID() {
        // Find the smallest missing ID (reuses deleted IDs)
        auto res = db.executeQuery(
            "SELECT t1.num + 1 AS gap "
            "FROM (SELECT CAST(SUBSTRING(MenuID, 4) AS UNSIGNED) AS num FROM Menu) t1 "
            "LEFT JOIN (SELECT CAST(SUBSTRING(MenuID, 4) AS UNSIGNED) AS num FROM Menu) t2 "
            "ON t1.num + 1 = t2.num "
            "WHERE t2.num IS NULL "
            "ORDER BY gap LIMIT 1");
        
        if (res && res->next()) {
            int gapID = res->getInt("gap");
            char buffer[10];
            sprintf_s(buffer, "MNU%03d", gapID);
            return string(buffer);
        }
        
        // If no gaps found, get max + 1 (or start with MNU001 if table is empty)
        auto maxRes = db.executeQuery(
            "SELECT COALESCE(MAX(CAST(SUBSTRING(MenuID, 4) AS UNSIGNED)), 0) as MaxID FROM Menu");
        if (maxRes && maxRes->next()) {
            int maxID = maxRes->getInt("MaxID");
            char buffer[10];
            sprintf_s(buffer, "MNU%03d", maxID + 1);
            return string(buffer);
        }
        return "MNU001";
    }

public:
    MenuModule(DatabaseConnection& database) : db(database) {}

    // View all menu items
    void viewAllMenu() {
        auto res = db.executeQuery(
            "SELECT MenuID, Menu_name, Price, Category, Availability "
            "FROM Menu ORDER BY Category, Menu_name");
        if (res) {
            cout << "\n" << string(85, '=') << endl;
            cout << "RESTAURANT MENU" << endl;
            cout << string(85, '=') << endl;
            cout << left << setw(10) << "Menu ID"
                << setw(30) << "Item Name"
                << setw(12) << "Price (RM)"
                << setw(15) << "Category"
                << "Status" << endl;
            cout << string(85, '-') << endl;

            string currentCategory = "";
            while (res->next()) {
                string category = res->getString("Category");
                if (category != currentCategory) {
                    if (!currentCategory.empty()) {
                        cout << string(85, '-') << endl;
                    }
                    currentCategory = category;
                }
                cout << left << setw(10) << res->getString("MenuID")
                    << setw(30) << res->getString("Menu_name")
                    << setw(12) << fixed << setprecision(2) << res->getDouble("Price")
                    << setw(15) << category
                    << res->getString("Availability") << endl;
            }
            cout << string(85, '=') << endl;
        }
    }

    // View available menu only
    void viewAvailableMenu() {
        auto res = db.executeQuery(
            "SELECT MenuID, Menu_name, Price, Category "
            "FROM Menu WHERE Availability = 'Available' ORDER BY Category, Menu_name");
        if (res) {
            cout << "\n" << string(70, '=') << endl;
            cout << "AVAILABLE MENU ITEMS" << endl;
            cout << string(70, '=') << endl;
            cout << left << setw(10) << "Menu ID"
                << setw(30) << "Item Name"
                << setw(12) << "Price (RM)"
                << "Category" << endl;
            cout << string(70, '-') << endl;

            while (res->next()) {
                cout << left << setw(10) << res->getString("MenuID")
                    << setw(30) << res->getString("Menu_name")
                    << setw(12) << fixed << setprecision(2) << res->getDouble("Price")
                    << res->getString("Category") << endl;
            }
            cout << string(70, '=') << endl;
        }
    }

    // View menu by category
    void viewMenuByCategory(const string& category) {
        auto pstmt = db.prepareStatement(
            "SELECT MenuID, Menu_name, Price, Availability "
            "FROM Menu WHERE Category = ? ORDER BY Menu_name");
        if (pstmt) {
            pstmt->setString(1, category);
            auto res = pstmt->executeQuery();

            cout << "\n" << string(60, '=') << endl;
            cout << category << " MENU" << endl;
            cout << string(60, '=') << endl;
            cout << left << setw(10) << "Menu ID"
                << setw(25) << "Item Name"
                << setw(12) << "Price (RM)"
                << "Status" << endl;
            cout << string(60, '-') << endl;

            int count = 0;
            while (res->next()) {
                cout << left << setw(10) << res->getString("MenuID")
                    << setw(25) << res->getString("Menu_name")
                    << setw(12) << fixed << setprecision(2) << res->getDouble("Price")
                    << res->getString("Availability") << endl;
                count++;
            }
            if (count == 0) {
                cout << "No items found in this category." << endl;
            }
            cout << string(60, '=') << endl;
        }
    }

    // Search menu
    void searchMenu(const string& searchTerm) {
        auto pstmt = db.prepareStatement(
            "SELECT MenuID, Menu_name, Price, Category, Availability "
            "FROM Menu WHERE Menu_name LIKE ? ORDER BY Menu_name");
        if (pstmt) {
            pstmt->setString(1, "%" + searchTerm + "%");
            auto res = pstmt->executeQuery();

            cout << "\n" << string(75, '=') << endl;
            cout << "SEARCH RESULTS FOR: " << searchTerm << endl;
            cout << string(75, '=') << endl;

            int count = 0;
            while (res->next()) {
                cout << left << setw(10) << res->getString("MenuID")
                    << setw(25) << res->getString("Menu_name")
                    << "RM " << setw(10) << fixed << setprecision(2) << res->getDouble("Price")
                    << setw(12) << res->getString("Category")
                    << res->getString("Availability") << endl;
                count++;
            }
            if (count == 0) {
                cout << "No items found matching '" << searchTerm << "'" << endl;
            }
            cout << string(75, '=') << endl;
        }
    }

    // Add menu item
    bool addMenuItem(const string& name, double price, const string& category) {
        try {
            // Validate category
            if (category != "Food" && category != "Beverage" && category != "Dessert") {
                cout << "[FAILED] Invalid category! Use 'Food', 'Beverage', or 'Dessert'." << endl;
                return false;
            }

            // Validate price
            if (price <= 0) {
                cout << "[FAILED] Price must be greater than 0!" << endl;
                return false;
            }

            string menuID = generateMenuID();
            auto pstmt = db.prepareStatement(
                "INSERT INTO Menu (MenuID, Menu_name, Price, Category, Availability) "
                "VALUES (?, ?, ?, ?, 'Available')");
            if (pstmt) {
                pstmt->setString(1, menuID);
                pstmt->setString(2, name);
                pstmt->setDouble(3, price);
                pstmt->setString(4, category);
                pstmt->executeUpdate();
                cout << "[SUCCESS] Menu item added with ID: " << menuID << endl;
                return true;
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error adding menu item: " << e.what() << endl;
            return false;
        }
    }

    // Update menu price
    bool updateMenuPrice(const string& menuID, double newPrice) {
        try {
            if (newPrice <= 0) {
                cout << "[FAILED] Price must be greater than 0!" << endl;
                return false;
            }

            auto pstmt = db.prepareStatement("UPDATE Menu SET Price = ? WHERE MenuID = ?");
            if (pstmt) {
                pstmt->setDouble(1, newPrice);
                pstmt->setString(2, menuID);
                int result = pstmt->executeUpdate();
                if (result > 0) {
                    cout << "[SUCCESS] Price updated to RM " << fixed
                        << setprecision(2) << newPrice << endl;
                    return true;
                }
                else {
                    cout << "[FAILED] Menu item not found!" << endl;
                }
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error updating price: " << e.what() << endl;
            return false;
        }
    }

    // Update menu availability
    bool updateMenuAvailability(const string& menuID, const string& availability) {
        try {
            if (availability != "Available" && availability != "Unavailable") {
                cout << "[FAILED] Invalid status! Use 'Available' or 'Unavailable'." << endl;
                return false;
            }

            auto pstmt = db.prepareStatement("UPDATE Menu SET Availability = ? WHERE MenuID = ?");
            if (pstmt) {
                pstmt->setString(1, availability);
                pstmt->setString(2, menuID);
                int result = pstmt->executeUpdate();
                if (result > 0) {
                    cout << "[SUCCESS] Availability updated to " << availability << endl;
                    return true;
                }
                else {
                    cout << "[FAILED] Menu item not found!" << endl;
                }
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error updating availability: " << e.what() << endl;
            return false;
        }
    }

    // Delete menu item
    bool deleteMenuItem(const string& menuID) {
        try {
            // Check if menu item has orders
            auto checkStmt = db.prepareStatement("SELECT Order_itemID FROM Order_Item WHERE MenuID = ?");
            if (checkStmt) {
                checkStmt->setString(1, menuID);
                auto checkRes = checkStmt->executeQuery();
                if (checkRes->next()) {
                    cout << "[FAILED] Cannot delete menu item with existing orders!" << endl;
                    cout << "[INFO] Set availability to 'Unavailable' instead." << endl;
                    return false;
                }
            }

            auto pstmt = db.prepareStatement("DELETE FROM Menu WHERE MenuID = ?");
            if (pstmt) {
                pstmt->setString(1, menuID);
                int result = pstmt->executeUpdate();
                if (result > 0) {
                    cout << "[SUCCESS] Menu item deleted successfully!" << endl;
                    return true;
                }
                else {
                    cout << "[FAILED] Menu item not found!" << endl;
                }
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error deleting menu item: " << e.what() << endl;
            return false;
        }
    }

    // Get menu item price (for order processing)
    double getMenuPrice(const string& menuID) {
        try {
            auto pstmt = db.prepareStatement(
                "SELECT Price FROM Menu WHERE MenuID = ? AND Availability = 'Available'");
            if (pstmt) {
                pstmt->setString(1, menuID);
                auto res = pstmt->executeQuery();
                if (res->next()) {
                    return res->getDouble("Price");
                }
            }
            return -1;
        }
        catch (sql::SQLException& e) {
            cerr << "Error getting price: " << e.what() << endl;
            return -1;
        }
    }

    // Check if menu item is available
    bool isMenuAvailable(const string& menuID) {
        try {
            auto pstmt = db.prepareStatement(
                "SELECT MenuID FROM Menu WHERE MenuID = ? AND Availability = 'Available'");
            if (pstmt) {
                pstmt->setString(1, menuID);
                auto res = pstmt->executeQuery();
                return res->next();
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error checking availability: " << e.what() << endl;
            return false;
        }
    }
};

