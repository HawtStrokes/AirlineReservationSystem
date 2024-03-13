#pragma once
#include <string>
#include "nlohmann/json_fwd.hpp"

namespace AirlineReservationSystem
{
	using json = nlohmann::json;
	
	class AmadeusInterface
	{
	private:
		static const std::string m_API_KEY;
		static const std::string m_API_SECRET;
		std::string m_AccessToken;

	private:
		// private Ctor
		AmadeusInterface();

	private:
		std::string Internal_PerformHttpGet(const char* url, const char* authorizationHeader);

	public:
		static AmadeusInterface& Get();
		std::string GetAccessToken();
		json GetAvailableFlights(const std::string& origin, const std::string& destination,
			const std::string& departureDate, const std::string& returnDate);
	};
}


