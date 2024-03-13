#include "Flight.h"

#include "PersistenceManager.h"
#include "SeatMap.h"

#include <string>
#include <iostream>

namespace AirlineReservationSystem
{
    void Flight::SaveToJSON(const std::string& filename) const
    {
        m_SeatMap.SaveToJSON(filename);
    }

    void Flight::LoadFromJSON(const std::string& filename)
    {
        m_SeatMap.LoadFromJSON(filename);
    }

    void Flight::LoadFromFlightInformation(const SQLite::Database& db, const FlightInformation& flightInfo)
    {
        m_SeatMap.LoadFromFlightInformation(db, flightInfo);
    }

    std::string Flight::GenerateSeatCode(unsigned short row, unsigned short col)
    {
        std::string lHandCode;
        unsigned short procRow = row + 65;
        while (procRow > 90)
        {
            lHandCode += 'Z';
            procRow -= 26;
        }
        lHandCode += static_cast<char>(procRow);
        return lHandCode + std::to_string(col);
    }

    Flight::Flight(const std::string& flightID, const std::string& origin, const std::string& destination,
        int departureTime, int arrivalTime, int rows, int cols)
        : m_FlightID(flightID), m_Origin(origin), m_Destination(destination),
           m_DepartureTime(departureTime), m_ArrivalTime(arrivalTime), m_SeatMap(rows, cols)
    {
    }


    bool Flight::BookSeat(int row, int col, const std::string& passengerName, const std::string& email, const std::string& maxLoad)
    {
        return m_SeatMap.BookSeat(row, col, passengerName, email, maxLoad);
    }

    void Flight::CancelBooking(int row, int col)
    {
        m_SeatMap.CancelBooking(row, col);
    }

    void Flight::DisplayInfo() const
    {
        std::cout << "Flight " << m_FlightID << " to " << m_Destination << std::endl;
        m_SeatMap.DisplaySeatMap();
    }

    std::string Flight::GetPassengerName(int row, int col) const
    {
        return m_SeatMap.GetPassengerName(row, col);
    }

    std::string Flight::GetEmail(int row, int col) const
    {
        return m_SeatMap.GetEmail(row, col);
    }

    std::string Flight::GetMaxLoad(int row, int col) const
    {
        return m_SeatMap.GetMaxLoad(row, col);
    }


    std::string Flight::GetFlightID() const
    {
        return m_FlightID;
    }
    std::string Flight::GetDestination() const
    {
        return m_Destination;
    }
    SeatMap& Flight::GetSeatMap()
    {
        return m_SeatMap;
    }
}
