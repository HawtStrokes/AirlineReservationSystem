#include <vector>
#include <iostream>
#include <fstream>

#include "Gui/GuiApp.h"
#include "imgui.h"
// #include "imfilebrowser.h"

#include "Core/SeatMap.h"
#include "Core/Flight.h"
#include "Core/PersistenceManager.h"
#include "Core/AmadeusInterface.h"

#include "Util/Util.h"

#include "SQLiteCpp/SQLiteCpp.h"

using namespace AirlineReservationSystem;

namespace AirlineReservationSystem
{
	static ImVec2 g_WindowDimensions = { 800, 1000 };
	static const std::filesystem::path flightPath = std::filesystem::current_path().append("flights");
	
	static bool showAbout = false;
	static bool showHelp = false;
	
	static SeatMap* activeSeatMap = nullptr;
	static Flight* activeFlight = nullptr;
	SQLite::Database db("flights.db", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);

	//static ImGui::FileBrowser fileDialog;
	//// static ImGui::FileBrowser fileDialogTheme(ImGuiFileBrowserFlags_SelectDirectory);

	static struct { int row, col; } g_SelectedRowCol = {0,0};
	static ImGuiTextBuffer g_LogBuf;
	static std::string flightId = "No Flight Selected";

	static char passengerBuffer[512];
	static char emailBuffer[512];
	static char maxLoadBuffer[512];

	inline std::string GenerateBookingTableName()
	{
		if (activeFlight != nullptr) return "Booking" + activeFlight->GetFlightID();
		return "";	// Add Exception Handling
	}


	namespace GUIComponents
	{
		// MENU ITEMS
		static void About()
		{
			ImGui::Begin("About", &showAbout);
			ImGui::TextWrapped("AirlineReservationSystem v0.1a");
			ImGui::End();
		}
		static void Help()
		{
			ImGui::Begin("Help", &showHelp);
			if (ImGui::Button("Copy Link to Official GitHub Page"))
			{
				ImGui::SetClipboardText("https://github.com/HawtStrokes/AirlineReservationSystem");
			}

			ImGui::Separator();

			if (ImGui::CollapsingHeader("Navigation and Layout"))
			{
				const std::string strNavHelp = R"(There are three dock spaces, and each window can be docked to any of the three dock spaces or be left floating. 

To dock a window to a dock space, simply drag the title bar of a window to one of the five dock spaces

It isn't required to dock a window, it can be left detached so long as you don't direct it to a dock space.
)";
				ImGui::TextWrapped("%s", strNavHelp.c_str());
			}

			if (ImGui::CollapsingHeader("Bookings"))
			{
				const std::string persistHelp = R"(You can create new bookings through the Create Bookings Menu and Manage them (update or delete) via the Manage Bookings Menu
)";
				ImGui::TextWrapped("%s", persistHelp.c_str());
			}

			ImGui::End();
		}

		// Deprecated JSON Implementation
		/*
		 // CORE GUI
		namespace JSONImplementation
		{
			// Display SeatMap
			static void SeatMapGUI()
			{
				ImGui::Begin("Seat Map", nullptr);
				if (activeFlight != nullptr)
				{
					int labelCounter = 0;
					int seatLNum = 0;
					for (const auto& row : activeSeatMap->GetVecSeatMap())
					{
						int seatRNum = 0;
						for (bool seat : row)
						{
							std::string codeText = (seat ? "AVAILABLE" : "TAKEN");
							codeText += " (" + std::to_string(seatLNum) + std::string(",") + std::to_string(seatRNum) + ") " +
								activeFlight->GenerateSeatCode(seatLNum, seatRNum);
							std::string seatCodeGui = "###Btn_Seat_";
							seatCodeGui += std::to_string(labelCounter); // AVAILABLE###Btn_Seat_1 // Label is Btn_Seat_1
							codeText += seatCodeGui;
							if (ImGui::Button(codeText.c_str(), ImVec2(g_WindowDimensions.x / activeSeatMap->GetVecSeatMap().size(), g_WindowDimensions.x / activeSeatMap->GetVecSeatMap().size())))
							{
								// selectedSeatCode = seatCodeGui;
								// Get row and column
								g_SelectedRowCol = { seatLNum, seatRNum };
								std::cout << seatCodeGui << " " << seatLNum << " " << seatRNum << std::endl;
								g_LogBuf.appendf("Selected seat is %s.\n", std::string(seat ? "AVAILABLE" : "TAKEN").c_str());

							}
							ImGui::SameLine();
							++labelCounter;
							++seatRNum;
						}
						++seatLNum;
						seatRNum = 0;
						ImGui::Separator();
					}
					seatLNum = 0;
					labelCounter = 0;
				}
				else
					ImGui::Text("No Flight Selected.");
				ImGui::End();
			}
			// Select a flight
			static void SelectFlight()
			{
				ImGui::Begin("Select Flight", nullptr);
				if (ImGui::Button("Browse Flights", ImVec2(g_WindowDimensions.x - 30, 50)))
				{
					fileDialog.Open();
				}
				if (fileDialog.HasSelected())
				{
					std::string str = fileDialog.GetSelected().filename().string();
					std::size_t pos = str.find(".json");      // position of ".json" in str
					flightId = str.substr(0, pos);
					delete activeFlight;
					activeFlight = new Flight(flightId, "", "", 0, 0, 0, 0);
					activeFlight->LoadFromJSON(fileDialog.GetSelected().string());
					activeSeatMap = &activeFlight->GetSeatMap();
					g_LogBuf.appendf("Flight Selected: %s\n", flightId.c_str());
					fileDialog.ClearSelected();
				}

				// Todo, add flight information text here

				//if (ImGui::Button("Select", ImVec2(g_WindowDimensions.x - 30, 50)))
				//{
				//
				//}
				ImGui::End();
			}

			// Manage Reservations (Update and Delete)
			static void ManageBookings()
			{
				ImGui::Begin("Manage Bookings", nullptr);
				if (activeFlight != nullptr)
				{
					ImGui::Text("(Row, Col): (%s, %s) %s", std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str(),
						activeFlight->GenerateSeatCode(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
					if (!activeSeatMap->IsSeatAvailable(g_SelectedRowCol.row, g_SelectedRowCol.col))
					{
						static char passengerBuffer[512];
						static char emailBuffer[512];
						static char maxLoadBuffer[512];
						ImGui::Separator();
						ImGui::Text("Seat Information:");
						// strcpy_s(passengerBuffer, activeFlight->GetPassengerName(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
						ImGui::Text("Passenger Name: %s", activeFlight->GetPassengerName(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
						ImGui::Text("Email Address: %s", activeFlight->GetEmail(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
						ImGui::Text("Max Load: %s", activeFlight->GetMaxLoad(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());

						ImGui::InputText("Update Passenger Name", passengerBuffer, 512);
						ImGui::InputText("Update Email Address", emailBuffer, 512);
						ImGui::InputText("Update Passenger Max Load", maxLoadBuffer, 512);

						if (ImGui::Button("Update", ImVec2(200, 30)))
						{
							activeFlight->CancelBooking(g_SelectedRowCol.row, g_SelectedRowCol.col);
							activeFlight->BookSeat(g_SelectedRowCol.row, g_SelectedRowCol.col, passengerBuffer, emailBuffer, maxLoadBuffer);
							activeFlight->SaveToJSON(flightPath.string() + "\\" + (activeFlight->GetFlightID() + ".json"));
							g_LogBuf.appendf("Booking Updated!\nPassenger Name: %s\n(Row, Col): (%d, %d)\n", passengerBuffer,
								g_SelectedRowCol.row, g_SelectedRowCol); 
						}
						ImGui::SameLine();
						if (ImGui::Button("Delete", ImVec2(200, 30)))
						{
							activeFlight->CancelBooking(g_SelectedRowCol.row, g_SelectedRowCol.col);;
							activeFlight->SaveToJSON(flightPath.string() + "\\" + (activeFlight->GetFlightID() + ".json"));
							g_LogBuf.appendf("Booking Deleted!\n(Row, Col): (%d, %d)\n", g_SelectedRowCol.row, g_SelectedRowCol.col); // Add Exception Handling
						}
					}
					else
					{
						ImGui::Text("Select A Booked Seat");
					}

				}
				else
				{
					ImGui::Text("No Flight Selected");
				}
				ImGui::End();
			}

			// Add reservations
			static void CreateBookings()
			{

				ImGui::Begin("Create Bookings", nullptr);
				if (activeFlight != nullptr)
				{
					static char nameBuffer[512];
					ImGui::InputText("Passenger Name", nameBuffer, sizeof(nameBuffer));
					//ImGui::Text("(Row, Col): (%s, %s)", std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str());
					static char emailBuffer[512];
					ImGui::InputText("Email Address", emailBuffer, sizeof(emailBuffer));
					//ImGui::Text("(Row, Col): (%s, %s)", std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str());
					static char maxLoadBuffer[512];
					ImGui::InputText("Max Load", maxLoadBuffer, sizeof(maxLoadBuffer));
					ImGui::Text("(Row, Col): (%s, %s)", std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str());

					if (ImGui::Button("Book"))
					{
						if (activeFlight->BookSeat(g_SelectedRowCol.row, g_SelectedRowCol.col, nameBuffer, emailBuffer, maxLoadBuffer))
						{
							// Valid
							activeFlight->SaveToJSON(flightPath.string() + "\\" + (activeFlight->GetFlightID() + ".json"));
							g_LogBuf.appendf("Booking Created!\nPassenger Name: %s\n(Row, Col): (%s, %s)\n", nameBuffer, std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str()); // Add Exception Handling
						}
						else
						{
							// Invalid
							g_LogBuf.appendf("Error Booking!\nCheck if seat is available.\n", nameBuffer, g_SelectedRowCol.row, g_SelectedRowCol.col); // Add Exception Handling
						}

					}
				}
				else
				{
					ImGui::Text("No Flight Selected");
				}
				ImGui::End();
			}
		}

		 */

		// Display SeatMap
		static void SeatMapGUI()
		{
			ImGui::Begin("Seat Map", nullptr);
			if (activeFlight != nullptr)
			{
				int labelCounter = 0;
				unsigned short seatLNum = 0;
				for (const auto& row : activeSeatMap->GetVecSeatMap())
				{
					unsigned short seatRNum = 0;
					for (const bool seat : row)
					{
						if (!seat)
						{
							strcpy_s(passengerBuffer, activeFlight->GetPassengerName(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
							strcpy_s(emailBuffer, activeFlight->GetEmail(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
							strcpy_s(maxLoadBuffer, activeFlight->GetMaxLoad(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
						}
						std::string codeText = (seat ? "AVAILABLE" : "TAKEN");
						codeText += " (" + std::to_string(seatLNum) + std::string(",") + std::to_string(seatRNum) + ") " +
							activeFlight->GenerateSeatCode(seatLNum, seatRNum);
						std::string seatCodeGui = "###Btn_Seat_";
						seatCodeGui += std::to_string(labelCounter); // AVAILABLE###Btn_Seat_1 // Label is Btn_Seat_1
						codeText += seatCodeGui;
						if (ImGui::Button(codeText.c_str(),
						                  ImVec2(g_WindowDimensions.x / activeSeatMap->GetVecSeatMap().size(),
						                         g_WindowDimensions.x / activeSeatMap->GetVecSeatMap().size())))
						{
							// Get row and column
							g_SelectedRowCol = { seatLNum, seatRNum };
							g_LogBuf.appendf("Selected seat is %s.\n", std::string(seat ? "AVAILABLE" : "TAKEN").c_str());
						}
						ImGui::SameLine();
						++labelCounter;
						++seatRNum;
					}
					++seatLNum;
					// seatRNum = 0;
					ImGui::Separator();
				}
				seatLNum = 0;
				// labelCounter = 0;
			}
			else
				ImGui::Text("No Flight Selected.");
			ImGui::End();
		}
		// Select a flight
		static void SelectFlight()
		{
			ImGui::Begin("Select Flight", nullptr);

			auto& pmInstance = PersistenceManager::Get();
			// SQLite::Database db("flights.db", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);

			std::vector<FlightInformation*> flightsInformation = pmInstance.GetFlightsInfo(db);
			for (auto* flightInformation : flightsInformation)
			{
				ImGui::Text("Flight ID: %s\nOrigin: %s\nDestination: %s\nDeparture Time: %s\nArrival Time: %s\nRows: %d\nColumns: %d",
					flightInformation->flightID.c_str(),
					flightInformation->origin.c_str(),
					flightInformation->destination.c_str(),
					AirlineReservationSystem::Util::IntToDatetime(flightInformation->departureTime).c_str(),
					AirlineReservationSystem::Util::IntToDatetime(flightInformation->arrivalTime).c_str(),
					flightInformation->rows, flightInformation->cols);
				std::string btnCodeLabel = std::string("Select###btn") + flightId;
				if (ImGui::Button(btnCodeLabel.c_str()))
				{
					delete activeFlight;
					activeFlight = new Flight(flightInformation->flightID, flightInformation->origin,
						flightInformation->destination, flightInformation->departureTime, flightInformation->arrivalTime,
						flightInformation->rows, flightInformation->cols);
					activeFlight->LoadFromFlightInformation(db, *flightInformation);
					activeSeatMap = &activeFlight->GetSeatMap();
					g_LogBuf.appendf("Flight Selected: %s\n", flightId.c_str());
					// fileDialog.ClearSelected();
				}
			}
			ImGui::Separator();
			ImGui::End();

			// Free memory
			for (const auto* flightInformation : flightsInformation)
			{
				delete flightInformation;
			}
		}

		// Manage Reservations (Update and Delete)
		static void ManageBookings()
		{
			ImGui::Begin("Manage Bookings", nullptr);
			if (activeFlight != nullptr)
			{
				ImGui::Text("(Row, Col): (%s, %s) %s", std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str(),
					activeFlight->GenerateSeatCode(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
				if (!activeSeatMap->IsSeatAvailable(g_SelectedRowCol.row, g_SelectedRowCol.col))
				{
					ImGui::Separator();
					ImGui::Text("Seat Information:");
					ImGui::Text("Passenger Name: %s", activeFlight->GetPassengerName(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
					ImGui::Text("Email Address: %s", activeFlight->GetEmail(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
					ImGui::Text("Max Load: %s", activeFlight->GetMaxLoad(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());

					ImGui::InputText("Update Passenger Name", passengerBuffer, 512);
					ImGui::InputText("Update Email Address", emailBuffer, 512);
					ImGui::InputText("Update Passenger Max Load", maxLoadBuffer, 512);

					auto& pmInstance = PersistenceManager::Get();

					if (ImGui::Button("Update", ImVec2(200, 30)))
					{
						activeFlight->CancelBooking(g_SelectedRowCol.row, g_SelectedRowCol.col);
						activeFlight->BookSeat(g_SelectedRowCol.row, g_SelectedRowCol.col, passengerBuffer, emailBuffer, maxLoadBuffer);
						// activeFlight->SaveToJSON(flightPath.string() + "\\" + (activeFlight->GetFlightID() + ".json"));
						pmInstance.InsertBookingData(*activeFlight, db, GenerateBookingTableName());

						g_LogBuf.appendf("Booking Updated!\nPassenger Name: %s\n(Row, Col): (%d, %d)\n", passengerBuffer,
							g_SelectedRowCol.row, g_SelectedRowCol);
					}
					ImGui::SameLine();
					if (ImGui::Button("Delete", ImVec2(200, 30)))
					{
						activeFlight->CancelBooking(g_SelectedRowCol.row, g_SelectedRowCol.col);;
						pmInstance.DeleteBookingData(db, GenerateBookingTableName(), g_SelectedRowCol.row, g_SelectedRowCol.col);
						// activeFlight->SaveToJSON(flightPath.string() + "\\" + (activeFlight->GetFlightID() + ".json"));
						g_LogBuf.appendf("Booking Deleted!\n\n(Row, Col): (%d, %d)\n", g_SelectedRowCol.row, g_SelectedRowCol.col);
					}
				}
				else
				{
					ImGui::Text("Select A Booked Seat");
				}

			}
			else
			{
				ImGui::Text("No Flight Selected");
			}
			ImGui::End();
		}

		// Add reservations
		static void CreateBookings()
		{

			ImGui::Begin("Create Bookings", nullptr);
			if (activeFlight != nullptr)
			{
				static char nameBuffer[512];
				ImGui::InputText("Passenger Name", nameBuffer, sizeof(nameBuffer));
				//ImGui::Text("(Row, Col): (%s, %s)", std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str());
				static char emailBuffer[512];
				ImGui::InputText("Email Address", emailBuffer, sizeof(emailBuffer));
				//ImGui::Text("(Row, Col): (%s, %s)", std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str());
				static char maxLoadBuffer[512];
				ImGui::InputText("Max Load", maxLoadBuffer, sizeof(maxLoadBuffer));
				ImGui::Text("(Row, Col): (%s, %s)", std::to_string(g_SelectedRowCol.row).c_str(),
					std::to_string(g_SelectedRowCol.col).c_str());

				auto& pmInstance = PersistenceManager::Get();

				if (ImGui::Button("Book"))
				{
					if (activeFlight->BookSeat(g_SelectedRowCol.row, g_SelectedRowCol.col, nameBuffer, emailBuffer, maxLoadBuffer))
					{
						// Valid
						pmInstance.InsertBookingData(*activeFlight, db, GenerateBookingTableName());
						g_LogBuf.appendf("Booking Created!\nPassenger Name: %s\n(Row, Col): (%s, %s)\n", nameBuffer, std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str()); // Add Exception Handling
					}
					else
					{
						// Invalid
						g_LogBuf.appendf("Error Booking!\nCheck if seat is available.\n", nameBuffer, g_SelectedRowCol.row, g_SelectedRowCol.col); // Add Exception Handling
					}

				}
			}
			else
			{
				ImGui::Text("No Flight Selected");
			}
			ImGui::End();
		}

		// Define a structure to hold the selected date
		struct Date
		{
			int year = 2024;
			int month = 3;
			int day = 12;
		};

		

		static void AmadeusWindow()
		{

			ImGui::Begin("Amadeus GDS");

			static char originBuffer[512];
			static char destinationBuffer[512];
			static Date departureDate = {};
			static Date arrivalDate = {};
			// departure date and arrival date

			ImGui::InputText("Origin", originBuffer, 512);
			ImGui::InputText("Destination", destinationBuffer, 512);
			// Display the dates
			

			if (ImGui::Button("Select Departure Date"))
			{
				ImGui::OpenPopup("CalendarPopupDepart");
			}
			if (ImGui::BeginPopup("CalendarPopupDepart"))
			{
				// Month selection
				ImGui::Text("Select Month:");
				for (int m = 1; m <= 12; ++m)
				{
					if (ImGui::Selectable(std::to_string(m).c_str(), departureDate.month == m))
					{
						departureDate.month = m;
					}
				}

				// Year selection
				ImGui::Text("Select Year:");
				for (int y = 2000; y <= 2100; ++y)
				{
					if (ImGui::Selectable(std::to_string(y).c_str(), departureDate.year == y))
					{
						departureDate.year = y;
					}
				}

				// Validate day selection based on the selected month and year
				int daysInMonth = 31; // Assume maximum days for simplicity (you can adjust this)
				if (departureDate.month == 2) // February
				{
					daysInMonth = (departureDate.year % 4 == 0 && (departureDate.year % 100 != 0 || departureDate.year % 400 == 0)) ? 29 : 28;
				}
				else if (departureDate.month == 4 || departureDate.month == 6 || departureDate.month == 9 || departureDate.month == 11)
				{
					daysInMonth = 30;
				}

				ImGui::Text("Select Day:");
				for (int d = 1; d <= daysInMonth; ++d)
				{
					if (ImGui::Selectable(std::to_string(d).c_str(), departureDate.day == d))
					{
						departureDate.day = d;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();

			ImGui::Text("Departure Date: %04d-%02d-%02d", departureDate.year, departureDate.month, departureDate.day);
			if (ImGui::Button("Select Arrival Date  "))
			{
				ImGui::OpenPopup("CalendarPopupArrival");
			}

			if (ImGui::BeginPopup("CalendarPopupArrival"))
			{
				// Month selection
				ImGui::Text("Select Month:");
				for (int m = 1; m <= 12; ++m)
				{
					if (ImGui::Selectable(std::to_string(m).c_str(), arrivalDate.month == m))
					{
						arrivalDate.month = m;
					}
				}

				// Year selection
				ImGui::Text("Select Year:");
				for (int y = 2000; y <= 2100; ++y)
				{
					if (ImGui::Selectable(std::to_string(y).c_str(), arrivalDate.year == y))
					{
						arrivalDate.year = y;
					}
				}

				// Validate day selection based on the selected month and year
				int daysInMonth = 31; // Assume maximum days for simplicity (you can adjust this)
				if (arrivalDate.month == 2) // February
				{
					daysInMonth = (arrivalDate.year % 4 == 0 && (arrivalDate.year % 100 != 0 || arrivalDate.year % 400 == 0)) ? 29 : 28;
				}
				else if (arrivalDate.month == 4 || arrivalDate.month == 6 || arrivalDate.month == 9 || arrivalDate.month == 11)
				{
					daysInMonth = 30;
				}

				ImGui::Text("Select Day:");
				for (int d = 1; d <= daysInMonth; ++d)
				{
					if (ImGui::Selectable(std::to_string(d).c_str(), arrivalDate.day == d))
					{
						arrivalDate.day = d;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();

			ImGui::Text("Arrival Date: %04d-%02d-%02d", arrivalDate.year, arrivalDate.month, arrivalDate.day);

			ImGui::Separator();

			char formattedArrivalDate[11]; // Allocate space for "YYYY-MM-DD\0"
			std::sprintf(formattedArrivalDate, "%04d-%02d-%02d", arrivalDate.year, arrivalDate.month, arrivalDate.day);
			std::string arrivalDateString(formattedArrivalDate);

			char formattedDepartureDate[11]; // Allocate space for "YYYY-MM-DD\0"
			std::sprintf(formattedDepartureDate, "%04d-%02d-%02d", arrivalDate.year, arrivalDate.month, arrivalDate.day);
			std::string departureDateString(formattedArrivalDate);

			if (ImGui::Button("Search Flights", ImVec2(g_WindowDimensions.x - 30, 50)))
			{
				auto& amInstance = AmadeusInterface::Get();
				//auto resAvailableFlights = amInstance.GetAvailableFlights("MAD", "PAR", "2024-03-22", "2024-03-30");
				auto resAvailableFlights = amInstance.GetAvailableFlights(originBuffer, destinationBuffer, departureDateString, arrivalDateString);
				std::ofstream file("availableFlights.json");
				if (file.is_open())
				{
					file << resAvailableFlights.dump(4); // Pretty-print JSON
				}

				else
				{
					std::cerr << "Error opening file for writing: " << "availableFlights.json" << std::endl;
				}
				Util::OpenFileWithDefaultApp("availableFlights.json");
			}



			ImGui::End();
		}

	}

	void TestSQLiteCPP()
	{
		 activeFlight = new Flight("AA123", "MAD", "PAR", Util::DatetimeToInt(2024, 3, 15, 0), Util::DatetimeToInt(2024, 3, 30, 0), 4, 3);
		 activeSeatMap = &activeFlight->GetSeatMap();
		activeFlight->BookSeat(1, 1, "Roxanne Cocaine", "Roxanne.ca3.com", "43");
		std::string tableName = "BookingAA123";
		auto& pmInstance = PersistenceManager::Get();
		// SQLite::Database db("flights.db", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
		pmInstance.CreateFlightsTable(db);
		pmInstance.InsertFlight(db, "AA123", "New York", "Los Angeles", "2024-03-15 10:00:00", "2024-03-15 14:00:00", 4, 3);
		pmInstance.CreateBookingTable(db, tableName);
		pmInstance.InsertBookingData(*activeFlight, db, tableName);
		pmInstance.PrintFlights(db);
		pmInstance.PrintBooking(db, tableName);
	}

	void GUIMain()
	{
		auto& guiInstance = WrapGui::GuiApp::Get();
		ImGui::SetNextWindowSize(g_WindowDimensions);
		ImGui::Begin("Airplane Reservation System", guiInstance.GetRunStatePtr(), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);
		// fileDialog.SetTypeFilters({".json"});
		// fileDialog.SetTypeFilters({ ".db" });
		// fileDialog.Display();
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("About"))
				{
					showAbout = true;
				}
				if (ImGui::MenuItem("Exit"))
				{
					guiInstance.Close();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("View Help"))
				{
					showHelp = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		if (activeFlight != nullptr) 
			flightId = activeFlight->GetFlightID();
		ImGui::Text("Flight ID: %s", flightId.c_str());

		ImGui::Separator();

		ImGui::DockSpace(ImGui::GetID("SeatMap"), ImVec2(g_WindowDimensions.x, g_WindowDimensions.y/2.5f));
		ImGui::SetNextWindowDockID(ImGui::GetID("SeatMap"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);
		GUIComponents::SeatMapGUI();

		ImGui::Separator();

		ImGui::DockSpace(ImGui::GetID("Persistence"), ImVec2(g_WindowDimensions.x - 10.0f, g_WindowDimensions.y / 4.0f));
		ImGui::SetNextWindowDockID(ImGui::GetID("Persistence"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);
		GUIComponents::AmadeusWindow();
		ImGui::SetNextWindowDockID(ImGui::GetID("Persistence"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);
		GUIComponents::CreateBookings();
		ImGui::SetNextWindowDockID(ImGui::GetID("Persistence"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);
		GUIComponents::ManageBookings();
		ImGui::SetNextWindowDockID(ImGui::GetID("Persistence"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);
		GUIComponents::SelectFlight();

		ImGui::DockSpace(ImGui::GetID("Logs"));
		ImGui::SetNextWindowDockID(ImGui::GetID("Logs"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);

		ImGui::Begin("Log Window");
		if (ImGui::Button("Clear"))
			g_LogBuf.clear();
		if (ImGui::CollapsingHeader("Logs", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("%s", g_LogBuf.begin());
		}
		ImGui::End();

		if(showAbout)
		{
			ImGui::SetNextWindowDockID(ImGui::GetID("Persistence"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);
			GUIComponents::About();
		}

		if (showHelp)
		{
			ImGui::SetNextWindowDockID(ImGui::GetID("Persistence"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);
			GUIComponents::Help();
		}

		ImGui::End();
	}

}

int main()
{
	// TestSQLiteCPP();
	// Run Gui
	WrapGui::GuiApp::InitGui({ "", 1, 1 });
	auto& guiInstance = WrapGui::GuiApp::Get();
	guiInstance.HideWindow();
	guiInstance.SetMain(GUIMain);
	guiInstance.Run();

	// Free memory
	delete AirlineReservationSystem::activeFlight;
}