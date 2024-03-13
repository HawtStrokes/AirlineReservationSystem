#include "PersistenceManager.h"
#include "Flight.h"
#include "SeatMap.h"
#include "../Util/Util.h"

#include "SQLiteCpp/SQLiteCpp.h"

#include <iostream>

namespace AirlineReservationSystem
{   
    PersistenceManager::PersistenceManager()
    {
        // Todo: pre read existing databases and load somewhere
    }

    PersistenceManager& PersistenceManager::Get()
    {
        static PersistenceManager instance;
        return instance;
    }


    // FLIGHTS FUNCTIONS

    void PersistenceManager::CreateFlightsTable(SQLite::Database& db)
    {

        db.exec("CREATE TABLE IF NOT EXISTS flights ("
            "flightId TEXT PRIMARY KEY,"
            "origin TEXT,"
            "destination TEXT,"
            "departureTime DATETIME,"
            "arrivalTime DATETIME,"
            "bookingTable TEXT," // a text of the name of the booking table associated with it.
            "rows INTEGER,"
            "cols INTEGER"
            ");");
    }

    void PersistenceManager::InsertFlight(SQLite::Database& db, const std::string& flightId,
        const std::string& origin, const std::string& destination,
        const std::string& departureTime, const std::string& arrivalTime, int rows, int cols)
    {
        std::string bookingTable = "booking" + flightId;
        SQLite::Transaction transaction(db); // Start a transaction
        try
        {
            SQLite::Statement query(db, "INSERT or REPLACE INTO flights (flightId, origin, destination, departureTime, arrivalTime, bookingTable, rows, cols) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
            query.bind(1, flightId);
            query.bind(2, origin);
            query.bind(3, destination);
            query.bind(4, departureTime);
            query.bind(5, arrivalTime);
            query.bind(6, bookingTable);
            query.bind(7, rows);
            query.bind(8, cols);
            query.exec();

            transaction.commit(); // Commit the transaction
            std::cout << "Flight " << flightId << " inserted successfully." << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error inserting flight: " << e.what() << std::endl;
            transaction.rollback(); // Roll back the transaction on error
        }
    }

    void PersistenceManager::InsertFlight(SQLite::Database& db, FlightInformation flightInfo)
    {
        std::string bookingTable = "booking" + flightInfo.flightID;
        SQLite::Transaction transaction(db); // Start a transaction
        try
        {
            SQLite::Statement query(db, "INSERT or REPLACE INTO flights (flightId, origin, destination, departureTime, arrivalTime, bookingTable) "
                "VALUES (?, ?, ?, ?, ?, ?);");
            query.bind(1, flightInfo.flightID);
            query.bind(2, flightInfo.origin);
            query.bind(3, flightInfo.destination);
            query.bind(4, flightInfo.departureTime);
            query.bind(5, flightInfo.arrivalTime);
            query.bind(6, bookingTable);
            query.bind(7, flightInfo.rows);
            query.bind(8, flightInfo.cols);
            query.exec();

            transaction.commit(); // Commit the transaction
            std::cout << "Flight " << flightInfo.flightID << " inserted successfully." << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error inserting flight: " << e.what() << std::endl;
            transaction.rollback(); // Roll back the transaction on error
        }
    }


    // BOOKINGS FUNCTIONS

    void PersistenceManager::CreateBookingTable(SQLite::Database& db, const std::string& tableName)
    {
        db.exec("CREATE TABLE IF NOT EXISTS " + tableName + " ("
            "name TEXT,"
            "email TEXT,"
            "maxLoad TEXT,"
            "row INTEGER,"
            "col INTEGER"
            ");");
    }

    void PersistenceManager::InsertBookingData(Flight& flight, SQLite::Database& db, const std::string& tableName)
    {
        SeatMap& seatMap = flight.GetSeatMap();
        std::vector<std::vector<std::string>> passengerMap = seatMap.GetVecPassengerMap();

        for (int row = 0; row < seatMap.m_Rows; ++row)
        {
            for (int col = 0; col < seatMap.m_Cols; ++col)
            {
                if (!passengerMap[row][col].empty())
                {
                    SQLite::Transaction transaction(db); // Start a transaction
                    try
                    {
                        // Check if a booking with the same row and column exists
                        SQLite::Statement checkQuery(db, "SELECT row, col FROM " + tableName + " WHERE row = ? AND col = ?;");
                        checkQuery.bind(1, row);
                        checkQuery.bind(2, col);

                        if (checkQuery.executeStep()) // Entry exists, update it
                        {
                            SQLite::Statement updateQuery(db, "UPDATE " + tableName + " SET name = ?, email = ?, maxLoad = ? WHERE row = ? AND col = ?;");
                            updateQuery.bind(1, passengerMap[row][col]);
                            updateQuery.bind(2, seatMap.GetEmail(row, col));
                            updateQuery.bind(3, seatMap.GetMaxLoad(row, col));
                            updateQuery.bind(4, row);
                            updateQuery.bind(5, col);
                            updateQuery.exec();
                            std::cout << "Booking updated successfully." << std::endl;
                        }
                        else // Entry does not exist, insert a new one
                        {
                            SQLite::Statement insertQuery(db, "INSERT INTO " + tableName + "(name, email, maxLoad, row, col) VALUES (?, ?, ?, ?, ?);");
                            insertQuery.bind(1, passengerMap[row][col]);
                            insertQuery.bind(2, seatMap.GetEmail(row, col));
                            insertQuery.bind(3, seatMap.GetMaxLoad(row, col));
                            insertQuery.bind(4, row);
                            insertQuery.bind(5, col);
                            insertQuery.exec();
                            std::cout << "Booking inserted successfully." << std::endl;
                        }

                        transaction.commit(); // Commit the transaction
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "Error inserting/updating booking: " << e.what() << std::endl;
                        transaction.rollback(); // Roll back the transaction on error
                    }
                }
            }
        }

    }

    // Move the following to UTIL
    // Define a function to count the number of entries in a table
    size_t CountEntriesInTable(const SQLite::Database& db, const std::string& tableName)
    {
        try
        {
            // Prepare an SQL statement to count the rows in the specified table
            SQLite::Statement query(db, "SELECT COUNT(*) FROM " + tableName + ";");

            // Execute the query and retrieve the count
            if (query.executeStep())
            {
                size_t rowCount = query.getColumn(0).getInt(); // Get the count from the first column
                return rowCount;
            }
            else
            {
                // Handle the case where the query didn't return any results
                return 0;
            }
        }
        catch (const std::exception& e)
        {
            // Handle exceptions (e.g., invalid table name, database error)
            std::cerr << "Error counting entries: " << e.what() << std::endl;
            return -1; // Return a negative value to indicate an error
        }
    }

    // RETREIVAL FUNCTIONS


    std::vector<FlightInformation*> PersistenceManager::GetFlightsInfo(SQLite::Database& db)
    {
        std::vector<FlightInformation*> returnVec;
        size_t numEntries = CountEntriesInTable(db, "flights");
        returnVec.reserve(numEntries);
        try
        {
            // Prepare an SQL statement to query the flights table
            SQLite::Statement query(db, "SELECT * FROM flights");

            // Execute the query
            while (query.executeStep())
            {
                // Extract values from the query result
                std::string flightID = query.getColumn("flightId").getString();
                std::string origin = query.getColumn("origin").getString();
                std::string destination = query.getColumn("destination").getString();
                std::string bookingTable = query.getColumn("bookingTable").getString();

                int departureTime = AirlineReservationSystem::Util::DatetimeToInt(query.getColumn("departureTime").getString());
                int arrivalTime = AirlineReservationSystem::Util::DatetimeToInt(query.getColumn("arrivalTime").getString());
                int rows = query.getColumn("rows").getInt();
                int cols =  query.getColumn("cols").getInt();
                // int arrivalTime = query.getColumn("arrivalTime").getInt();

                // Create a new FlightInformation object and add it to the vector
                returnVec.push_back(new FlightInformation{ flightID, origin, destination, bookingTable,
                    departureTime, arrivalTime, rows, cols });
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error retrieving flights information: " << e.what() << std::endl;
        }

        return returnVec;
    }

    void PersistenceManager::DeleteBookingData(SQLite::Database& db, const std::string& tableName, int row, int col)
    {
        try
        {
            // Construct the DELETE query with placeholders for row and col
            SQLite::Statement query(db, "DELETE FROM " + tableName + " WHERE row = ? AND col = ? ");
            query.bind(1, row);
            query.bind(2, col);

            // Execute the query
            query.exec();

            std::cout << "Booking deleted successfully." << std::endl;
        }
        catch (const SQLite::Exception& e)
        {
            std::cerr << "Error deleting booking: " << e.what() << std::endl;
        }
        

    }

    std::vector<BookingInformation*> PersistenceManager::GetBookingsInfo(const SQLite::Database& db, const std::string& tableName)
    {
        std::vector<BookingInformation*> returnVec;
        const size_t numEntries = CountEntriesInTable(db, tableName);
        returnVec.reserve(numEntries);
        try
        {
            // Prepare an SQL statement to query the bookingTable
            SQLite::Statement query(db, "SELECT * FROM " + tableName + ";");

            // Execute the query
            while (query.executeStep())
            {
                // Extract values from the query result
                const std::string name = query.getColumn("name").getString();
                const std::string email = query.getColumn("email").getString();
                const std::string maxLoad = query.getColumn("maxLoad").getString();

                const int row = query.getColumn("row").getInt();
                const int col = query.getColumn("col").getInt();

                // Create a new BookingInformation object and add it to the vector
                returnVec.push_back(new BookingInformation{ name, email, maxLoad, row, col });
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error retrieving booking information: " << e.what() << std::endl;
        }
        return returnVec;
    }

    // Save to SQL Database
    void PersistenceManager::Save(const SQLite::Database& db)
    {
        throw "Not Implemented";
    }

    // PRINT FUNCTIONS

    void PersistenceManager::PrintFlights(const SQLite::Database& db)
    {
        SQLite::Statement query(db, "SELECT * FROM flights;");
        while (query.executeStep())
        {
            std::cout << "Flight ID: " << query.getColumn("flightId").getText() << std::endl;
            std::cout << "Origin: " << query.getColumn("origin").getText() << std::endl;
            std::cout << "Destination: " << query.getColumn("destination").getText() << std::endl;
            std::cout << "Departure Time: " << query.getColumn("departureTime").getString() << std::endl; 
            std::cout << "Arrival Time: " << query.getColumn("arrivalTime").getString() << std::endl; 
            std::cout << "Rows: " << query.getColumn("rows").getInt() << std::endl;
            std::cout << "Cols: " << query.getColumn("cols").getInt() << std::endl;
            std::cout << std::endl;
        }
    }

    void PersistenceManager::PrintBooking(const SQLite::Database& db, const std::string& tableName)
    {
        SQLite::Statement query(db, "SELECT * FROM " + tableName + ";");
        while (query.executeStep())
        {
            std::cout << "Seat Row: " << query.getColumn("row").getInt() << std::endl;
            std::cout << "Seat Column: " << query.getColumn("col").getInt() << std::endl;
            std::cout << "Passenger Name: " << query.getColumn("name").getText() << std::endl;
            std::cout << "Max Load: " << query.getColumn("maxLoad").getText() << std::endl;
            std::cout << "Email Address: " << query.getColumn("email").getText() << std::endl;
            std::cout << std::endl;
        }
    }
}