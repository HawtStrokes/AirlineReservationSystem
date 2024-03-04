#include <vector>
#include <iostream>

#include "Gui/GuiApp.h"
#include "imgui.h"
#include "fmt/format.h"
#include "HawtLib/Text/TextFormatting.h"

#include "Core/SeatMap.h"
#include "Core/Flight.h"

using namespace AirlineReservationSystem;

namespace AirlineReservationSystem
{
	static ImVec2 g_WindowDimensions = { 800, 1000 };
	
	// static ImGuiTextBuffer g_LogBuf;
	
	static bool showAbout = false;
	// static bool showHelp = false;
	static SeatMap* activeSeatMap = nullptr;
	static Flight* activeFlight = nullptr;

	static struct { int row, col; } g_SelectedRowCol = {0,0};
	static ImGuiTextBuffer g_LogBuf;
	// static std::vector<Flight*> g_Flights;

	// static std::string selectedSeatCode = "";

	namespace GUIComponents
	{

		static void About()
		{
			ImGui::Begin("About", &showAbout);
			ImGui::TextWrapped("AirlineReservationSystem v0.1a");
			ImGui::End();
		}
		/*static void Help()
		{
			ImGui::Begin("Help", &showHelp);
			ImGui::TextWrapped("Sample Text");
			ImGui::End();
		}*/
		static void SeatMapGUI() 
		{
			
			ImGui::Begin("Seat Map", nullptr);

			int labelCounter = 0;
			int seatLNum = 0;
			for (const auto& row : activeSeatMap->GetVecSeatMap())
			{
				int seatRNum = 0;
				for (bool seat : row)
				{	
					std::string codeText = (seat ? "AVAILABLE" : "TAKEN");
					codeText += " (" + std::to_string(seatLNum) + std::string(",") + std::to_string(seatRNum) + ")";
					std::string seatCodeGui = "###Btn_Seat_";
					seatCodeGui += std::to_string(labelCounter); // AVAILABLE###Btn_Seat_1 // Label is Btn_Seat_1
					codeText += seatCodeGui;
					if (ImGui::Button(codeText.c_str(), ImVec2(g_WindowDimensions.x/activeSeatMap->GetVecSeatMap().size(), g_WindowDimensions.x / 2.0f / activeSeatMap->GetVecSeatMap().size())))
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
			ImGui::End();
			
		}

		static void Bookings()
		{

			ImGui::Begin("Bookings", nullptr);
			
			// Todo: List all flights after selecting load the flights and update seatmap



			static char nameBuffer[512];
			ImGui::InputText("Passenger Name", nameBuffer, sizeof(nameBuffer));
			ImGui::Text("(Row, Col): (%s, %s)", std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str());
			
			/*static char rowBuffer[512];
			/*ImGui::InputText("Seat Row", rowBuffer, sizeof(rowBuffer));
			static char colBuffer[512];
			ImGui::InputText("Seat Col", colBuffer, sizeof(colBuffer));*/
			if (ImGui::Button("Book"))
			{
				if (activeFlight->BookSeat(g_SelectedRowCol.row, g_SelectedRowCol.col, nameBuffer))
				{
					// Valid
					g_LogBuf.appendf("BOOKED!\nPassenger Name: %s\n(Row, Col): (%s, %s)\n", nameBuffer, std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str()); // Add Exception Handling
					activeFlight->SaveToJSON(activeFlight->GetFlightNumber() + ".json");
				}
				else
				{
					// Invalid
					g_LogBuf.appendf("ERROR BOOKING!\nCheck if seat is available.\n", nameBuffer, g_SelectedRowCol.row, g_SelectedRowCol.col); // Add Exception Handling
				}
				
			}
			// Auto save

			ImGui::End();

		}

	}

	void GUIMain()
	{
		auto& guiInstance = WrapGui::GuiApp::Get();

		ImGui::SetNextWindowSize(g_WindowDimensions);
		ImGui::Begin("Airplane Reservation System", guiInstance.GetRunStatePtr(), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);
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
		/*	if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("View Help"))
				{
					showHelp = true;
				}
				ImGui::EndMenu();
			}*/
			ImGui::EndMenuBar();
		}

		std::string flightId = activeFlight->GetFlightNumber();
		ImGui::Text("Flight ID: %s", flightId.c_str());

		ImGui::Separator();

		ImGui::DockSpace(ImGui::GetID("SeatMap"), ImVec2(g_WindowDimensions.x, g_WindowDimensions.y/2.5f));
		ImGui::SetNextWindowDockID(ImGui::GetID("SeatMap"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);
		GUIComponents::SeatMapGUI();

		ImGui::Separator();

		ImGui::DockSpace(ImGui::GetID("Persistence"), ImVec2(g_WindowDimensions.x - 10.0f, g_WindowDimensions.y / 4.0f));
		ImGui::SetNextWindowDockID(ImGui::GetID("Persistence"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);
		GUIComponents::Bookings();

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

	/*	if (showHelp)
		{
			ImGui::SetNextWindowDockID(ImGui::GetID("Persistence"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);
			GUIComponents::Help();
		}*/

		ImGui::End();
	}
}

int main()
{
	Flight flight("AA123", "New York", 5, 4);
	activeFlight = &flight;
	activeSeatMap = &flight.GetSeatMap();

	// g_Flights.push_back(flight);

	// Book seats
	flight.BookSeat(2, 1, "Alice");
	flight.BookSeat(4, 2, "Bob");

	// Display flight info
	flight.DisplayInfo();

	// Retrieve passenger name
	int rowToCheck = 2;
	int colToCheck = 1;
	std::string passengerName = flight.GetPassengerName(rowToCheck, colToCheck);
	if (!passengerName.empty())
	{
		std::cout << "Passenger at seat (" << rowToCheck << ", " << colToCheck << "): " << passengerName << std::endl;
	}
	else
	{
		std::cout << "No passenger at seat (" << rowToCheck << ", " << colToCheck << ")" << std::endl;
	}

	// Save booking information to a JSON file
	flight.SaveToJSON(flight.GetFlightNumber() + ".json");

	// Load booking information from the JSON file
	Flight loadedFlight("AA123", "New York", 0, 0); // Initialize with dummy values
	flight.LoadFromJSON(flight.GetFlightNumber() + ".json");
	loadedFlight.DisplayInfo();

	WrapGui::GuiApp::InitGui({ "", 1, 1 });
	auto& guiInstance = WrapGui::GuiApp::Get();
	guiInstance.HideWindow();
	guiInstance.SetMain(GUIMain);
	guiInstance.Run();
}