#pragma once
#include "DatabaseConnection.h"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

class AdminModule {
private:
    DatabaseConnection& db;
    string currentAdminID;
    string currentAdminName;
    bool isLoggedIn;

public:
    AdminModule(DatabaseConnection& database) : db(database), isLoggedIn(false) {}

    // Admin Login
    bool login(const string& email, const string& password) {
        try {
            auto pstmt = db.prepareStatement(
                "SELECT AdminID, Name FROM Admin WHERE Email = ? AND Password = ?");
            if (pstmt) {
                pstmt->setString(1, email);
                pstmt->setString(2, password);
                auto res = pstmt->executeQuery();

                if (res->next()) {
                    currentAdminID = res->getString("AdminID");
                    currentAdminName = res->getString("Name");
                    isLoggedIn = true;
                    cout << "\n[SUCCESS] Welcome, " << currentAdminName << "!" << endl;
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
        currentAdminID = "";
        currentAdminName = "";
        isLoggedIn = false;
        cout << "[SUCCESS] Logged out successfully!" << endl;
    }

    bool checkLogin() const { return isLoggedIn; }
    string getAdminID() const { return currentAdminID; }
    string getAdminName() const { return currentAdminName; }

    // View all admins
    void viewAllAdmins() {
        auto res = db.executeQuery("SELECT AdminID, Name, Email, Address FROM Admin ORDER BY Name");
        if (res) {
            cout << "\n" << string(80, '=') << endl;
            cout << "ADMIN LIST" << endl;
            cout << string(80, '=') << endl;
            cout << left << setw(12) << "Admin ID"
                << setw(25) << "Name"
                << setw(30) << "Email"
                << "Address" << endl;
            cout << string(80, '-') << endl;

            while (res->next()) {
                cout << left << setw(12) << res->getString("AdminID")
                    << setw(25) << res->getString("Name")
                    << setw(30) << res->getString("Email")
                    << res->getString("Address") << endl;
            }
            cout << string(80, '=') << endl;
        }
    }

    // Generate Daily Sales Report
    void generateDailySalesReport(const string& date) {
        try {
            string query = "SELECT COUNT(BillID) as TotalBills, "
                "COALESCE(SUM(Total), 0) as TotalRevenue "
                "FROM Bill WHERE DATE(Bill_date) = ? AND Payment_status = 'Paid'";

            auto pstmt = db.prepareStatement(query);
            if (pstmt) {
                pstmt->setString(1, date);
                auto res = pstmt->executeQuery();

                if (res->next()) {
                    int totalBills = res->getInt("TotalBills");
                    double totalRevenue = res->getDouble("TotalRevenue");

                    cout << "\n" << string(50, '=') << endl;
                    cout << "DAILY SALES REPORT - " << date << endl;
                    cout << string(50, '=') << endl;
                    cout << "Total Bills Processed: " << totalBills << endl;
                    cout << "Total Revenue: RM " << fixed << setprecision(2)
                        << totalRevenue << endl;
                    cout << string(50, '=') << endl;

                    // Save to Daily_Sells table
                    string salesID = "SAL" + date.substr(0, 4) + date.substr(5, 2) + date.substr(8, 2);
                    string insertQuery = "INSERT INTO Daily_Sells (SalesID, Sales_date, Daily_total) "
                        "VALUES ('" + salesID + "', '" + date + "', " +
                        to_string(totalRevenue) + ") "
                        "ON DUPLICATE KEY UPDATE Daily_total = " + to_string(totalRevenue);
                    db.executeUpdate(insertQuery);
                    cout << "[INFO] Report saved to Daily_Sells table." << endl;
                }
            }
        }
        catch (sql::SQLException& e) {
            cerr << "Error generating report: " << e.what() << endl;
        }
    }

    // View all daily sales
    void viewAllDailySales() {
        auto res = db.executeQuery(
            "SELECT SalesID, Sales_date, Daily_total FROM Daily_Sells ORDER BY Sales_date DESC");
        if (res) {
            cout << "\n" << string(50, '=') << endl;
            cout << "DAILY SALES HISTORY" << endl;
            cout << string(50, '=') << endl;
            cout << left << setw(15) << "Sales ID"
                << setw(15) << "Date"
                << "Total (RM)" << endl;
            cout << string(50, '-') << endl;

            double grandTotal = 0;
            while (res->next()) {
                double total = res->getDouble("Daily_total");
                grandTotal += total;
                cout << left << setw(15) << res->getString("SalesID")
                    << setw(15) << res->getString("Sales_date")
                    << fixed << setprecision(2) << total << endl;
            }
            cout << string(50, '-') << endl;
            cout << left << setw(30) << "GRAND TOTAL:"
                << fixed << setprecision(2) << grandTotal << endl;
            cout << string(50, '=') << endl;
        }
    }
};

