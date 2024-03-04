//#include "AmadeusInterface.h"
//#include "Airplane.h"
//
//#include "curl/curl.h"
//
//#include "nlohmann/json.hpp"
//
//#include <iostream>
//#include <exception>
//
//namespace AirlineReservationSystem
//{
//	AmadeusInterface::AmadeusInterface()    // Add API Keys here
//	{}
//
//    // Callback function to write response data
//    size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response)
//    {
//        size_t totalSize = size * nmemb;
//        response->append(static_cast<char*>(contents), totalSize);
//        return totalSize;
//    }
//
//    std::string AmadeusInterface::GetAccessToken()
//    {
//        std::string accessToken;
//        CURL* curl = curl_easy_init();
//        if (!curl)
//        {
//            std::cerr << "Error initializing libcurl." << std::endl;
//            // return 1;
//        }
//
//        // Set the URL
//        curl_easy_setopt(curl, CURLOPT_URL, "https://test.api.amadeus.com/v1/security/oauth2/token");
//
//        // Set the headers
//        struct curl_slist* headers = nullptr;
//        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
//        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//
//        std::string postFields = "grant_type=client_credentials&client_id=" + API_KEY + "&client_secret=" + API_SECRET;
//        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
//
//        // Perform the request
//        std::string response;
//        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
//        CURLcode res = curl_easy_perform(curl);
//
//        if (res != CURLE_OK)
//        {
//            std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
//        }
//        else
//        {
//            try
//            {
//                auto json = nlohmann::json::parse(response);
//                accessToken = json["access_token"];
//                /*std::string tokenType = json["token_type"];
//                int expiresIn = json["expires_in"];*/
//            }
//            catch (const std::exception& e)
//            {
//                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
//            }
//        }
//
//        // Clean up
//        curl_slist_free_all(headers);
//        curl_easy_cleanup(curl);
//
//        return accessToken;
//    }
//
//
//	AmadeusInterface& AmadeusInterface::Get()
//	{
//		static AmadeusInterface instance;
//		return instance;
//	}
//	std::vector<Airplane*> AmadeusInterface::SearchFlights(std::string from, std::string to, std::string date, unsigned short adults, unsigned short childrens, unsigned short infants)
//	{
//		// curl amadeus self service
//
//		return std::vector<Airplane*>();
//	}
//	int AmadeusInterface::ReserveFlight(Airplane* airplane, std::vector<std::string> infoVec)
//	{
//		// curl amadeus self service then proceed to use paypal Rest API
//		return 0;
//	}
//}