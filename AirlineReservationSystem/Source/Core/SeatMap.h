#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"

namespace AirlineReservationSystem
{

    using json = nlohmann::json;

    class SeatMap
    {
    public:
        // TODO Move Persistence implementation to Persistence Manager Class and use friend
        void SaveToJSON(const std::string& filename) const
        {
            json data;
            data["rows"] = m_Rows;
            data["cols"] = m_Cols;
            data["seatMap"] = m_SeatMap;
            data["passengerMap"] = m_PassengerMap;

            std::ofstream file(filename);
            if (file.is_open())
            {
                file << data.dump(4); // Pretty-print JSON
                std::cout << "Booking information saved to " << filename << std::endl;
            }
            else
            {
                std::cerr << "Error opening file for writing: " << filename << std::endl;
            }
        }

        void LoadFromJSON(const std::string& filename)
        {
            std::ifstream file(filename);
            if (file.is_open())
            {
                json data;
                file >> data;

                m_Rows = data["rows"];
                m_Cols = data["cols"];
                m_SeatMap = data["seatMap"];
                m_PassengerMap = data["passengerMap"];

                std::cout << "Booking information loaded from " << filename << std::endl;
            }
            else
            {
                std::cerr << "Error opening file for reading: " << filename << std::endl;
            }
        }

    public:
        SeatMap() : m_Rows(0), m_Cols(0) {}
        SeatMap(int rows, int cols) : m_Rows(rows), m_Cols(cols)
        {
            // Initialize the seat map with available seats
            m_SeatMap.resize(m_Rows, std::vector<bool>(m_Cols, true));
            m_PassengerMap.resize(m_Rows, std::vector<std::string>(m_Cols));
        }

        bool IsSeatAvailable(int row, int col) const
        {
            if (row >= 0 && row < m_Rows && col >= 0 && col < m_Cols)
            {
                return m_SeatMap[row][col];
            }
            return false; // Invalid seat
        }

        bool BookSeat(int row, int col, const std::string& passengerName)
        {
            if (IsSeatAvailable(row, col))
            {
                m_SeatMap[row][col] = false; // Mark seat as booked
                m_PassengerMap[row][col] = passengerName;
                return true;
            }
            return false; // Seat not available or invalid
        }

        void CancelBooking(int row, int col)
        {
            if (row >= 0 && row < m_Rows && col >= 0 && col < m_Cols)
            {
                m_SeatMap[row][col] = true; // Mark seat as available
                m_PassengerMap[row][col].clear();
            }
        }

        std::string GetPassengerName(int row, int col) const
        {
            if (row >= 0 && row < m_Rows && col >= 0 && col < m_Cols)
            {
                return m_PassengerMap[row][col];
            }
            return ""; // Invalid seat
        }

        void DisplaySeatMap() const
        {
            for (const auto& row : m_SeatMap)
            {
                int counter = 0;
                for (bool seat : row)
                {
                    std::cout << (seat ? "O" : "X") << " ";
                }
                std::cout << std::endl;
            }
        }

        int GetRows()
        {
            m_Rows;
        }

        int GetColumns()
        {
            return m_Cols;
        }
        std::vector<std::vector<bool>> GetVecSeatMap()
        {
            return m_SeatMap;
        }
        std::vector<std::vector<std::string>> GetVecPassengerMap() 
        {
            return m_PassengerMap;
        }

    private:
        int m_Rows;
        int m_Cols;
        std::vector<std::vector<bool>> m_SeatMap;
        std::vector<std::vector<std::string>> m_PassengerMap;
    };
}