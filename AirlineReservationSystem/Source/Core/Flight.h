#pragma once
#include <string>
// #include <iostream>

#include "SeatMap.h"

namespace AirlineReservationSystem
{
    class PersistenceManager;
    struct FlightInformation;

    class Flight
    {
    private:
        std::string m_FlightID;
        std::string m_Origin;
        std::string m_Destination;
        int m_DepartureTime;
        int m_ArrivalTime;
        SeatMap m_SeatMap;

    public:
        // TODO Move Persistence implementation to Persistence Manager Class and use friend
        void SaveToJSON(const std::string& filename) const;

        void LoadFromJSON(const std::string& filename);

        void LoadFromFlightInformation(const SQLite::Database& db, const FlightInformation& flightInfo);

        std::string GenerateSeatCode(unsigned short row, unsigned short col);


        friend PersistenceManager;

    public:
        Flight() = delete;
        /*Flight(std::string flightID, std::string origin, std::string destination,
            int departureTime, int arrivalTime, int rows, int cols);*/
        Flight(const std::string& flightID, const std::string& origin, const std::string& destination,
               int departureTime,
               int arrivalTime, int rows, int cols);
    public:
        bool BookSeat(int row, int col, const std::string& passengerName, const std::string& email, const std::string& maxLoad);

        void CancelBooking(int row, int col);

        void DisplayInfo() const;

        std::string GetPassengerName(int row, int col) const;

        std::string GetEmail(int row, int col) const;

        std::string GetMaxLoad(int row, int col) const;

    public:
        std::string GetFlightID() const;
        std::string GetDestination() const;
        SeatMap& GetSeatMap();
    };
}
