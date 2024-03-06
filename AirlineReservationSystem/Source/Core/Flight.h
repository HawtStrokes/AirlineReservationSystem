#pragma once
#include <string>
#include <iostream>

#include "SeatMap.h"

namespace AirlineReservationSystem
{
    class Flight
    {
    private:
        std::string m_FlightID;
        std::string m_Destination;
        SeatMap m_SeatMap;

    public:
        // TODO Move Persistence implementation to Persistence Manager Class and use friend
        void SaveToJSON(const std::string& filename) const
        {
            m_SeatMap.SaveToJSON(filename);
        }

        void LoadFromJSON(const std::string& filename)
        {
            m_SeatMap.LoadFromJSON(filename);
        }

        std::string GenerateSeatCode(unsigned short row, unsigned short col)
        {
            std::string lHandCode;
            unsigned short procRow = row + 65;
            while (procRow > 90)
            {
                lHandCode += 'Z';
                procRow - 26;
            }
            lHandCode += static_cast<char>(procRow);
            return lHandCode + std::to_string(col);
        }

    public:
        Flight() : m_FlightID(""), m_Destination(""), m_SeatMap() {}
        Flight(const std::string& flightID, const std::string& destination, int rows, int cols)
            : m_FlightID(flightID), m_Destination(destination), m_SeatMap(rows, cols)
        {
        }

    public:
        bool BookSeat(int row, int col, const std::string& passengerName)
        {
            return m_SeatMap.BookSeat(row, col, passengerName);
        }

        void CancelBooking(int row, int col)
        {
            m_SeatMap.CancelBooking(row, col);
        }

        void DisplayInfo() const
        {
            std::cout << "Flight " << m_FlightID << " to " << m_Destination << std::endl;
            m_SeatMap.DisplaySeatMap();
        }

        std::string GetPassengerName(int row, int col) const
        {
            return m_SeatMap.GetPassengerName(row, col);
        }

    public:
        std::string GetFlightNumber() const
        {
            return m_FlightID;
        }
        std::string GetDestination() const
        {
            return m_Destination;
        }
        SeatMap& GetSeatMap()
        {
            return m_SeatMap;
        }
    };
}
