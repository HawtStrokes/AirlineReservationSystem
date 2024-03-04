#pragma once

#include <vector>
#include <string>
namespace AirlineReservationSystem
{

	class Airplane;

	class AmadeusInterface
	{
	private:
		std::string API_KEY;
		std::string API_SECRET;
		// std::string m_AccessToken;

	private:
		// private Ctor
		AmadeusInterface();

	public:
		static AmadeusInterface& Get();
		std::vector<Airplane*> SearchFlights(std::string from, std::string to, std::string date,
			unsigned short adults, unsigned short childrens, unsigned short infants);
		std::string GetAccessToken();
		int ReserveFlight(Airplane* airplane, std::vector<std::string> infoVec);

		// std::string GetAccessToken();
	};
}


