//#include "Airplane.h"
//#include "Flight.h"
//#include "AirlineManager.h"
//
//
//namespace AirlineReservationSystem
//{
//	AirlineManager::AirlineManager()
//	{
//
//	}
//
//
//	AirlineManager& AirlineManager::Get()
//	{
//		static AirlineManager instance;
//		return instance;
//	}
//
//	std::vector<Flight*> AirlineManager::GetFlights()
//	{
//		return m_Flights;
//	}
//
//	void AirlineManager::SetFlights(std::vector<Flight*> flights)
//	{
//		m_Flights = flights;
//	}
//
//	std::vector<Flight*> AirlineManager::SearchFlights(std::string from, std::string to)
//	{
//		std::vector<Flight*> retFiltered;
//		// Implement filtering functionality
//		return retFiltered;
//	}
//	void AirlineManager::ReserveFlight(Passenger* passenger, Flight* flight, Seat* seat)
//	{
//		flight->ReserveFlight(passenger, seat);
//	}
//}