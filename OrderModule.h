#pragma once
#include "DatabaseConnection.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

class OrderModule {
private:
    DatabaseConnection& db;

    string generateOrderID() {
        auto res = db.executeQuery("SELECT MAX(CAST(SUBSTRING(OrderID, 4) AS UNSIGNED)) as MaxID FROM Orders");
        if (res && res->next()) {
            int maxID = res->getInt("MaxID");
            char buffer[15];
            sprintf_s(buffer, "ORD%06d", maxID + 1);
            return string(buffer);
        }
        return "ORD000001";
    }

    string generateOrderItemID() {
        auto res = db.executeQuery("SELECT MAX(CAST(SUBSTRING(Order_itemID, 4) AS UNSIGNED)) as MaxID FROM Order_Item");
        if (res && res->next()) {
            int maxID = res->getInt("MaxID");
            char buffer[15];
            sprintf_s(buffer, "ORI%06d", maxID + 1);
            return string(buffer);
        }
        return "ORI000001";
    }

public:
    OrderModule(DatabaseConnection& database) : db(database) {}

    // Create new order
    string createOrder(const string& tableID, const string& staffID) {
        try {
            // Check if table can accept order (Occupied or Reserved)
            auto checkTable = db.prepareStatement(
                "SELECT Status FROM Tables WHERE TableID = ?");
            if (checkTable) {
                checkTable->setString(1, tableID);
                auto res = checkTable->executeQuery();
                if (res->next()) {
                    string status = res->getString("Status");
                    if (status != "Occupied" && status != "Reserved") {
                        cout << "[FAILED] Orders can only be created for Occupied or Reserved tables!" << endl;
                        return "";
                    }
                }
                else {
                    cout << "[FAILED] Table not found!" << endl;
                    return "";
                }
            }

            // Check if table already has an active order
            auto checkOrder = db.prepareStatement(
                "SELECT OrderID FROM Orders WHERE TableID = ? AND Order_status = 'Active'");
            if (checkOrder) {
                checkOrder->setString(1, tableID);
                auto res = checkOrder->executeQuery();
                if (res->next()) {
                    cout << "[FAILED] Table already has an active order!" << endl;
                    cout << "[INFO] Active Order ID: " << res->getString("OrderID") << endl;
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

            string orderID = generateOrderID();
            auto pstmt = db.prepareStatement(
                "INSERT INTO Orders (OrderID, TableID, StaffID, Total_amount, Order_status) "
                "VALUES (?, ?, ?, 0.00, 'Active')");
            if (pstmt) {
                pstmt->setString(1, orderID);
                pstmt->setString(2, tableID);
                pstmt->setString(3, staffID);
                pstmt->executeUpdate();

                // Update table status to Occupied
                db.executeUpdate("UPDATE Tables SET Status = 'Occupied' WHERE TableID = '" + tableID + "'");

                cout << "[SUCCESS] Order created with ID: " << orderID << endl;
                return orderID;
            }
            return "";
        }
        catch (sql::SQLException& e) {
            cerr << "Error creating order: " << e.what() << endl;
            return "";
        }
    }

    // Add item to order
    bool addOrderItem(const string& orderID, const string& menuID, int quantity) {
        try {
            // Validate quantity
            if (quantity < 1) {
                cout << "[FAILED] Quantity must be at least 1!" << endl;
                return false;
            }

            // Check if order is active
            auto checkOrder = db.prepareStatement(
                "SELECT Order_status FROM Orders WHERE OrderID = ?");
            if (checkOrder) {
                checkOrder->setString(1, orderID);
                auto res = checkOrder->executeQuery();
                if (res->next()) {
                    if (res->getString("Order_status") != "Active") {
                        cout << "[FAILED] Order is not active!" << endl;
                        return false;
                    }
                }
                else {
                    cout << "[FAILED] Order not found!" << endl;
                    return false;
                }
            }

            // Get menu price and check availability
            auto getPrice = db.prepareStatement(
                "SELECT Price, Menu_name, Availability FROM Menu WHERE MenuID = ?");
            double unitPrice = 0;
            string menuName;
            if (getPrice) {
                getPrice->setString(1, menuID);
                auto res = getPrice->executeQuery();
                if (res->next()) {
                    if (res->getString("Availability") != "Available") {
                        cout << "[FAILED] Menu item is not available!" << endl;
                        return false;
                    }
                    unitPrice = res->getDouble("Price");
                    menuName = res->getString("Menu_name");
                }
                else {
                    cout << "[FAILED] Menu item not found!" << endl;
                    return false;
                }
            }

            double total = unitPrice * quantity;
            string orderItemID = generateOrderItemID();

            auto pstmt = db.prepareStatement(
                "INSERT INTO Order_Item (Order_itemID, OrderID, MenuID, Quantity, Unit_price, Total) "
                "VALUES (?, ?, ?, ?, ?, ?)");
            if (pstmt) {
                pstmt->setString(1, orderItemID);
                pstmt->setString(2, orderID);
                pstmt->setString(3, menuID);
                pstmt->setInt(4, quantity);
                pstmt->setDouble(5, unitPrice);
                pstmt->setDouble(6, total);
                pstmt->executeUpdate();

                cout << "[SUCCESS] Added: " << quantity << "x " << menuName
                    << " @ RM" << fixed << setprecision(2) << unitPrice
                    << " = RM" << total << endl;
                return true;
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error adding item: " << e.what() << endl;
            return false;
        }
    }

    // Remove item from order
    bool removeOrderItem(const string& orderItemID) {
        try {
            // Check if order is active
            auto checkOrder = db.prepareStatement(
                "SELECT o.Order_status FROM Orders o "
                "JOIN Order_Item oi ON o.OrderID = oi.OrderID "
                "WHERE oi.Order_itemID = ?");
            if (checkOrder) {
                checkOrder->setString(1, orderItemID);
                auto res = checkOrder->executeQuery();
                if (res->next()) {
                    if (res->getString("Order_status") != "Active") {
                        cout << "[FAILED] Cannot modify completed order!" << endl;
                        return false;
                    }
                }
                else {
                    cout << "[FAILED] Order item not found!" << endl;
                    return false;
                }
            }

            auto pstmt = db.prepareStatement("DELETE FROM Order_Item WHERE Order_itemID = ?");
            if (pstmt) {
                pstmt->setString(1, orderItemID);
                int result = pstmt->executeUpdate();
                if (result > 0) {
                    cout << "[SUCCESS] Item removed from order!" << endl;
                    return true;
                }
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error removing item: " << e.what() << endl;
            return false;
        }
    }

    // View order details
    void viewOrderDetails(const string& orderID) {
        try {
            // Get order info
            auto orderStmt = db.prepareStatement(
                "SELECT o.OrderID, t.Table_number, s.Name as Staff_Name, "
                "o.Total_amount, o.Order_status, o.Order_date "
                "FROM Orders o "
                "JOIN Tables t ON o.TableID = t.TableID "
                "JOIN Staff s ON o.StaffID = s.StaffID "
                "WHERE o.OrderID = ?");
            if (orderStmt) {
                orderStmt->setString(1, orderID);
                auto orderRes = orderStmt->executeQuery();

                if (orderRes->next()) {
                    cout << "\n" << string(60, '=') << endl;
                    cout << "ORDER DETAILS" << endl;
                    cout << string(60, '=') << endl;
                    cout << "Order ID: " << orderRes->getString("OrderID") << endl;
                    cout << "Table: " << orderRes->getString("Table_number") << endl;
                    cout << "Staff: " << orderRes->getString("Staff_Name") << endl;
                    cout << "Date: " << orderRes->getString("Order_date") << endl;
                    cout << "Status: " << orderRes->getString("Order_status") << endl;
                    cout << string(60, '-') << endl;

                    // Get order items
                    auto itemStmt = db.prepareStatement(
                        "SELECT oi.Order_itemID, m.Menu_name, oi.Quantity, "
                        "oi.Unit_price, oi.Total "
                        "FROM Order_Item oi "
                        "JOIN Menu m ON oi.MenuID = m.MenuID "
                        "WHERE oi.OrderID = ?");
                    if (itemStmt) {
                        itemStmt->setString(1, orderID);
                        auto itemRes = itemStmt->executeQuery();

                        cout << left << setw(12) << "Item ID"
                            << setw(25) << "Menu Item"
                            << setw(8) << "Qty"
                            << setw(12) << "Price"
                            << "Total" << endl;
                        cout << string(60, '-') << endl;

                        while (itemRes->next()) {
                            cout << left << setw(12) << itemRes->getString("Order_itemID")
                                << setw(25) << itemRes->getString("Menu_name")
                                << setw(8) << itemRes->getInt("Quantity")
                                << "RM " << setw(8) << fixed << setprecision(2)
                                << itemRes->getDouble("Unit_price")
                                << "RM " << itemRes->getDouble("Total") << endl;
                        }
                    }

                    cout << string(60, '-') << endl;
                    cout << right << setw(48) << "ORDER TOTAL: RM "
                        << fixed << setprecision(2) << orderRes->getDouble("Total_amount") << endl;
                    cout << string(60, '=') << endl;
                }
                else {
                    cout << "[FAILED] Order not found!" << endl;
                }
            }
        }
        catch (sql::SQLException& e) {
            cerr << "Error viewing order: " << e.what() << endl;
        }
    }

    // View all active orders
    void viewActiveOrders() {
        auto res = db.executeQuery(
            "SELECT o.OrderID, t.Table_number, s.Name as Staff_Name, "
            "o.Total_amount, o.Order_date "
            "FROM Orders o "
            "JOIN Tables t ON o.TableID = t.TableID "
            "JOIN Staff s ON o.StaffID = s.StaffID "
            "WHERE o.Order_status = 'Active' "
            "ORDER BY o.Order_date");

        if (res) {
            cout << "\n" << string(75, '=') << endl;
            cout << "ACTIVE ORDERS" << endl;
            cout << string(75, '=') << endl;
            cout << left << setw(15) << "Order ID"
                << setw(10) << "Table"
                << setw(20) << "Staff"
                << setw(15) << "Amount (RM)"
                << "Date" << endl;
            cout << string(75, '-') << endl;

            int count = 0;
            while (res->next()) {
                cout << left << setw(15) << res->getString("OrderID")
                    << setw(10) << res->getString("Table_number")
                    << setw(20) << res->getString("Staff_Name")
                    << setw(15) << fixed << setprecision(2) << res->getDouble("Total_amount")
                    << res->getString("Order_date") << endl;
                count++;
            }
            if (count == 0) {
                cout << "No active orders found." << endl;
            }
            cout << string(75, '=') << endl;
        }
    }

    // View all orders
    void viewAllOrders() {
        auto res = db.executeQuery(
            "SELECT o.OrderID, t.Table_number, s.Name as Staff_Name, "
            "o.Total_amount, o.Order_status, o.Order_date "
            "FROM Orders o "
            "JOIN Tables t ON o.TableID = t.TableID "
            "JOIN Staff s ON o.StaffID = s.StaffID "
            "ORDER BY o.Order_date DESC");

        if (res) {
            cout << "\n" << string(90, '=') << endl;
            cout << "ALL ORDERS" << endl;
            cout << string(90, '=') << endl;
            cout << left << setw(15) << "Order ID"
                << setw(8) << "Table"
                << setw(18) << "Staff"
                << setw(12) << "Amount"
                << setw(12) << "Status"
                << "Date" << endl;
            cout << string(90, '-') << endl;

            while (res->next()) {
                cout << left << setw(15) << res->getString("OrderID")
                    << setw(8) << res->getString("Table_number")
                    << setw(18) << res->getString("Staff_Name")
                    << "RM " << setw(9) << fixed << setprecision(2) << res->getDouble("Total_amount")
                    << setw(12) << res->getString("Order_status")
                    << res->getString("Order_date") << endl;
            }
            cout << string(90, '=') << endl;
        }
    }

    // Cancel order
    bool cancelOrder(const string& orderID) {
        try {
            // Check if order is active
            auto checkOrder = db.prepareStatement(
                "SELECT Order_status, TableID FROM Orders WHERE OrderID = ?");
            string tableID;
            if (checkOrder) {
                checkOrder->setString(1, orderID);
                auto res = checkOrder->executeQuery();
                if (res->next()) {
                    if (res->getString("Order_status") != "Active") {
                        cout << "[FAILED] Only active orders can be cancelled!" << endl;
                        return false;
                    }
                    tableID = res->getString("TableID");
                }
                else {
                    cout << "[FAILED] Order not found!" << endl;
                    return false;
                }
            }

            // Cancel order
            auto pstmt = db.prepareStatement(
                "UPDATE Orders SET Order_status = 'Cancelled' WHERE OrderID = ?");
            if (pstmt) {
                pstmt->setString(1, orderID);
                pstmt->executeUpdate();

                // Set table to vacant
                db.executeUpdate("UPDATE Tables SET Status = 'Vacant' WHERE TableID = '" + tableID + "'");

                cout << "[SUCCESS] Order cancelled successfully!" << endl;
                return true;
            }
            return false;
        }
        catch (sql::SQLException& e) {
            cerr << "Error cancelling order: " << e.what() << endl;
            return false;
        }
    }

    // Get order total
    double getOrderTotal(const string& orderID) {
        try {
            auto pstmt = db.prepareStatement("SELECT Total_amount FROM Orders WHERE OrderID = ?");
            if (pstmt) {
                pstmt->setString(1, orderID);
                auto res = pstmt->executeQuery();
                if (res->next()) {
                    return res->getDouble("Total_amount");
                }
            }
            return 0;
        }
        catch (sql::SQLException& e) {
            cerr << "Error getting total: " << e.what() << endl;
            return 0;
        }
    }

    // Check if order exists and is active
    bool isOrderActive(const string& orderID) {
        try {
            auto pstmt = db.prepareStatement(
                "SELECT OrderID FROM Orders WHERE OrderID = ? AND Order_status = 'Active'");
            if (pstmt) {
                pstmt->setString(1, orderID);
                auto res = pstmt->executeQuery();
                return res->next();
            }
            return false;
        }
        catch (sql::SQLException& e) {
            return false;
        }
    }

    // Get active order for table
    string getActiveOrderForTable(const string& tableID) {
        try {
            auto pstmt = db.prepareStatement(
                "SELECT OrderID FROM Orders WHERE TableID = ? AND Order_status = 'Active'");
            if (pstmt) {
                pstmt->setString(1, tableID);
                auto res = pstmt->executeQuery();
                if (res->next()) {
                    return res->getString("OrderID");
                }
            }
            return "";
        }
        catch (sql::SQLException& e) {
            return "";
        }
    }
};
