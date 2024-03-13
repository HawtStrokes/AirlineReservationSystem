#pragma once
#include <string>
#include <vector>

namespace SQLite { class Database; }

namespace AirlineReservationSystem
{
	class Flight;

	struct FlightInformation
	{
		std::string flightID, origin, destination, bookingTable;
		int departureTime, arrivalTime, rows, cols;
	};

	struct BookingInformation
	{
		std::string name, email, maxLoad;
		int row, col;
	};

	class PersistenceManager
	{
	private:
		PersistenceManager();

	private:
		// TODO: Create internal class member functions for implementations that reuses the same code
		// void Internal_InsertBookingData(Flight& flight, SQLite::Database& db, const std::string& tableName, BookingInformation bookingInfo)

	public:
		static PersistenceManager& Get();

		void CreateFlightsTable(SQLite::Database& db);

        void InsertFlight(SQLite::Database& db, const std::string& flightId,
            const std::string& origin, const std::string& destination,
            const std::string& departureTime, const std::string& arrivalTime,
			int rows, int cols);

		void InsertFlight(SQLite::Database& db, FlightInformation flightInfo);

        void CreateBookingTable(SQLite::Database& db, const std::string& tableName);

		void InsertBookingData(Flight& flight, SQLite::Database& db, const std::string& tableName);
		// void InsertBookingData(Flight& flight, SQLite::Database& db, const std::string& tableName, BookingInformation bookingInfo);

		void DeleteBookingData(SQLite::Database& db, const std::string& tableName, int row, int col);

		std::vector<FlightInformation*> GetFlightsInfo(SQLite::Database& db);

		std::vector<BookingInformation*> GetBookingsInfo(const SQLite::Database& db, const std::string& tableName);

		void Save(const SQLite::Database& db);

		void PrintFlights(const SQLite::Database& db);

        void PrintBooking(const SQLite::Database& db, const std::string& tableName);
	};

}