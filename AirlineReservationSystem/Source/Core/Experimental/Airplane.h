#pragma once
#include <string>
#include <vector>

namespace AirlineReservationSystem
{


	struct Seat;

	struct Passenger
	{
		std::string name;

		// std::string email;
		// unsigned short age;
		// unsigned short maxLoad;
		// unsigned int contactNumber;
		Seat* seat;
	};

	struct Seat
	{
		std::string name;
		Passenger* passenger;

		void SetPassenger(Passenger* passenger)
		{
			this->passenger = passenger;
		}

		~Seat()
		{
			delete passenger;
		}
	};

	class Airplane
	{
	private:
		std::string m_Name;
		std::vector<Seat*> m_Seats;

	public:
		Airplane(std::string name, std::vector<Seat*> vecSeats);

	public:
		std::string GetName();
		std::vector<Seat*> GetSeats();
	};
}


