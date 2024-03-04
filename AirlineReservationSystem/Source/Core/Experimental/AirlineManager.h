#pragma once
#include <string>
#include <vector>

namespace AirlineReservationSystem
{

	class Flight;

	class AirlineManager
	{
	private:
		// Exception Throwers
		/*inline void Internal_ThrowEmpty(const std::string&);
		inline void Internal_ThrowCheckConfig(const std::string&);
		inline void Internal_CheckIllegals(const std::string&);*/

	private:
		std::vector<Flight*>  m_Flights;

		// private Ctor
		AirlineManager();

	public:
		static AirlineManager& Get();

		std::vector<Flight*> GetFlights();
		void SetFlights(std::vector<Flight*> flights);
		std::vector<Flight*> SearchFlights(std::string from, std::string to);

		void ReserveFlight(Passenger* passenger, Flight* flight, Seat* seat);
		
	};
}