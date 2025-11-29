#include "DatabaseConnection.h"

// Constructor
DatabaseConnection::DatabaseConnection() : driver(nullptr) {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        cout << "MySQL driver initialized successfully!" << endl;
    }
    catch (sql::SQLException& e) {
        cerr << "Error initializing MySQL driver: " << e.what() << endl;
        cerr << "Error code: " << e.getErrorCode() << endl;
    }
}

// Destructor
DatabaseConnection::~DatabaseConnection() {
    disconnect();
}

// Connect to database
bool DatabaseConnection::connect() {
    try {
        if (driver == nullptr) {
            cerr << "MySQL driver not initialized!" << endl;
            return false;
        }

        cout << "Attempting to connect to database..." << endl;
        cout << "Server: " << server << endl;
        cout << "Username: " << username << endl;
        cout << "Database: " << database << endl;

        // Create connection
        con.reset(driver->connect(server, username, password));

        if (con == nullptr) {
            cerr << "Failed to create connection object!" << endl;
            return false;
        }

        // Set schema (database)
        con->setSchema(database);

        cout << "Successfully connected to database: " << database << endl;
        return true;
    }
    catch (sql::SQLException& e) {
        cerr << "Database connection error: " << e.what() << endl;
        cerr << "Error code: " << e.getErrorCode() << endl;
        cerr << "SQLState: " << e.getSQLState() << endl;

        // Provide troubleshooting tips
        cout << "\nTroubleshooting steps:" << endl;
        cout << "1. Make sure MySQL is running in XAMPP" << endl;
        cout << "2. Check if the database '" << database << "' exists" << endl;
        cout << "3. Verify username and password" << endl;
        cout << "4. Check if MySQL is on port 3306" << endl;

        return false;
    }
}

// Disconnect from database
void DatabaseConnection::disconnect() {
    try {
        if (con != nullptr) {
            con->close();
            cout << "Database connection closed." << endl;
        }
    }
    catch (sql::SQLException& e) {
        cerr << "Error closing connection: " << e.what() << endl;
    }
}

// Check if connected
bool DatabaseConnection::isConnected() {
    try {
        return (con != nullptr && !con->isClosed());
    }
    catch (sql::SQLException& e) {
        cerr << "Error checking connection: " << e.what() << endl;
        return false;
    }
}

// Get connection object
sql::Connection* DatabaseConnection::getConnection() {
    return con.get();
}

// Execute query (SELECT)
unique_ptr<sql::ResultSet> DatabaseConnection::executeQuery(const string& query) {
    try {
        if (!isConnected()) {
            cerr << "Not connected to database!" << endl;
            return nullptr;
        }

        unique_ptr<sql::Statement> stmt(con->createStatement());
        return unique_ptr<sql::ResultSet>(stmt->executeQuery(query));
    }
    catch (sql::SQLException& e) {
        cerr << "Query execution error: " << e.what() << endl;
        cerr << "Error code: " << e.getErrorCode() << endl;
        cerr << "Failed query: " << query << endl;
        return nullptr;
    }
}

// Execute update (INSERT, UPDATE, DELETE)
int DatabaseConnection::executeUpdate(const string& query) {
    try {
        if (!isConnected()) {
            cerr << "Not connected to database!" << endl;
            return -1;
        }

        unique_ptr<sql::Statement> stmt(con->createStatement());
        return stmt->executeUpdate(query);
    }
    catch (sql::SQLException& e) {
        cerr << "Update execution error: " << e.what() << endl;
        cerr << "Error code: " << e.getErrorCode() << endl;
        cerr << "Failed query: " << query << endl;
        return -1;
    }
}

// Get prepared statement
unique_ptr<sql::PreparedStatement> DatabaseConnection::prepareStatement(const string& query) {
    try {
        if (!isConnected()) {
            cerr << "Not connected to database!" << endl;
            return nullptr;
        }

        return unique_ptr<sql::PreparedStatement>(con->prepareStatement(query));
    }
    catch (sql::SQLException& e) {
        cerr << "Prepared statement error: " << e.what() << endl;
        cerr << "Error code: " << e.getErrorCode() << endl;
        cerr << "Failed query: " << query << endl;
        return nullptr;
    }
}

// Test connection with sample queries
bool DatabaseConnection::testConnection() {
    try {
        if (!isConnected()) {
            cerr << "Not connected to database!" << endl;
            return false;
        }

        cout << "\n========================================" << endl;
        cout << "Testing Database Connection" << endl;
        cout << "========================================\n" << endl;

        // Test 1: Check if tables exist
        cout << "Test 1: Checking database tables..." << endl;
        auto tablesRes = executeQuery("SHOW TABLES");
        if (tablesRes) {
            cout << "Tables found in database: " << tablesRes->rowsCount() << endl;
            while (tablesRes->next()) {
                cout << "  - " << tablesRes->getString(1) << endl;
            }
        }

        // Test 2: Query Admin table
        cout << "\nTest 2: Querying Admin table..." << endl;
        auto adminRes = executeQuery("SELECT * FROM Admin");
        if (adminRes) {
            cout << "Admin records found: " << adminRes->rowsCount() << endl;
            while (adminRes->next()) {
                cout << "  - AdminID: " << adminRes->getString("AdminID")
                    << ", Name: " << adminRes->getString("Name")
                    << ", Email: " << adminRes->getString("Email") << endl;
            }
        }

        // Test 3: Query Staff table
        cout << "\nTest 3: Querying Staff table..." << endl;
        auto staffRes = executeQuery("SELECT * FROM Staff");
        if (staffRes) {
            cout << "Staff records found: " << staffRes->rowsCount() << endl;
            while (staffRes->next()) {
                cout << "  - StaffID: " << staffRes->getString("StaffID")
                    << ", Name: " << staffRes->getString("Name")
                    << ", Status: " << staffRes->getString("Status") << endl;
            }
        }

        // Test 4: Query Menu table
        cout << "\nTest 4: Querying Menu table..." << endl;
        auto menuRes = executeQuery("SELECT * FROM Menu");
        if (menuRes) {
            cout << "Menu items found: " << menuRes->rowsCount() << endl;
            while (menuRes->next()) {
                cout << "  - MenuID: " << menuRes->getString("MenuID")
                    << ", Item: " << menuRes->getString("Menu_name")
                    << ", Price: RM" << menuRes->getDouble("Price")
                    << ", Category: " << menuRes->getString("Category") << endl;
            }
        }

        // Test 5: Query Tables
        cout << "\nTest 5: Querying Tables..." << endl;
        auto tablesDataRes = executeQuery("SELECT * FROM Tables");
        if (tablesDataRes) {
            cout << "Tables found: " << tablesDataRes->rowsCount() << endl;
            while (tablesDataRes->next()) {
                cout << "  - Table #" << tablesDataRes->getString("Table_number")
                    << ", Capacity: " << tablesDataRes->getInt("Capacity")
                    << ", Status: " << tablesDataRes->getString("Status") << endl;
            }
        }

        cout << "\n========================================" << endl;
        cout << "All tests completed successfully!" << endl;
        cout << "========================================\n" << endl;

        return true;
    }
    catch (sql::SQLException& e) {
        cerr << "Test connection error: " << e.what() << endl;
        cerr << "Error code: " << e.getErrorCode() << endl;
        return false;
    }
}