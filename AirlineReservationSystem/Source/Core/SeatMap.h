#pragma once

#include <vector>
#include <string>

#include "nlohmann/json.hpp"

#include "PersistenceManager.h"

namespace AirlineReservationSystem
{
    using json = nlohmann::json;

    class PersistenceManager;

    class SeatMap
    {
    private:
        int m_Rows;
        int m_Cols;
        std::vector<std::vector<bool>> m_SeatMap;

        // Todo: Convert to a struct with appropriate data
        std::vector<std::vector<std::string>> m_PassengerMap;
        std::vector<std::vector<std::string>> m_EmailMap;
        std::vector<std::vector<std::string>> m_MaxLoadMap;

    public:
        // TODO Move Persistence implementation to Persistence Manager Class and use friend
        void SaveToJSON(const std::string& filename) const;

        void LoadFromJSON(const std::string& filename);

        // Load Bookings from Flight Information
        void LoadFromFlightInformation(const SQLite::Database& db, const FlightInformation& flightInfo);

    private:
        inline void Internal_InitMaps();

    public:
        SeatMap() = delete;

        SeatMap(int rows, int cols);

    public:
        bool IsSeatAvailable(int row, int col);

        bool BookSeat(int row, int col, const std::string& passengerName, const std::string& email, const std::string& maxLoad);

        void CancelBooking(int row, int col);

        std::string GetPassengerName(int row, int col) const;

        std::string GetMaxLoad(int row, int col) const;

        std::string GetEmail(int row, int col) const;

        void DisplaySeatMap() const;

        int GetRows();

        int GetColumns();

        std::vector<std::vector<bool>> GetVecSeatMap();

        std::vector<std::vector<std::string>> GetVecPassengerMap();

        friend PersistenceManager;
    };
}