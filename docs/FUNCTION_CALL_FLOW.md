# 📞 Function Call Flow - Where Functions Are Called

This document shows **exactly where each function is called** in the code.

---

## 🎯 Main Entry Point

### `main()` Function (Line 720)
**Called by:** Operating System (program starts here)

**Calls:**
```cpp
// Line 728: Database connection
db.connect()

// Lines 739-744: Initialize all modules
new AdminModule(db)
new StaffModule(db)
new MenuModule(db)
new TableModule(db)
new OrderModule(db)
new BillingModule(db)

// Line 752: Show login menu
showLoginMenu()

// Lines 764, 779: Login functions
adminModule->login(email, password)
staffModule->login(email, password)

// Lines 765, 780: Show dashboards
adminDashboard()
staffDashboard()

// Line 802: Disconnect database
db.disconnect()
```

---

## 🔐 Login Functions

### `showLoginMenu()` (Line 104)
**Called by:** `main()` at line 752

**Calls:**
```cpp
// Line 105: Print header
printHeader("RESTAURANT MANAGEMENT SYSTEM")
```

### `adminModule->login()` (Line 764)
**Called by:** `main()` at line 764

**Location:** `include/AdminModule.h`

**Calls internally:**
```cpp
db.prepareStatement()  // Prepare SQL query
pstmt->setString()     // Set parameters
pstmt->executeQuery()  // Execute query
```

### `staffModule->login()` (Line 779)
**Called by:** `main()` at line 779

**Location:** `include/StaffModule.h`

**Calls internally:**
```cpp
db.prepareStatement()
pstmt->executeQuery()
```

---

## 👨‍💼 Admin Dashboard Functions

### `adminDashboard()` (Line 389)
**Called by:** `main()` at line 765 (after admin login)

**Calls:**
```cpp
// Line 393: Show menu
showAdminMainMenu()

// Lines 399-405: Management functions
adminStaffManagement()
adminMenuManagement()
adminTableManagement()

// Line 408: View orders
orderModule->viewAllOrders()

// Line 412: View bills
billingModule->viewAllBills()

// Line 428: Generate sales report
adminModule->generateDailySalesReport(date)

// Line 433: View sales history
adminModule->viewAllDailySales()

// Line 437: Logout
adminModule->logout()
```

### `showAdminMainMenu()` (Line 120)
**Called by:** `adminDashboard()` at line 393

**Calls:**
```cpp
// Line 121: Print header
printHeader("ADMIN DASHBOARD - Welcome, " + adminModule->getAdminName())

// Line 121: Get admin name
adminModule->getAdminName()
```

---

## 👥 Staff Management Functions

### `adminStaffManagement()` (Line 134)
**Called by:** `adminDashboard()` at line 399

**Calls:**
```cpp
// Line 138: Clear screen
clearScreen()

// Line 138: Print header
printHeader("STAFF MANAGEMENT")

// Line 152: View all staff
staffModule->viewAllStaff()

// Line 156: View active staff
staffModule->viewActiveStaff()

// Line 168: Get password with confirmation
getPasswordWithConfirmation()

// Line 169: Add new staff
staffModule->addStaff(name, email, address, password)

// Line 174: View all staff (before update)
staffModule->viewAllStaff()

// Line 180: Update staff status
staffModule->updateStaffStatus(staffID, status)

// Line 185: View all staff (before delete)
staffModule->viewAllStaff()

// Line 189: Delete staff
staffModule->deleteStaff(staffID)

// Line 153, 157, 171, 181, 190: Wait for user
pressEnterToContinue()
```

### `getPasswordWithConfirmation()` (Line 75)
**Called by:** `adminStaffManagement()` at line 168

**Calls:**
```cpp
// Line 80: Get hidden password (first time)
getHiddenPassword()

// Line 89: Get hidden password (confirmation)
getHiddenPassword()
```

### `getHiddenPassword()` (Line 48)
**Called by:** 
- `getPasswordWithConfirmation()` at lines 80, 89
- `main()` at lines 762, 777 (for login)

**Calls:**
```cpp
// Line 53: Get character without echo
_getch()  // Windows function
```

---

## 🍔 Menu Management Functions

### `adminMenuManagement()` (Line 201)
**Called by:** `adminDashboard()` at line 402

**Calls:**
```cpp
// Line 204: Clear screen
clearScreen()

// Line 205: Print header
printHeader("MENU MANAGEMENT")

// Line 222: View all menu
menuModule->viewAllMenu()

// Line 226: View available menu
menuModule->viewAvailableMenu()

// Line 233: View by category
menuModule->viewMenuByCategory(category)

// Line 241: Search menu
menuModule->searchMenu(searchTerm)

// Line 255: Add menu item
menuModule->addMenuItem(name, price, category)

// Line 260: View all menu (before update)
menuModule->viewAllMenu()

// Line 268: Update menu price
menuModule->updateMenuPrice(menuID, newPrice)

// Line 273: View all menu (before update)
menuModule->viewAllMenu()

// Line 279: Update availability
menuModule->updateMenuAvailability(menuID, availability)

// Line 284: View all menu (before delete)
menuModule->viewAllMenu()

// Line 288: Delete menu item
menuModule->deleteMenuItem(menuID)
```

---

## 🪑 Table Management Functions

### `adminTableManagement()` (Line 300)
**Called by:** `adminDashboard()` at line 405

**Calls:**
```cpp
// Line 303: Clear screen
clearScreen()

// Line 304: Print header
printHeader("TABLE MANAGEMENT")

// Line 321: View all tables
tableModule->viewAllTables()

// Line 325: View vacant tables
tableModule->viewTablesByStatus("Vacant")

// Line 329: View occupied tables
tableModule->viewTablesByStatus("Occupied")

// Line 333: View reserved tables
tableModule->viewTablesByStatus("Reserved")

// Line 344: Add new table
tableModule->addTable(tableNumber, capacity)

// Line 349: View all tables (before update)
tableModule->viewAllTables()

// Line 355: Update table status
tableModule->updateTableStatus(tableID, status)

// Line 360: View all tables (before update)
tableModule->viewAllTables()

// Line 368: Update table capacity
tableModule->updateTableCapacity(tableID, capacity)

// Line 373: View all tables (before delete)
tableModule->viewAllTables()

// Line 377: Delete table
tableModule->deleteTable(tableID)
```

---

## 👨‍🍳 Staff Dashboard Functions

### `staffDashboard()` (Line 644)
**Called by:** `main()` at line 780 (after staff login)

**Calls:**
```cpp
// Line 647: Clear screen
clearScreen()

// Line 648: Show menu
showStaffMainMenu()

// Line 654: View available menu
menuModule->viewAvailableMenu()

// Line 658: View all tables
tableModule->viewAllTables()

// Line 662: Create new order
staffCreateOrder()

// Line 666: Add items to order
staffAddItemsToOrder()

// Line 673: View order details
orderModule->viewOrderDetails(orderID)

// Line 678: View active orders
orderModule->viewActiveOrders()

// Line 691: Cancel order
orderModule->cancelOrder(orderID)

// Line 697: Generate bill
staffGenerateBill()

// Line 701: Process payment
staffProcessPayment()

// Line 705: View unpaid bills
billingModule->viewUnpaidBills()

// Line 709: Logout
staffModule->logout()
```

### `showStaffMainMenu()` (Line 448)
**Called by:** `staffDashboard()` at line 648

**Calls:**
```cpp
// Line 449: Print header
printHeader("STAFF DASHBOARD - Welcome, " + staffModule->getStaffName())

// Line 449: Get staff name
staffModule->getStaffName()
```

---

## 📝 Order Processing Functions

### `staffCreateOrder()` (Line 465)
**Called by:** `staffDashboard()` at line 662

**Calls:**
```cpp
// Line 466: View all tables
tableModule->viewAllTables()

// Line 474: Get table status
tableModule->getTableStatus(tableID)

// Line 482: Update table status
tableModule->updateTableStatus(tableID, "Occupied")

// Line 486: Check for active order
tableModule->hasActiveOrder(tableID)

// Line 488: Get active order ID
orderModule->getActiveOrderForTable(tableID)

// Line 495: Create order
orderModule->createOrder(tableID, staffModule->getStaffID())

// Line 495: Get staff ID
staffModule->getStaffID()

// Line 505: View available menu
menuModule->viewAvailableMenu()

// Line 518: Add item to order
orderModule->addOrderItem(orderID, menuID, quantity)

// Line 526: View order details
orderModule->viewOrderDetails(orderID)
```

### `staffAddItemsToOrder()` (Line 531)
**Called by:** `staffDashboard()` at line 666

**Calls:**
```cpp
// Line 532: View active orders
orderModule->viewActiveOrders()

// Line 539: Check if order is active
orderModule->isOrderActive(orderID)

// Line 544: View available menu
menuModule->viewAvailableMenu()

// Line 557: Add item to order
orderModule->addOrderItem(orderID, menuID, quantity)

// Line 565: View order details
orderModule->viewOrderDetails(orderID)
```

---

## 💰 Billing Functions

### `staffGenerateBill()` (Line 568)
**Called by:** `staffDashboard()` at line 697

**Calls:**
```cpp
// Line 569: View active orders
orderModule->viewActiveOrders()

// Line 576: Check if order is active
orderModule->isOrderActive(orderID)

// Line 582: Get order total
orderModule->getOrderTotal(orderID)

// Line 608: Generate bill
billingModule->generateBill(orderID, staffModule->getStaffID(), paymentMethod)

// Line 608: Get staff ID
staffModule->getStaffID()

// Line 611: View bill details
billingModule->viewBillDetails(billID)

// Line 619: Process payment
billingModule->processPayment(billID)
```

### `staffProcessPayment()` (Line 624)
**Called by:** `staffDashboard()` at line 701

**Calls:**
```cpp
// Line 625: View unpaid bills
billingModule->viewUnpaidBills()

// Line 632: View bill details
billingModule->viewBillDetails(billID)

// Line 640: Process payment
billingModule->processPayment(billID)
```

---

## 🛠️ Utility Functions

### `clearScreen()` (Line 31)
**Called by:**
- `pressEnterToContinue()` at line 38
- `adminStaffManagement()` at line 137
- `adminMenuManagement()` at line 204
- `adminTableManagement()` at line 303
- `adminDashboard()` at line 392
- `staffDashboard()` at line 647
- `main()` at line 751

### `clearInputBuffer()` (Line 26)
**Called by:**
- `showLoginMenu()` at line 113
- All menu functions after `cin >> choice`
- Used throughout to clear input buffer

### `pressEnterToContinue()` (Line 35)
**Called by:**
- `main()` at lines 734, 767, 782
- `adminStaffManagement()` at lines 153, 157, 171, 181, 190
- `adminMenuManagement()` at lines 223, 227, 234, 242, 256, 269, 280, 289
- `adminTableManagement()` at lines 322, 326, 330, 334, 345, 356, 369, 378
- `adminDashboard()` at lines 409, 413, 429, 434
- `staffDashboard()` at lines 655, 659, 663, 667, 674, 679, 693, 698, 702, 706

**Calls:**
```cpp
// Line 38: Clear screen
clearScreen()
```

### `printHeader()` (Line 41)
**Called by:**
- `showLoginMenu()` at line 105
- `showAdminMainMenu()` at line 121
- `showStaffMainMenu()` at line 449
- `adminStaffManagement()` at line 138
- `adminMenuManagement()` at line 205
- `adminTableManagement()` at line 304

---

## 📊 Module Function Calls (Internal)

### StaffModule Functions Called from main.cpp:

| Function | Called At | Purpose |
|----------|-----------|---------|
| `login()` | Line 779 | Staff authentication |
| `getStaffID()` | Lines 495, 608 | Get current staff ID |
| `getStaffName()` | Line 449 | Get current staff name |
| `viewAllStaff()` | Lines 152, 174, 185 | Display all staff |
| `viewActiveStaff()` | Line 156 | Display active staff only |
| `addStaff()` | Line 169 | Add new staff member |
| `updateStaffStatus()` | Line 180 | Change staff status |
| `deleteStaff()` | Line 189 | Remove staff member |
| `logout()` | Line 709 | Staff logout |

### MenuModule Functions Called from main.cpp:

| Function | Called At | Purpose |
|----------|-----------|---------|
| `viewAllMenu()` | Line 222, 260, 273, 284 | Display all menu items |
| `viewAvailableMenu()` | Lines 226, 505, 544, 654 | Display available items |
| `viewMenuByCategory()` | Line 233 | Filter by category |
| `searchMenu()` | Line 241 | Search menu items |
| `addMenuItem()` | Line 255 | Add new menu item |
| `updateMenuPrice()` | Line 268 | Change menu price |
| `updateMenuAvailability()` | Line 279 | Change availability |
| `deleteMenuItem()` | Line 288 | Remove menu item |

### TableModule Functions Called from main.cpp:

| Function | Called At | Purpose |
|----------|-----------|---------|
| `viewAllTables()` | Lines 321, 349, 360, 373, 466, 658 | Display all tables |
| `viewTablesByStatus()` | Lines 325, 329, 333 | Filter by status |
| `addTable()` | Line 344 | Add new table |
| `updateTableStatus()` | Lines 355, 482 | Change table status |
| `updateTableCapacity()` | Line 368 | Change table capacity |
| `deleteTable()` | Line 377 | Remove table |
| `getTableStatus()` | Line 474 | Get current status |
| `hasActiveOrder()` | Line 486 | Check for active order |

### OrderModule Functions Called from main.cpp:

| Function | Called At | Purpose |
|----------|-----------|---------|
| `createOrder()` | Line 495 | Create new order |
| `addOrderItem()` | Lines 518, 557 | Add item to order |
| `viewOrderDetails()` | Lines 526, 565, 673 | Display order info |
| `viewActiveOrders()` | Lines 532, 569, 678, 682 | Show active orders |
| `viewAllOrders()` | Line 408 | Show all orders |
| `cancelOrder()` | Line 691 | Cancel an order |
| `getOrderTotal()` | Line 582 | Get order total amount |
| `isOrderActive()` | Lines 539, 576 | Check order status |
| `getActiveOrderForTable()` | Line 488 | Get order for table |

### BillingModule Functions Called from main.cpp:

| Function | Called At | Purpose |
|----------|-----------|---------|
| `generateBill()` | Line 608 | Create bill for order |
| `processPayment()` | Lines 619, 640 | Mark bill as paid |
| `viewBillDetails()` | Lines 611, 632 | Display bill info |
| `viewAllBills()` | Line 412 | Show all bills |
| `viewUnpaidBills()` | Lines 625, 705 | Show unpaid bills |

### AdminModule Functions Called from main.cpp:

| Function | Called At | Purpose |
|----------|-----------|---------|
| `login()` | Line 764 | Admin authentication |
| `getAdminName()` | Line 121 | Get current admin name |
| `generateDailySalesReport()` | Line 428 | Create sales report |
| `viewAllDailySales()` | Line 433 | Show sales history |
| `logout()` | Line 437 | Admin logout |

---

## 🔄 Complete Function Call Chain Example

### Example: Adding a New Staff Member

```
1. main() [Line 720]
   ↓
2. showLoginMenu() [Line 752]
   ↓ calls printHeader()
   ↓
3. User selects "Admin Login"
   ↓
4. adminModule->login() [Line 764]
   ↓ calls getHiddenPassword()
   ↓
5. adminDashboard() [Line 765]
   ↓
6. User selects "Staff Management"
   ↓
7. adminStaffManagement() [Line 399]
   ↓ calls clearScreen()
   ↓ calls printHeader()
   ↓
8. User selects "Add New Staff"
   ↓
9. getPasswordWithConfirmation() [Line 168]
   ↓ calls getHiddenPassword() [twice]
   ↓
10. staffModule->addStaff() [Line 169]
    ↓ (inside StaffModule)
    ↓ calls generateStaffID()
    ↓ calls db.prepareStatement()
    ↓ calls pstmt->executeUpdate()
    ↓
11. pressEnterToContinue() [Line 171]
    ↓ calls clearScreen()
```

---

## 📍 Key Function Call Locations Summary

### Most Called Functions:
1. **`pressEnterToContinue()`** - Called 30+ times (after every action)
2. **`clearScreen()`** - Called 10+ times (before menus)
3. **`printHeader()`** - Called 10+ times (for menu headers)
4. **`clearInputBuffer()`** - Called 20+ times (after input)

### Module Functions Most Used:
1. **`viewAllStaff()`** - Called 3 times
2. **`viewAllMenu()`** - Called 4 times
3. **`viewAllTables()`** - Called 6 times
4. **`viewActiveOrders()`** - Called 4 times

---

## 🎯 Function Call Pattern

**Typical Pattern:**
```
User Action
  ↓
Menu Function (e.g., adminStaffManagement)
  ↓
clearScreen() → printHeader() → Show Menu
  ↓
User Selects Option
  ↓
Module Function (e.g., staffModule->addStaff())
  ↓
Module calls DatabaseConnection functions
  ↓
pressEnterToContinue() → clearScreen()
  ↓
Back to Menu
```

---

This shows **exactly where every function is called** in your code! 🎯

