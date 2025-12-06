# 🍽️ Restaurant Management System

A comprehensive Restaurant Management System built with **C++** and **MySQL** database.

## 📋 Features

### Admin Module
- ✅ Admin login and authentication
- ✅ Staff management (Add/Edit/Delete/Activate/Deactivate)
- ✅ Menu management (Add/Edit/Delete/Update prices)
- ✅ Table management
- ✅ View all orders and bills
- ✅ Generate daily sales reports
- ✅ View sales history

### Staff Module
- ✅ Staff login with access control
- ✅ View available menu items
- ✅ Create and manage orders
- ✅ Add items to orders
- ✅ Generate bills & process payments (combined workflow)
- ✅ Process payments for unpaid bills separately
- ✅ View unpaid bills

### Core Features
- 🔐 Secure login for Admin and Staff
- 📊 Real-time order tracking
- 🍔 Dynamic menu management
- 🪑 Table status management (Vacant/Occupied/Reserved)
- 💳 Multiple payment methods
- 📈 Daily sales reporting
- ⚡ Automatic order total calculation
- 🔄 Auto-update table status on payment

## 🛠️ Technology Stack

- **Language:** C++
- **Database:** MySQL
- **Connector:** MySQL Connector/C++
- **IDE:** Visual Studio 2022
- **Build:** x64 Release

## 📦 Database Schema

### Tables
- `Admin` - Administrator accounts
- `Staff` - Staff member accounts
- `Menu` - Restaurant menu items
- `Tables` - Restaurant table information
- `Orders` - Customer orders
- `Order_Item` - Order line items
- `Bill` - Billing information
- `Daily_Sells` - Daily sales records

## 🚀 Installation

### Prerequisites
1. **MySQL Server** (via XAMPP or standalone)
2. **Visual Studio 2022** (with C++ development tools)
3. **MySQL Connector/C++** (included in project)

### Database Setup
1. Start MySQL in XAMPP
2. Open phpMyAdmin or MySQL Workbench
3. Import `restaurant_database.sql`
4. Database `restaurant_management` will be created with sample data

### Build and Run
1. Open `restaurant testing.sln` in Visual Studio
2. Set configuration to **Release** and **x64**
3. Build solution (Ctrl+Shift+B)
4. Run the executable from `x64\Release\restaurant testing.exe`

## 🔑 Default Login Credentials

### Admin Login
| Email | Password |
|-------|----------|
| admin@restaurant.com | admin123 |
| manager@restaurant.com | manager123 |

### Staff Login
| Email | Password |
|-------|----------|
| john@restaurant.com | john123 |
| jane@restaurant.com | jane123 |
| bob@restaurant.com | bob123 |

### 🔒 Security Features
- ✅ Password input is **hidden** (displays `*` instead of characters)
- ✅ Password confirmation when creating new staff (asks twice)
- ✅ Minimum password length validation (4+ characters)
- ✅ Backspace support during password entry

## 📁 Project Structure

```
restaurant testing/
├── README.md                   # Project documentation
├── restaurant testing.sln      # Visual Studio solution
├── restaurant testing.vcxproj  # Visual Studio project
│
├── src/                        # Source files
│   ├── main.cpp               # Main application entry point
│   └── DatabaseConnection.cpp # Database implementation
│
├── include/                    # Header files
│   ├── DatabaseConnection.h   # Database connection
│   ├── AdminModule.h          # Admin management
│   ├── StaffModule.h          # Staff management
│   ├── MenuModule.h           # Menu management
│   ├── TableModule.h          # Table management
│   ├── OrderModule.h          # Order processing
│   └── BillingModule.h        # Billing & payment
│
├── database/                   # Database files
│   └── restaurant_database.sql # Schema and sample data
│
├── libs/                       # MySQL connector libraries
│   ├── mysqlcppconn-*.dll
│   ├── libcrypto-3-x64.dll
│   └── libssl-3-x64.dll
│
├── plugin/                     # MySQL authentication plugins
│   └── authentication_*.dll
│
├── docs/                       # Documentation & scripts
│   ├── GIT_SETUP_GUIDE.md
│   ├── UPLOAD_INSTRUCTIONS.md
│   └── *.bat (helper scripts)
│
└── x64/Release/               # Build output
    └── restaurant testing.exe
```

## 🎯 Business Rules Implemented

- Only Active staff can login and process orders
- Orders only for Occupied/Reserved tables
- One active order per table at a time
- Only available menu items can be ordered
- Table automatically sets to Vacant when bill is paid
- Price validation (must be > 0)
- Quantity validation (must be ≥ 1)
- Email uniqueness for Admin and Staff
- Secure password storage

## 💡 Usage Example

### Creating an Order
1. Staff logs in
2. View available tables
3. Set table to "Occupied"
4. Create new order
5. Add menu items with quantities
6. View order summary
7. Generate bill
8. Process payment
9. Table automatically becomes "Vacant"

### Admin Operations
1. Admin logs in
2. Manage staff accounts
3. Update menu items and prices
4. View all active orders
5. Generate daily sales report
6. View sales history

## 📊 Database Triggers

- Auto-update order total when items are added/removed
- Auto-set table to Vacant when bill is marked as Paid
- Auto-set order status to Completed on payment

## 🔧 Configuration

Database connection settings in `DatabaseConnection.h`:
```cpp
string server = "tcp://127.0.0.1:3306";
string username = "root";
string password = "";  // Change if you have MySQL password
string database = "restaurant_management";
```

## 🐛 Troubleshooting

**Cannot connect to database?**
- Ensure MySQL is running in XAMPP
- Check database name exists
- Verify connection settings in `DatabaseConnection.h`

**Build errors?**
- Ensure MySQL Connector DLLs are in the project folder
- Check project configuration is set to x64 Release
- Verify include and library paths in project settings

## 📝 License

This project is created for educational purposes as part of UTEM Workshop 1 coursework.

## 👤 Author

**snetchx**
- GitHub: [@snetchx](https://github.com/snetchx)

## 🙏 Acknowledgments

- UTEM Sem3 Workshop 1
- MySQL Connector/C++ documentation
- Visual Studio C++ community

---

**⭐ If you find this project useful, please star the repository!**
