#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <memory>
#include <iostream>
#include <string>

using namespace std;

class DatabaseConnection {
private:
    sql::mysql::MySQL_Driver* driver;
    unique_ptr<sql::Connection> con;
    string server = "tcp://127.0.0.1:3306";
    string username = "root";
    string password = "";  // Change if you have a MySQL password
    string database = "restaurant_management";

public:
    // Constructor
    DatabaseConnection();

    // Destructor
    ~DatabaseConnection();

    // Connect to database
    bool connect();

    // Disconnect from database
    void disconnect();

    // Check if connected
    bool isConnected();

    // Get connection object
    sql::Connection* getConnection();

    // Execute query (SELECT)
    unique_ptr<sql::ResultSet> executeQuery(const string& query);

    // Execute update (INSERT, UPDATE, DELETE)
    int executeUpdate(const string& query);

    // Get prepared statement
    unique_ptr<sql::PreparedStatement> prepareStatement(const string& query);

    // Test connection with sample queries
    bool testConnection();
};