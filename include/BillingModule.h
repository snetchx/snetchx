#pragma once
#include "DatabaseConnection.h"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

class BillingModule {
private:
    DatabaseConnection& db;

    string generateBillID() {
        // Find the smallest missing ID (reuses deleted IDs)
        auto res = db.executeQuery(
            "SELECT t1.num + 1 AS gap "
            "FROM (SELECT CAST(SUBSTRING(BillID, 4) AS UNSIGNED) AS num FROM Bill) t1 "
            "LEFT JOIN (SELECT CAST(SUBSTRING(BillID, 4) AS UNSIGNED) AS num FROM Bill) t2 "
            "ON t1.num + 1 = t2.num "
            "WHERE t2.num IS NULL "
            "ORDER BY gap LIMIT 1");
        
        if (res && res->next()) {
            int gapID = res->getInt("gap");
            char buffer[15];
            sprintf_s(buffer, "BIL%06d", gapID);
            return string(buffer);
        }
        
        // If no gaps found, get max + 1 (or start with BIL000001 if table is empty)
        auto maxRes = db.executeQuery(
            "SELECT COALESCE(MAX(CAST(SUBSTRING(BillID, 4) AS UNSIGNED)), 0) as MaxID FROM Bill");
        if (maxRes && maxRes->next()) {
            int maxID = maxRes->getInt("MaxID");
            char buffer[15];
            sprintf_s(buffer, "BIL%06d", maxID + 1);
            return string(buffer);
        }
        return "BIL000001";
    }

public:
    BillingModule(DatabaseConnection& database) : db(database) {}

    // Generate bill for order
    string generateBill(const string& orderID, const string& staffID,
        const string& paymentMethod) {
        try {
            // Validate payment method
            if (paymentMethod != "Cash" && paymentMethod != "Card" && paymentMethod != "E-Wallet") {
                cout << "[FAILED] Invalid payment method! Use 'Cash', 'Card', or 'E-Wallet'." << endl;
                return "";
            }

            // Check if order exists and is active
            auto checkOrder = db.prepareStatement(
                "SELECT Order_status, Total_amount FROM Orders WHERE OrderID = ?");
            double totalAmount = 0;
            if (checkOrder) {
                checkOrder->setString(1, orderID);
                auto res = checkOrder->executeQuery();
                if (res->next()) {
                    string status = res->getString("Order_status");
                    if (status != "Active") {
                        cout << "[FAILED] Order is not active! Status: " << status << endl;
                        return "";
                    }
                    totalAmount = res->getDouble("Total_amount");
                    if (totalAmount <= 0) {
                        cout << "[FAILED] Order has no items!" << endl;
                        return "";
                    }
                }
                else {
                    cout << "[FAILED] Order not found!" << endl;
                    return "";
                }
            }

            // Check if bill already exists for this order
            auto checkBill = db.prepareStatement("SELECT BillID FROM Bill WHERE OrderID = ?");
            if (checkBill) {
                checkBill->setString(1, orderID);
                auto res = checkBill->executeQuery();
                if (res->next()) {
                    cout << "[FAILED] Bill already exists for this order!" << endl;
                    cout << "[INFO] Bill ID: " << res->getString("BillID") << endl;
                    return "";
                }
            }

            // Check if staff is active
            auto checkStaff = db.prepareStatement(
                "SELECT Status FROM Staff WHERE StaffID = ?");
            if (checkStaff) {
                checkStaff->setString(1, staffID);
                auto res = checkStaff->executeQuery();
                if (res->next()) {
                    if (res->getString("Status") != "Active") {
                        cout << "[FAILED] Staff is not active!" << endl;
                        return "";
                    }
                }
                else {
                    cout << "[FAILED] Staff not found!" << endl;
                    return "";
                }
            }

            string billID = generateBillID();
            auto pstmt = db.prepareStatement(
                "INSERT INTO Bill (BillID, OrderID, StaffID, Total, Payment_method, Payment_status) "
                "VALUES (?, ?, ?, ?, ?, 'Unpaid')");
            if (pstmt) {
                pstmt->setString(1, billID);
                pstmt->setString(2, orderID);
                pstmt->setString(3, staffID);
                pstmt->setDouble(4, totalAmount);
                pstmt->setString(5, paymentMethod);
                pstmt->executeUpdate();

                cout << "[SUCCESS] Bill generated with ID: " << billID << endl;
                return billID;
            }
            return "";
        }
        catch (sql::SQLException& e) {
            cerr << "Error generating bill: " << e.what() << endl;
            return "";
        }
    }

    // Process payment
    bool processPayment(const string& billID) {
        try {
            // Check if bill exists and is unpaid
            auto checkBill = db.prepareStatement(
                "SELECT Payment_status, OrderID FROM Bill WHERE BillID = ?");
            string orderID;
            if (checkBill) {
                checkBill->setString(1, billID);
                auto res = checkBill->executeQuery();
                if (res->next()) {
                    if (res->getString("Payment_status") == "Paid") {
                        cout << "[INFO] Bill is already paid!" << endl;
                        return true;
                    }
                    orderID = res->getString("OrderID");
                }
                else {
                    cout << "[FAILED] Bill not found!" << endl;
                    return false;
                }
            }

            // Update bill status to Paid
            auto pstmt = db.prepareStatement(
                "UPDATE Bill SET Payment_status = 'Paid' WHERE BillID = ?");
            if (pstmt) {
                pstmt->setString(1, billID);
                pstmt->executeUpdate();

                // The trigger will set table to Vacant and order to Completed
                cout << "[SUCCESS] Payment processed successfully!" << endl;
                return true;
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error processing payment: " << e.what() << endl;
            return false;
        }
    }

    // View bill details
    void viewBillDetails(const string& billID) {
        try {
            auto billStmt = db.prepareStatement(
                "SELECT b.BillID, b.OrderID, s.Name as Staff_Name, "
                "b.Bill_date, b.Total, b.Payment_method, b.Payment_status, "
                "t.Table_number "
                "FROM Bill b "
                "JOIN Staff s ON b.StaffID = s.StaffID "
                "JOIN Orders o ON b.OrderID = o.OrderID "
                "JOIN Tables t ON o.TableID = t.TableID "
                "WHERE b.BillID = ?");

            if (billStmt) {
                billStmt->setString(1, billID);
                auto billRes = billStmt->executeQuery();

                if (billRes->next()) {
                    string orderID = billRes->getString("OrderID");

                    cout << "\n" << string(60, '=') << endl;
                    cout << "                      BILL" << endl;
                    cout << string(60, '=') << endl;
                    cout << "Bill ID: " << billRes->getString("BillID") << endl;
                    cout << "Order ID: " << orderID << endl;
                    cout << "Table: " << billRes->getString("Table_number") << endl;
                    cout << "Staff: " << billRes->getString("Staff_Name") << endl;
                    cout << "Date: " << billRes->getString("Bill_date") << endl;
                    cout << string(60, '-') << endl;

                    // Get order items
                    auto itemStmt = db.prepareStatement(
                        "SELECT m.Menu_name, oi.Quantity, oi.Unit_price, oi.Total "
                        "FROM Order_Item oi "
                        "JOIN Menu m ON oi.MenuID = m.MenuID "
                        "WHERE oi.OrderID = ?");
                    if (itemStmt) {
                        itemStmt->setString(1, orderID);
                        auto itemRes = itemStmt->executeQuery();

                        cout << left << setw(28) << "Item"
                            << setw(6) << "Qty"
                            << setw(12) << "Price"
                            << "Total" << endl;
                        cout << string(60, '-') << endl;

                        while (itemRes->next()) {
                            cout << left << setw(28) << itemRes->getString("Menu_name")
                                << setw(6) << itemRes->getInt("Quantity")
                                << "RM " << setw(8) << fixed << setprecision(2)
                                << itemRes->getDouble("Unit_price")
                                << "RM " << itemRes->getDouble("Total") << endl;
                        }
                    }

                    cout << string(60, '-') << endl;
                    cout << right << setw(48) << "TOTAL: RM "
                        << fixed << setprecision(2) << billRes->getDouble("Total") << endl;
                    cout << string(60, '-') << endl;
                    cout << "Payment Method: " << billRes->getString("Payment_method") << endl;
                    cout << "Payment Status: " << billRes->getString("Payment_status") << endl;
                    cout << string(60, '=') << endl;
                    cout << "           Thank you for dining with us!" << endl;
                    cout << string(60, '=') << endl;
                }
                else {
                    cout << "[FAILED] Bill not found!" << endl;
                }
            }
        }
        catch (sql::SQLException& e) {
            cerr << "Error viewing bill: " << e.what() << endl;
        }
    }

    // View all bills
    void viewAllBills() {
        auto res = db.executeQuery(
            "SELECT b.BillID, b.OrderID, s.Name as Staff_Name, "
            "b.Bill_date, b.Total, b.Payment_method, b.Payment_status "
            "FROM Bill b "
            "JOIN Staff s ON b.StaffID = s.StaffID "
            "ORDER BY b.Bill_date DESC");

        if (res) {
            cout << "\n" << string(95, '=') << endl;
            cout << "ALL BILLS" << endl;
            cout << string(95, '=') << endl;
            cout << left << setw(12) << "Bill ID"
                << setw(12) << "Order ID"
                << setw(18) << "Staff"
                << setw(12) << "Total"
                << setw(12) << "Method"
                << setw(10) << "Status"
                << "Date" << endl;
            cout << string(95, '-') << endl;

            while (res->next()) {
                cout << left << setw(12) << res->getString("BillID")
                    << setw(12) << res->getString("OrderID")
                    << setw(18) << res->getString("Staff_Name")
                    << "RM " << setw(8) << fixed << setprecision(2) << res->getDouble("Total")
                    << setw(12) << res->getString("Payment_method")
                    << setw(10) << res->getString("Payment_status")
                    << res->getString("Bill_date") << endl;
            }
            cout << string(95, '=') << endl;
        }
    }

    // View unpaid bills
    void viewUnpaidBills() {
        auto res = db.executeQuery(
            "SELECT b.BillID, b.OrderID, t.Table_number, b.Total, b.Payment_method "
            "FROM Bill b "
            "JOIN Orders o ON b.OrderID = o.OrderID "
            "JOIN Tables t ON o.TableID = t.TableID "
            "WHERE b.Payment_status = 'Unpaid' "
            "ORDER BY b.Bill_date");

        if (res) {
            cout << "\n" << string(60, '=') << endl;
            cout << "UNPAID BILLS" << endl;
            cout << string(60, '=') << endl;
            cout << left << setw(12) << "Bill ID"
                << setw(12) << "Order ID"
                << setw(10) << "Table"
                << setw(14) << "Amount"
                << "Payment Method" << endl;
            cout << string(60, '-') << endl;

            int count = 0;
            double totalUnpaid = 0;
            while (res->next()) {
                double amount = res->getDouble("Total");
                totalUnpaid += amount;
                cout << left << setw(12) << res->getString("BillID")
                    << setw(12) << res->getString("OrderID")
                    << setw(10) << res->getString("Table_number")
                    << "RM " << setw(10) << fixed << setprecision(2) << amount
                    << res->getString("Payment_method") << endl;
                count++;
            }
            if (count == 0) {
                cout << "No unpaid bills." << endl;
            }
            else {
                cout << string(60, '-') << endl;
                cout << "Total Unpaid: RM " << fixed << setprecision(2) << totalUnpaid << endl;
            }
            cout << string(60, '=') << endl;
        }
    }

    // Get bill for order
    string getBillForOrder(const string& orderID) {
        try {
            auto pstmt = db.prepareStatement("SELECT BillID FROM Bill WHERE OrderID = ?");
            if (pstmt) {
                pstmt->setString(1, orderID);
                auto res = pstmt->executeQuery();
                if (res->next()) {
                    return res->getString("BillID");
                }
            }
            return "";
        }
        catch (sql::SQLException& e) {
            return "";
        }
    }

    // Calculate daily sales
    double calculateDailySales(const string& date) {
        try {
            auto pstmt = db.prepareStatement(
                "SELECT COALESCE(SUM(Total), 0) as DailyTotal "
                "FROM Bill WHERE DATE(Bill_date) = ? AND Payment_status = 'Paid'");
            if (pstmt) {
                pstmt->setString(1, date);
                auto res = pstmt->executeQuery();
                if (res->next()) {
                    return res->getDouble("DailyTotal");
                }
            }
            return 0;
        }
        catch (sql::SQLException& e) {
            cerr << "Error calculating sales: " << e.what() << endl;
            return 0;
        }
    }
};
