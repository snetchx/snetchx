#pragma once
#include "DatabaseConnection.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

class TableModule {
private:
    DatabaseConnection& db;

    string generateTableID() {
        // Find the smallest missing ID (reuses deleted IDs)
        auto res = db.executeQuery(
            "SELECT t1.num + 1 AS gap "
            "FROM (SELECT CAST(SUBSTRING(TableID, 4) AS UNSIGNED) AS num FROM Tables) t1 "
            "LEFT JOIN (SELECT CAST(SUBSTRING(TableID, 4) AS UNSIGNED) AS num FROM Tables) t2 "
            "ON t1.num + 1 = t2.num "
            "WHERE t2.num IS NULL "
            "ORDER BY gap LIMIT 1");
        
        if (res && res->next()) {
            int gapID = res->getInt("gap");
            char buffer[10];
            sprintf_s(buffer, "TBL%03d", gapID);
            return string(buffer);
        }
        
        // If no gaps found, get max + 1 (or start with TBL001 if table is empty)
        auto maxRes = db.executeQuery(
            "SELECT COALESCE(MAX(CAST(SUBSTRING(TableID, 4) AS UNSIGNED)), 0) as MaxID FROM Tables");
        if (maxRes && maxRes->next()) {
            int maxID = maxRes->getInt("MaxID");
            char buffer[10];
            sprintf_s(buffer, "TBL%03d", maxID + 1);
            return string(buffer);
        }
        return "TBL001";
    }

public:
    TableModule(DatabaseConnection& database) : db(database) {}

    // View all tables
    void viewAllTables() {
        auto res = db.executeQuery(
            "SELECT TableID, Table_number, Capacity, Status "
            "FROM Tables ORDER BY Table_number");
        if (res) {
            cout << "\n" << string(60, '=') << endl;
            cout << "RESTAURANT TABLES" << endl;
            cout << string(60, '=') << endl;
            cout << left << setw(12) << "Table ID"
                << setw(15) << "Table Number"
                << setw(12) << "Capacity"
                << "Status" << endl;
            cout << string(60, '-') << endl;

            while (res->next()) {
                cout << left << setw(12) << res->getString("TableID")
                    << setw(15) << res->getString("Table_number")
                    << setw(12) << res->getInt("Capacity")
                    << res->getString("Status") << endl;
            }
            cout << string(60, '=') << endl;
        }
    }

    // View tables by status
    void viewTablesByStatus(const string& status) {
        auto pstmt = db.prepareStatement(
            "SELECT TableID, Table_number, Capacity "
            "FROM Tables WHERE Status = ? ORDER BY Table_number");
        if (pstmt) {
            pstmt->setString(1, status);
            auto res = pstmt->executeQuery();

            cout << "\n" << string(50, '=') << endl;
            cout << status << " TABLES" << endl;
            cout << string(50, '=') << endl;
            cout << left << setw(12) << "Table ID"
                << setw(15) << "Table Number"
                << "Capacity" << endl;
            cout << string(50, '-') << endl;

            int count = 0;
            while (res->next()) {
                cout << left << setw(12) << res->getString("TableID")
                    << setw(15) << res->getString("Table_number")
                    << res->getInt("Capacity") << endl;
                count++;
            }
            if (count == 0) {
                cout << "No " << status << " tables found." << endl;
            }
            cout << string(50, '=') << endl;
        }
    }

    // Add new table
    bool addTable(const string& tableNumber, int capacity) {
        try {
            // Validate capacity
            if (capacity < 1) {
                cout << "[FAILED] Capacity must be at least 1!" << endl;
                return false;
            }

            // Check if table number already exists
            auto checkStmt = db.prepareStatement("SELECT TableID FROM Tables WHERE Table_number = ?");
            if (checkStmt) {
                checkStmt->setString(1, tableNumber);
                auto checkRes = checkStmt->executeQuery();
                if (checkRes->next()) {
                    cout << "[FAILED] Table number already exists!" << endl;
                    return false;
                }
            }

            string tableID = generateTableID();
            auto pstmt = db.prepareStatement(
                "INSERT INTO Tables (TableID, Table_number, Capacity, Status) "
                "VALUES (?, ?, ?, 'Vacant')");
            if (pstmt) {
                pstmt->setString(1, tableID);
                pstmt->setString(2, tableNumber);
                pstmt->setInt(3, capacity);
                pstmt->executeUpdate();
                cout << "[SUCCESS] Table added with ID: " << tableID << endl;
                return true;
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error adding table: " << e.what() << endl;
            return false;
        }
    }

    // Update table status
    bool updateTableStatus(const string& tableID, const string& status) {
        try {
            // Convert input to lowercase for case-insensitive comparison
            string statusLower = status;
            transform(statusLower.begin(), statusLower.end(), statusLower.begin(), ::tolower);
            
            // Normalize to proper case format
            string normalizedStatus;
            if (statusLower == "vacant") {
                normalizedStatus = "Vacant";
            }
            else if (statusLower == "occupied") {
                normalizedStatus = "Occupied";
            }
            else if (statusLower == "reserved") {
                normalizedStatus = "Reserved";
            }
            else {
                cout << "[FAILED] Invalid status! Use 'Vacant', 'Occupied', or 'Reserved'." << endl;
                return false;
            }

            // Business rule: Vacant tables cannot have active orders (check when setting to Vacant)
            if (normalizedStatus == "Vacant") {
                auto checkStmt = db.prepareStatement(
                    "SELECT OrderID FROM Orders WHERE TableID = ? AND Order_status = 'Active'");
                if (checkStmt) {
                    checkStmt->setString(1, tableID);
                    auto checkRes = checkStmt->executeQuery();
                    if (checkRes->next()) {
                        cout << "[FAILED] Cannot set to Vacant! Table has an active order." << endl;
                        return false;
                    }
                }
            }

            auto pstmt = db.prepareStatement("UPDATE Tables SET Status = ? WHERE TableID = ?");
            if (pstmt) {
                pstmt->setString(1, normalizedStatus);
                pstmt->setString(2, tableID);
                int result = pstmt->executeUpdate();
                if (result > 0) {
                    cout << "[SUCCESS] Table status updated to " << normalizedStatus << endl;
                    return true;
                }
                else {
                    cout << "[FAILED] Table not found!" << endl;
                }
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error updating status: " << e.what() << endl;
            return false;
        }
    }

    // Update table capacity
    bool updateTableCapacity(const string& tableID, int newCapacity) {
        try {
            if (newCapacity < 1) {
                cout << "[FAILED] Capacity must be at least 1!" << endl;
                return false;
            }

            auto pstmt = db.prepareStatement("UPDATE Tables SET Capacity = ? WHERE TableID = ?");
            if (pstmt) {
                pstmt->setInt(1, newCapacity);
                pstmt->setString(2, tableID);
                int result = pstmt->executeUpdate();
                if (result > 0) {
                    cout << "[SUCCESS] Table capacity updated to " << newCapacity << endl;
                    return true;
                }
                else {
                    cout << "[FAILED] Table not found!" << endl;
                }
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error updating capacity: " << e.what() << endl;
            return false;
        }
    }

    // Delete table
    bool deleteTable(const string& tableID) {
        try {
            // Check if table has orders
            auto checkStmt = db.prepareStatement("SELECT OrderID FROM Orders WHERE TableID = ?");
            if (checkStmt) {
                checkStmt->setString(1, tableID);
                auto checkRes = checkStmt->executeQuery();
                if (checkRes->next()) {
                    cout << "[FAILED] Cannot delete table with existing orders!" << endl;
                    return false;
                }
            }

            auto pstmt = db.prepareStatement("DELETE FROM Tables WHERE TableID = ?");
            if (pstmt) {
                pstmt->setString(1, tableID);
                int result = pstmt->executeUpdate();
                if (result > 0) {
                    cout << "[SUCCESS] Table deleted successfully!" << endl;
                    return true;
                }
                else {
                    cout << "[FAILED] Table not found!" << endl;
                }
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error deleting table: " << e.what() << endl;
            return false;
        }
    }

    // Check if table can accept orders (Occupied or Reserved)
    bool canAcceptOrder(const string& tableID) {
        try {
            auto pstmt = db.prepareStatement(
                "SELECT Status FROM Tables WHERE TableID = ?");
            if (pstmt) {
                pstmt->setString(1, tableID);
                auto res = pstmt->executeQuery();
                if (res->next()) {
                    string status = res->getString("Status");
                    return (status == "Occupied" || status == "Reserved");
                }
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error checking table: " << e.what() << endl;
            return false;
        }
    }

    // Get table status
    string getTableStatus(const string& tableID) {
        try {
            auto pstmt = db.prepareStatement("SELECT Status FROM Tables WHERE TableID = ?");
            if (pstmt) {
                pstmt->setString(1, tableID);
                auto res = pstmt->executeQuery();
                if (res->next()) {
                    return res->getString("Status");
                }
            }
            return "";
        }
        catch (sql::SQLException& e) {
            cerr << "Error getting status: " << e.what() << endl;
            return "";
        }
    }

    // Check if table has active order
    bool hasActiveOrder(const string& tableID) {
        try {
            auto pstmt = db.prepareStatement(
                "SELECT OrderID FROM Orders WHERE TableID = ? AND Order_status = 'Active'");
            if (pstmt) {
                pstmt->setString(1, tableID);
                auto res = pstmt->executeQuery();
                return res->next();
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error checking order: " << e.what() << endl;
            return false;
        }
    }
};

