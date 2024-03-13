#include <iostream>
#include <exception>

#include "AmadeusInterface.h"

#include "curl/curl.h"
#include "nlohmann/json.hpp"

namespace AirlineReservationSystem
{
    // Callback function to write response data
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response)
    {
	    const size_t totalSize = size * nmemb;
        response->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }

    // Add the API Key and Secret from your account
    const std::string AmadeusInterface::m_API_KEY = "";
    const std::string AmadeusInterface::m_API_SECRET = "";

	AmadeusInterface::AmadeusInterface()
	{
        m_AccessToken = this->GetAccessToken(); // Initialize m_AccessToken
    }

    std::string AmadeusInterface::Internal_PerformHttpGet(const char* url, const char* authorizationHeader)
    {
        CURL* curl = curl_easy_init();
        std::string response;

        if (curl)
        {
            // Set the URL
            curl_easy_setopt(curl, CURLOPT_URL, url);

            // Set the authorization header
            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, authorizationHeader);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // Set the write callback
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Perform the request
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }

            // Clean up
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }

        return response;
    }

	AmadeusInterface& AmadeusInterface::Get()
	{
		static AmadeusInterface instance;
		return instance;
	}

    std::string AmadeusInterface::GetAccessToken()
    {
        std::string accessToken;
        CURL* curl = curl_easy_init();
        if (!curl)
        {
            std::cerr << "Error initializing libcurl." << std::endl;
            // return 1;
        }

        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://test.api.amadeus.com/v1/security/oauth2/token");

        // Set the headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        std::string postFields = "grant_type=client_credentials&client_id=" + m_API_KEY + "&client_secret=" + m_API_SECRET;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

        // Perform the request
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            try
            {
                auto json = nlohmann::json::parse(response);
                accessToken = json["access_token"];
                /*std::string tokenType = json["token_type"];
                int expiresIn = json["expires_in"];*/
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            }
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return accessToken;
    }

    json AmadeusInterface::GetAvailableFlights(const std::string& origin, const std::string& destination, const std::string& departureDate, const std::string& returnDate)
    {
        // curl -X GET "https://test.api.amadeus.com/v2/shopping/flight-offers?originLocationCode=PAR&destinationLocationCode=MAD&departureDate=2024-03-22&returnDate=2024-03-30&adults=1&nonStop=false&maxPrice=200&currency=EUR" -H "Authorization: Bearer API_SECRET"
        const std::string url = "https://test.api.amadeus.com/v2/shopping/flight-offers?originLocationCode=" + origin +
            "&destinationLocationCode=" + destination + "&departureDate=" + departureDate +
            "&returnDate=" + returnDate + "&adults=1&nonStop=false";
        const std::string authorizationHeader = std::string(std::string("Authorization: Bearer ") + this->GetAccessToken());
        std::cout << url << std::endl;
        std::cout << authorizationHeader << std::endl;
        return json::parse(Internal_PerformHttpGet(url.c_str(), authorizationHeader.c_str()));
    }

}