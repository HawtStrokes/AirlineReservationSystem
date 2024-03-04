#pragma once

#include <string>

namespace AirlineReservationSystem
{

	class Airplane;
	struct Seat;

	class Flight
	{
	private:
		std::string m_FlightId;
		Airplane* m_Airplane;

		std::string m_From;
		std::string m_To;

		// std::string m_DepartureDateTime;
		// std::string m_ArrivalDateTime;

	public:
		Flight(std::string flightID, Airplane* airplane, std::string from, std::string to);

	public:
		std::string GetFlightID();
		Airplane* GetAirplane();

		std::string GetFrom();
		std::string GetTo();
		// std::string GetDepartureDateTime();
		// std::string GetArrivalDateTime();

		void ReserveFlight(Passenger* passenger, Seat* seat);
		void CancelReservation(Passenger* passenger);
	};

}

