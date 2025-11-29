-- ============================================
-- Restaurant Management System Database Schema
-- ============================================

-- Drop database if exists and create new one
DROP DATABASE IF EXISTS restaurant_management;
CREATE DATABASE restaurant_management;
USE restaurant_management;

-- ============================================
-- 1. ADMIN TABLE
-- ============================================
CREATE TABLE Admin (
    AdminID VARCHAR(10) PRIMARY KEY,
    Name VARCHAR(100) NOT NULL,
    Email VARCHAR(100) NOT NULL UNIQUE,
    Address VARCHAR(100),
    Password VARCHAR(255) NOT NULL
);

-- ============================================
-- 2. STAFF TABLE
-- ============================================
CREATE TABLE Staff (
    StaffID VARCHAR(10) PRIMARY KEY,
    Name VARCHAR(100) NOT NULL,
    Email VARCHAR(100) NOT NULL UNIQUE,
    Address VARCHAR(100),
    Password VARCHAR(255) NOT NULL,
    Status VARCHAR(15) DEFAULT 'Active',
    CONSTRAINT chk_staff_status CHECK (Status IN ('Active', 'Inactive'))
);

-- ============================================
-- 3. MENU TABLE
-- ============================================
CREATE TABLE Menu (
    MenuID VARCHAR(10) PRIMARY KEY,
    Menu_name VARCHAR(100) NOT NULL,
    Price DECIMAL(8,2) NOT NULL,
    Category VARCHAR(100) NOT NULL,
    Availability VARCHAR(255) DEFAULT 'Available',
    CONSTRAINT chk_menu_price CHECK (Price > 0),
    CONSTRAINT chk_menu_category CHECK (Category IN ('Food', 'Beverage', 'Dessert')),
    CONSTRAINT chk_menu_availability CHECK (Availability IN ('Available', 'Unavailable'))
);

-- ============================================
-- 4. TABLES TABLE (Restaurant Tables)
-- ============================================
CREATE TABLE Tables (
    TableID VARCHAR(10) PRIMARY KEY,
    Table_number VARCHAR(5) NOT NULL UNIQUE,
    Capacity INT NOT NULL,
    Status VARCHAR(15) DEFAULT 'Vacant',
    CONSTRAINT chk_table_capacity CHECK (Capacity >= 1),
    CONSTRAINT chk_table_status CHECK (Status IN ('Vacant', 'Occupied', 'Reserved'))
);

-- ============================================
-- 5. ORDER TABLE
-- ============================================
CREATE TABLE Orders (
    OrderID VARCHAR(15) PRIMARY KEY,
    TableID VARCHAR(10) NOT NULL,
    StaffID VARCHAR(10) NOT NULL,
    Total_amount DECIMAL(10,2) DEFAULT 0.00,
    Order_status VARCHAR(20) DEFAULT 'Active',
    Order_date DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (TableID) REFERENCES Tables(TableID),
    FOREIGN KEY (StaffID) REFERENCES Staff(StaffID),
    CONSTRAINT chk_order_status CHECK (Order_status IN ('Active', 'Completed', 'Cancelled'))
);

-- ============================================
-- 6. ORDER_ITEM TABLE
-- ============================================
CREATE TABLE Order_Item (
    Order_itemID VARCHAR(15) PRIMARY KEY,
    OrderID VARCHAR(15) NOT NULL,
    MenuID VARCHAR(10) NOT NULL,
    Quantity INT NOT NULL,
    Unit_price DECIMAL(8,2) NOT NULL,
    Total DECIMAL(8,2) NOT NULL,
    FOREIGN KEY (OrderID) REFERENCES Orders(OrderID),
    FOREIGN KEY (MenuID) REFERENCES Menu(MenuID),
    CONSTRAINT chk_quantity CHECK (Quantity >= 1)
);

-- ============================================
-- 7. BILL TABLE
-- ============================================
CREATE TABLE Bill (
    BillID VARCHAR(15) PRIMARY KEY,
    OrderID VARCHAR(15) NOT NULL UNIQUE,
    StaffID VARCHAR(10) NOT NULL,
    Bill_date DATETIME DEFAULT CURRENT_TIMESTAMP,
    Total DECIMAL(10,2) NOT NULL,
    Payment_method VARCHAR(20) NOT NULL,
    Payment_status VARCHAR(15) DEFAULT 'Unpaid',
    FOREIGN KEY (OrderID) REFERENCES Orders(OrderID),
    FOREIGN KEY (StaffID) REFERENCES Staff(StaffID),
    CONSTRAINT chk_payment_method CHECK (Payment_method IN ('Cash', 'Card', 'E-Wallet')),
    CONSTRAINT chk_payment_status CHECK (Payment_status IN ('Paid', 'Unpaid'))
);

-- ============================================
-- 8. DAILY_SELLS TABLE
-- ============================================
CREATE TABLE Daily_Sells (
    SalesID VARCHAR(15) PRIMARY KEY,
    Sales_date DATE NOT NULL,
    Daily_total DECIMAL(10,2) NOT NULL
);

-- ============================================
-- TRIGGERS
-- ============================================

-- Trigger to update Order Total when Order_Item is added
DELIMITER //
CREATE TRIGGER update_order_total_insert
AFTER INSERT ON Order_Item
FOR EACH ROW
BEGIN
    UPDATE Orders 
    SET Total_amount = (
        SELECT COALESCE(SUM(Total), 0) 
        FROM Order_Item 
        WHERE OrderID = NEW.OrderID
    )
    WHERE OrderID = NEW.OrderID;
END//
DELIMITER ;

-- Trigger to update Order Total when Order_Item is deleted
DELIMITER //
CREATE TRIGGER update_order_total_delete
AFTER DELETE ON Order_Item
FOR EACH ROW
BEGIN
    UPDATE Orders 
    SET Total_amount = (
        SELECT COALESCE(SUM(Total), 0) 
        FROM Order_Item 
        WHERE OrderID = OLD.OrderID
    )
    WHERE OrderID = OLD.OrderID;
END//
DELIMITER ;

-- Trigger to set table to Vacant when bill is paid
DELIMITER //
CREATE TRIGGER set_table_vacant_on_payment
AFTER UPDATE ON Bill
FOR EACH ROW
BEGIN
    IF NEW.Payment_status = 'Paid' AND OLD.Payment_status = 'Unpaid' THEN
        UPDATE Tables t
        INNER JOIN Orders o ON t.TableID = o.TableID
        SET t.Status = 'Vacant'
        WHERE o.OrderID = NEW.OrderID;
        
        UPDATE Orders SET Order_status = 'Completed' WHERE OrderID = NEW.OrderID;
    END IF;
END//
DELIMITER ;

-- ============================================
-- SAMPLE DATA
-- ============================================

-- Insert Admin
INSERT INTO Admin (AdminID, Name, Email, Address, Password) VALUES
('ADM001', 'System Administrator', 'admin@restaurant.com', '123 Admin Street', 'admin123'),
('ADM002', 'Manager One', 'manager@restaurant.com', '456 Manager Ave', 'manager123');

-- Insert Staff
INSERT INTO Staff (StaffID, Name, Email, Address, Password, Status) VALUES
('STF001', 'John Smith', 'john@restaurant.com', '789 Staff Road', 'john123', 'Active'),
('STF002', 'Jane Doe', 'jane@restaurant.com', '321 Worker Lane', 'jane123', 'Active'),
('STF003', 'Bob Wilson', 'bob@restaurant.com', '654 Employee St', 'bob123', 'Active'),
('STF004', 'Alice Brown', 'alice@restaurant.com', '987 Helper Ave', 'alice123', 'Inactive');

-- Insert Menu Items
INSERT INTO Menu (MenuID, Menu_name, Price, Category, Availability) VALUES
-- Food Items
('MNU001', 'Nasi Lemak Special', 12.50, 'Food', 'Available'),
('MNU002', 'Mee Goreng Mamak', 10.00, 'Food', 'Available'),
('MNU003', 'Chicken Chop', 18.50, 'Food', 'Available'),
('MNU004', 'Fish and Chips', 22.00, 'Food', 'Available'),
('MNU005', 'Nasi Goreng Kampung', 11.00, 'Food', 'Available'),
('MNU006', 'Spaghetti Carbonara', 16.50, 'Food', 'Available'),
('MNU007', 'Grilled Lamb Chop', 35.00, 'Food', 'Unavailable'),
-- Beverages
('MNU008', 'Teh Tarik', 3.50, 'Beverage', 'Available'),
('MNU009', 'Kopi O', 2.50, 'Beverage', 'Available'),
('MNU010', 'Fresh Orange Juice', 6.00, 'Beverage', 'Available'),
('MNU011', 'Iced Lemon Tea', 4.50, 'Beverage', 'Available'),
('MNU012', 'Milo Dinosaur', 5.50, 'Beverage', 'Available'),
-- Desserts
('MNU013', 'Cendol', 5.00, 'Dessert', 'Available'),
('MNU014', 'Ice Cream Sundae', 8.50, 'Dessert', 'Available'),
('MNU015', 'Chocolate Brownie', 9.00, 'Dessert', 'Available');

-- Insert Tables
INSERT INTO Tables (TableID, Table_number, Capacity, Status) VALUES
('TBL001', 'T01', 2, 'Vacant'),
('TBL002', 'T02', 2, 'Vacant'),
('TBL003', 'T03', 4, 'Vacant'),
('TBL004', 'T04', 4, 'Vacant'),
('TBL005', 'T05', 6, 'Vacant'),
('TBL006', 'T06', 6, 'Vacant'),
('TBL007', 'T07', 8, 'Vacant'),
('TBL008', 'T08', 8, 'Reserved'),
('TBL009', 'T09', 10, 'Vacant'),
('TBL010', 'T10', 12, 'Vacant');

-- ============================================
-- USEFUL VIEWS
-- ============================================

-- View for active orders with details
CREATE VIEW vw_active_orders AS
SELECT 
    o.OrderID,
    t.Table_number,
    s.Name AS Staff_Name,
    o.Total_amount,
    o.Order_date,
    o.Order_status
FROM Orders o
JOIN Tables t ON o.TableID = t.TableID
JOIN Staff s ON o.StaffID = s.StaffID
WHERE o.Order_status = 'Active';

-- View for menu availability
CREATE VIEW vw_available_menu AS
SELECT MenuID, Menu_name, Price, Category
FROM Menu
WHERE Availability = 'Available'
ORDER BY Category, Menu_name;

-- View for daily sales summary
CREATE VIEW vw_daily_sales_summary AS
SELECT 
    DATE(b.Bill_date) AS Sale_Date,
    COUNT(b.BillID) AS Total_Bills,
    SUM(b.Total) AS Total_Revenue
FROM Bill b
WHERE b.Payment_status = 'Paid'
GROUP BY DATE(b.Bill_date)
ORDER BY Sale_Date DESC;

SELECT 'Database created successfully!' AS Status;

