# 🔧 How The Restaurant Management System Code Works

## 📚 Table of Contents
1. [System Architecture](#system-architecture)
2. [Database Connection](#database-connection)
3. [Module Structure](#module-structure)
4. [ID Generation](#id-generation)
5. [Order Processing Flow](#order-processing-flow)
6. [Security Features](#security-features)
7. [Key Functions Explained](#key-functions-explained)

---

## 🏗️ System Architecture

### Overall Structure
```
main.cpp (Entry Point)
    ↓
DatabaseConnection (Handles MySQL)
    ↓
┌─────────────────────────────────────┐
│         Module Classes              │
├─────────────────────────────────────┤
│ AdminModule    │ StaffModule        │
│ MenuModule     │ TableModule        │
│ OrderModule    │ BillingModule      │
└─────────────────────────────────────┘
    ↓
MySQL Database (restaurant_management)
```

### How It Works:
1. **main.cpp** starts the program
2. Creates a **DatabaseConnection** object
3. Initializes all **Module** objects
4. Shows login menu
5. Based on user role, shows appropriate dashboard
6. Each module handles its own operations

---

## 🔌 Database Connection

### DatabaseConnection Class

**Purpose:** Manages all MySQL database operations

**Key Functions:**

```cpp
bool connect()
```
- Establishes connection to MySQL server
- Sets the database schema
- Returns true if successful

```cpp
unique_ptr<ResultSet> executeQuery(string query)
```
- Executes SELECT queries
- Returns result set for reading data
- Example: `SELECT * FROM Staff`

```cpp
int executeUpdate(string query)
```
- Executes INSERT, UPDATE, DELETE queries
- Returns number of rows affected
- Example: `INSERT INTO Staff VALUES (...)`

```cpp
unique_ptr<PreparedStatement> prepareStatement(string query)
```
- Creates prepared statements (prevents SQL injection)
- Uses placeholders: `SELECT * FROM Staff WHERE StaffID = ?`
- Safer than string concatenation

**How It Works:**
1. Uses MySQL Connector/C++ library
2. Connects to `localhost:3306`
3. Uses database `restaurant_management`
4. All modules share the same connection

---

## 📦 Module Structure

### Each Module Follows This Pattern:

```cpp
class ModuleName {
private:
    DatabaseConnection& db;  // Reference to shared DB connection
    
    string generateID() {
        // Finds gaps in IDs to reuse deleted IDs
        // Returns next available ID
    }
    
public:
    // View functions
    void viewAll() { ... }
    
    // Add functions
    bool add(...) { ... }
    
    // Update functions
    bool update(...) { ... }
    
    // Delete functions
    bool delete(...) { ... }
}
```

### Example: StaffModule

**1. View All Staff:**
```cpp
void viewAllStaff() {
    // Execute SQL query
    auto res = db.executeQuery("SELECT * FROM Staff");
    
    // Loop through results
    while (res->next()) {
        // Display each staff member
        cout << res->getString("Name") << endl;
    }
}
```

**2. Add New Staff:**
```cpp
bool addStaff(name, email, address, password) {
    // 1. Generate new ID
    string staffID = generateStaffID();
    
    // 2. Check if email exists
    // 3. Insert into database
    auto pstmt = db.prepareStatement(
        "INSERT INTO Staff VALUES (?, ?, ?, ?, ?, 'Active')");
    pstmt->setString(1, staffID);
    // ... set other values
    pstmt->executeUpdate();
    
    return true;
}
```

**3. Update Staff Status:**
```cpp
bool updateStaffStatus(staffID, newStatus) {
    // Validate status
    if (status != "Active" && status != "Inactive") {
        return false;
    }
    
    // Update database
    auto pstmt = db.prepareStatement(
        "UPDATE Staff SET Status = ? WHERE StaffID = ?");
    pstmt->setString(1, newStatus);
    pstmt->setString(2, staffID);
    pstmt->executeUpdate();
    
    return true;
}
```

---

## 🔢 ID Generation (Gap-Filling Algorithm)

### How It Works:

**Problem:** When you delete STF001, you want to reuse it for new staff.

**Solution:** Find the smallest missing ID.

**SQL Query Explained:**
```sql
SELECT t1.num + 1 AS gap
FROM (SELECT CAST(SUBSTRING(StaffID, 4) AS UNSIGNED) AS num FROM Staff) t1
LEFT JOIN (SELECT CAST(SUBSTRING(StaffID, 4) AS UNSIGNED) AS num FROM Staff) t2
ON t1.num + 1 = t2.num
WHERE t2.num IS NULL
ORDER BY gap LIMIT 1
```

**Step-by-Step:**
1. **t1**: Gets all existing IDs (e.g., 2, 4, 5)
2. **t2**: Same list, but for comparison
3. **LEFT JOIN**: For each ID in t1, check if (ID + 1) exists in t2
4. **WHERE t2.num IS NULL**: If (ID + 1) doesn't exist, that's a gap!
5. **ORDER BY gap LIMIT 1**: Get the smallest gap

**Example:**
- Existing IDs: STF002, STF004, STF005
- t1 finds: 2, 4, 5
- Check gaps:
  - 2+1 = 3 → Not found → **GAP FOUND!**
  - 4+1 = 5 → Found → No gap
  - 5+1 = 6 → Not found → Gap, but 3 is smaller
- **Result: STF003** (reuses deleted ID)

**If No Gaps:**
- Get MAX(ID) + 1
- Example: Max is STF005 → New ID is STF006

---

## 🛒 Order Processing Flow

### Complete Workflow:

```
1. Staff Login
   ↓
2. Create Order
   ├─ Select Table
   ├─ Check table status (must be Occupied/Reserved)
   ├─ Check if table has active order
   ├─ Create order record (OrderID generated)
   └─ Set table to "Occupied"
   ↓
3. Add Items to Order
   ├─ Select Menu Item
   ├─ Check availability
   ├─ Enter quantity
   ├─ Get current price
   ├─ Calculate: Total = Quantity × Unit_Price
   ├─ Insert into Order_Item table
   └─ Trigger updates Order.Total_amount automatically
   ↓
4. View Order
   ├─ Display order details
   ├─ List all items
   └─ Show total amount
   ↓
5. Generate Bill
   ├─ Check order is active
   ├─ Check order has items
   ├─ Select payment method
   ├─ Create bill record
   └─ Bill.Total = Order.Total_amount
   ↓
6. Process Payment
   ├─ Update Bill.Payment_status = 'Paid'
   ├─ Trigger fires:
   │  ├─ Sets Table.Status = 'Vacant'
   │  └─ Sets Order.Order_status = 'Completed'
   └─ Done!
```

### Key Code in OrderModule:

**Create Order:**
```cpp
string createOrder(tableID, staffID) {
    // 1. Validate table
    // 2. Check for existing active order
    // 3. Generate OrderID
    // 4. Insert into Orders table
    // 5. Update table status
    return orderID;
}
```

**Add Item:**
```cpp
bool addOrderItem(orderID, menuID, quantity) {
    // 1. Get menu price
    // 2. Calculate total
    // 3. Insert into Order_Item
    // 4. Trigger automatically updates Order.Total_amount
    return true;
}
```

**Database Trigger (in SQL):**
```sql
CREATE TRIGGER update_order_total_insert
AFTER INSERT ON Order_Item
FOR EACH ROW
BEGIN
    UPDATE Orders 
    SET Total_amount = (
        SELECT SUM(Total) FROM Order_Item 
        WHERE OrderID = NEW.OrderID
    )
    WHERE OrderID = NEW.OrderID;
END
```

---

## 🔒 Security Features

### 1. Password Hiding

**Function:**
```cpp
string getHiddenPassword() {
    string password = "";
    char ch;
    
    while (true) {
        ch = _getch();  // Get char without displaying
        
        if (ch == 13) break;  // Enter key
        if (ch == 8) {         // Backspace
            password.pop_back();
            cout << "\b \b";  // Erase asterisk
        }
        else {
            password += ch;
            cout << '*';  // Show asterisk instead
        }
    }
    return password;
}
```

**How It Works:**
- `_getch()` reads character without echoing
- Displays `*` instead of actual character
- Supports backspace to correct mistakes
- Returns actual password string

### 2. Password Confirmation

**Function:**
```cpp
string getPasswordWithConfirmation() {
    do {
        password1 = getHiddenPassword();
        password2 = getHiddenPassword();
        
        if (password1 != password2) {
            cout << "Passwords don't match!";
        }
    } while (password1 != password2);
    
    return password1;
}
```

**How It Works:**
- Asks for password twice
- Compares both inputs
- Loops until they match
- Validates minimum length

### 3. Prepared Statements (SQL Injection Prevention)

**Instead of:**
```cpp
string query = "SELECT * FROM Staff WHERE Email = '" + email + "'";
// DANGEROUS! Vulnerable to SQL injection
```

**We Use:**
```cpp
auto pstmt = db.prepareStatement("SELECT * FROM Staff WHERE Email = ?");
pstmt->setString(1, email);
// SAFE! Parameters are escaped automatically
```

---

## 🎨 UI Features

### Screen Clearing

**Function:**
```cpp
void clearScreen() {
    system("cls");  // Windows command to clear console
}
```

**When Used:**
- Before showing menus
- After pressing Enter to continue
- Creates clean, professional interface

**Flow:**
```
Show Menu → User Selects → Show Result → Press Enter → Clear Screen → Show Menu Again
```

---

## 🔄 Data Flow Example: Adding Staff

```
User Input:
├─ Name: "John Doe"
├─ Email: "john@restaurant.com"
├─ Address: "123 Street"
└─ Password: "****" (hidden)

↓

main.cpp:
├─ Calls: staffModule->addStaff(...)
└─ Passes all parameters

↓

StaffModule::addStaff():
├─ 1. Check email exists
│  └─ Query: SELECT StaffID FROM Staff WHERE Email = ?
│
├─ 2. Generate StaffID
│  └─ Query: Find gap in IDs (e.g., STF003)
│
├─ 3. Insert into database
│  └─ Query: INSERT INTO Staff VALUES (?, ?, ?, ?, ?, 'Active')
│  └─ Parameters: STF003, "John Doe", "john@restaurant.com", ...
│
└─ 4. Return success

↓

Database:
└─ New record inserted:
   StaffID: STF003
   Name: John Doe
   Email: john@restaurant.com
   Status: Active

↓

User Sees:
└─ "[SUCCESS] Staff added with ID: STF003"
```

---

## 🎯 Key Design Patterns

### 1. **Singleton-like Database Connection**
- One connection shared by all modules
- Created once in main()
- Passed by reference to all modules

### 2. **Module Pattern**
- Each module handles one entity type
- Encapsulates all related operations
- Clean separation of concerns

### 3. **RAII (Resource Acquisition Is Initialization)**
- Uses `unique_ptr` for automatic memory management
- Database connections closed automatically
- No memory leaks

### 4. **Prepared Statements**
- Reusable query templates
- Parameter binding
- SQL injection prevention

---

## 📊 Database Triggers (Automatic Actions)

### Trigger 1: Update Order Total
```sql
AFTER INSERT ON Order_Item
→ Automatically recalculates Order.Total_amount
```

### Trigger 2: Set Table Vacant on Payment
```sql
AFTER UPDATE ON Bill (when Payment_status = 'Paid')
→ Sets Table.Status = 'Vacant'
→ Sets Order.Order_status = 'Completed'
```

**Why Triggers?**
- Ensures data consistency
- Automatic calculations
- No manual updates needed
- Business rules enforced at database level

---

## 🧪 Testing Flow

### To Test Order Processing:

1. **Login as Staff**
   - Email: `john@restaurant.com`
   - Password: `john123`

2. **Set Table to Occupied**
   - Admin → Table Management → Update Status → Occupied

3. **Create Order**
   - Staff → Create New Order
   - Select table (e.g., TBL001)
   - Order created: ORD000001

4. **Add Items**
   - Select menu item (e.g., MNU001 - Nasi Lemak)
   - Quantity: 2
   - Total updates automatically

5. **Generate Bill**
   - Staff → Generate Bill
   - Select payment method: Cash
   - Bill created: BIL000001

6. **Process Payment**
   - Staff → Process Payment
   - Bill marked as Paid
   - Table automatically becomes Vacant
   - Order marked as Completed

---

## 💡 Important Concepts

### 1. **Foreign Keys**
- Order.TableID → References Tables.TableID
- Order.StaffID → References Staff.StaffID
- Ensures data integrity

### 2. **Transactions** (Implicit)
- Each operation is atomic
- Either succeeds completely or fails completely
- No partial updates

### 3. **Error Handling**
- Try-catch blocks around database operations
- User-friendly error messages
- Graceful failure handling

### 4. **Input Validation**
- Check email uniqueness
- Validate status values
- Check business rules (e.g., can't delete staff with orders)

---

## 🎓 Summary

**The system works by:**
1. **Connecting** to MySQL database
2. **Organizing** code into modules (one per entity)
3. **Using** prepared statements for safety
4. **Generating** IDs intelligently (gap-filling)
5. **Enforcing** business rules at multiple levels
6. **Automating** calculations with triggers
7. **Providing** clean UI with screen clearing
8. **Securing** passwords with hidden input

**Key Strength:** Modular design makes it easy to understand, maintain, and extend!

