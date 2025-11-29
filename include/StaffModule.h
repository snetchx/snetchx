#pragma once
#include "DatabaseConnection.h"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

class StaffModule {
private:
    DatabaseConnection& db;
    string currentStaffID;
    string currentStaffName;
    bool isLoggedIn;

    string generateStaffID() {
        auto res = db.executeQuery("SELECT MAX(CAST(SUBSTRING(StaffID, 4) AS UNSIGNED)) as MaxID FROM Staff");
        if (res && res->next()) {
            int maxID = res->getInt("MaxID");
            char buffer[10];
            sprintf_s(buffer, "STF%03d", maxID + 1);
            return string(buffer);
        }
        return "STF001";
    }

public:
    StaffModule(DatabaseConnection& database) : db(database), isLoggedIn(false) {}

    // Staff Login
    bool login(const string& email, const string& password) {
        try {
            auto pstmt = db.prepareStatement(
                "SELECT StaffID, Name, Status FROM Staff WHERE Email = ? AND Password = ?");
            if (pstmt) {
                pstmt->setString(1, email);
                pstmt->setString(2, password);
                auto res = pstmt->executeQuery();

                if (res->next()) {
                    string status = res->getString("Status");
                    if (status == "Inactive") {
                        cout << "[FAILED] Your account is inactive. Contact admin." << endl;
                        return false;
                    }
                    currentStaffID = res->getString("StaffID");
                    currentStaffName = res->getString("Name");
                    isLoggedIn = true;
                    cout << "\n[SUCCESS] Welcome, " << currentStaffName << "!" << endl;
                    return true;
                }
            }
            cout << "[FAILED] Invalid email or password!" << endl;
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Login error: " << e.what() << endl;
            return false;
        }
    }

    void logout() {
        currentStaffID = "";
        currentStaffName = "";
        isLoggedIn = false;
        cout << "[SUCCESS] Logged out successfully!" << endl;
    }

    bool checkLogin() const { return isLoggedIn; }
    string getStaffID() const { return currentStaffID; }
    string getStaffName() const { return currentStaffName; }

    // View all staff
    void viewAllStaff() {
        auto res = db.executeQuery(
            "SELECT StaffID, Name, Email, Address, Status FROM Staff ORDER BY Name");
        if (res) {
            cout << "\n" << string(90, '=') << endl;
            cout << "STAFF LIST" << endl;
            cout << string(90, '=') << endl;
            cout << left << setw(12) << "Staff ID"
                << setw(20) << "Name"
                << setw(28) << "Email"
                << setw(20) << "Address"
                << "Status" << endl;
            cout << string(90, '-') << endl;

            while (res->next()) {
                cout << left << setw(12) << res->getString("StaffID")
                    << setw(20) << res->getString("Name")
                    << setw(28) << res->getString("Email")
                    << setw(20) << res->getString("Address")
                    << res->getString("Status") << endl;
            }
            cout << string(90, '=') << endl;
        }
    }

    // View active staff only
    void viewActiveStaff() {
        auto res = db.executeQuery(
            "SELECT StaffID, Name, Email FROM Staff WHERE Status = 'Active' ORDER BY Name");
        if (res) {
            cout << "\n" << string(60, '=') << endl;
            cout << "ACTIVE STAFF" << endl;
            cout << string(60, '=') << endl;
            cout << left << setw(12) << "Staff ID"
                << setw(20) << "Name"
                << "Email" << endl;
            cout << string(60, '-') << endl;

            while (res->next()) {
                cout << left << setw(12) << res->getString("StaffID")
                    << setw(20) << res->getString("Name")
                    << res->getString("Email") << endl;
            }
            cout << string(60, '=') << endl;
        }
    }

    // Add new staff
    bool addStaff(const string& name, const string& email,
        const string& address, const string& password) {
        try {
            // Check if email already exists
            auto checkStmt = db.prepareStatement("SELECT StaffID FROM Staff WHERE Email = ?");
            if (checkStmt) {
                checkStmt->setString(1, email);
                auto checkRes = checkStmt->executeQuery();
                if (checkRes->next()) {
                    cout << "[FAILED] Email already registered!" << endl;
                    return false;
                }
            }

            string staffID = generateStaffID();
            auto pstmt = db.prepareStatement(
                "INSERT INTO Staff (StaffID, Name, Email, Address, Password, Status) "
                "VALUES (?, ?, ?, ?, ?, 'Active')");
            if (pstmt) {
                pstmt->setString(1, staffID);
                pstmt->setString(2, name);
                pstmt->setString(3, email);
                pstmt->setString(4, address);
                pstmt->setString(5, password);
                pstmt->executeUpdate();
                cout << "[SUCCESS] Staff added with ID: " << staffID << endl;
                return true;
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error adding staff: " << e.what() << endl;
            return false;
        }
    }

    // Update staff status
    bool updateStaffStatus(const string& staffID, const string& status) {
        try {
            if (status != "Active" && status != "Inactive") {
                cout << "[FAILED] Invalid status! Use 'Active' or 'Inactive'." << endl;
                return false;
            }

            auto pstmt = db.prepareStatement("UPDATE Staff SET Status = ? WHERE StaffID = ?");
            if (pstmt) {
                pstmt->setString(1, status);
                pstmt->setString(2, staffID);
                int result = pstmt->executeUpdate();
                if (result > 0) {
                    cout << "[SUCCESS] Staff status updated to " << status << endl;
                    return true;
                }
                else {
                    cout << "[FAILED] Staff not found!" << endl;
                }
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error updating staff: " << e.what() << endl;
            return false;
        }
    }

    // Delete staff
    bool deleteStaff(const string& staffID) {
        try {
            // Check if staff has orders
            auto checkStmt = db.prepareStatement("SELECT OrderID FROM Orders WHERE StaffID = ?");
            if (checkStmt) {
                checkStmt->setString(1, staffID);
                auto checkRes = checkStmt->executeQuery();
                if (checkRes->next()) {
                    cout << "[FAILED] Cannot delete staff with existing orders!" << endl;
                    cout << "[INFO] Set status to 'Inactive' instead." << endl;
                    return false;
                }
            }

            auto pstmt = db.prepareStatement("DELETE FROM Staff WHERE StaffID = ?");
            if (pstmt) {
                pstmt->setString(1, staffID);
                int result = pstmt->executeUpdate();
                if (result > 0) {
                    cout << "[SUCCESS] Staff deleted successfully!" << endl;
                    return true;
                }
                else {
                    cout << "[FAILED] Staff not found!" << endl;
                }
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error deleting staff: " << e.what() << endl;
            return false;
        }
    }
};

