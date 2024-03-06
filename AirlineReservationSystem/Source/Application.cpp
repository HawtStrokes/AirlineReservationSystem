#include <vector>
#include <iostream>

#include "Gui/GuiApp.h"
#include "imgui.h"
#include "imfilebrowser.h"
#include "fmt/format.h"
#include "HawtLib/Text/TextFormatting.h"

#include "Core/SeatMap.h"
#include "Core/Flight.h"

using namespace AirlineReservationSystem;

namespace AirlineReservationSystem
{
	static ImVec2 g_WindowDimensions = { 800, 1000 };
	static const std::filesystem::path flightPath = std::filesystem::current_path().append("flights");
	
	static bool showAbout = false;
	static bool showHelp = false;
	static SeatMap* activeSeatMap = nullptr;
	static Flight* activeFlight = nullptr;

	static ImGui::FileBrowser fileDialog;
	static ImGui::FileBrowser fileDialogTheme(ImGuiFileBrowserFlags_SelectDirectory);

	static struct { int row, col; } g_SelectedRowCol = {0,0};
	static ImGuiTextBuffer g_LogBuf;
	// static std::vector<Flight*> g_Flights;
	static std::string flightId = "No Flight Selected";
	static char passengerBuffer[512];

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
			// TODO: Provide Wiki page clipboard?
			ImGui::TextWrapped("Sample Text");
			ImGui::End();
		}


		// CORE GUI

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
						if (ImGui::Button(codeText.c_str(), ImVec2(g_WindowDimensions.x / activeSeatMap->GetVecSeatMap().size(), g_WindowDimensions.x / 2.0f / activeSeatMap->GetVecSeatMap().size())))
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
			if (ImGui::Button("Select", ImVec2(g_WindowDimensions.x - 30, 50)))
			{
				delete activeFlight;
				activeFlight = new Flight("", "", 0, 0);
				activeFlight->LoadFromJSON(fileDialog.GetSelected().string());
				activeSeatMap = &activeFlight->GetSeatMap();
			}
			if (fileDialog.HasSelected())
			{
				std::string str = fileDialog.GetSelected().filename().string();
				std::size_t pos = str.find(".json");      // position of "live" in str
				flightId = str.substr(0, pos);     // get from "live" to the end)
			}
			ImGui::End();
		}
		

		// Manage Reservations (Update and Delete)
		static void ManageBookings()
		{
			ImGui::Begin("Manage Bookings", nullptr);
			if (activeFlight != nullptr)
			{
				ImGui::Text("(Row, Col): (%s, %s)", std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str());
				if (!activeSeatMap->IsSeatAvailable(g_SelectedRowCol.row, g_SelectedRowCol.col))
				{
					ImGui::Separator();
					ImGui::Text("Seat Information:");
					// strcpy_s(passengerBuffer, activeFlight->GetPassengerName(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
					ImGui::Text("Passenger Name: %s", activeFlight->GetPassengerName(g_SelectedRowCol.row, g_SelectedRowCol.col).c_str());
					ImGui::InputText("Update Passenger Name", passengerBuffer, 512);
					if (ImGui::Button("Update"))
					{
						std::cout << passengerBuffer;
						activeFlight->CancelBooking(g_SelectedRowCol.row, g_SelectedRowCol.col);
						activeFlight->BookSeat(g_SelectedRowCol.row, g_SelectedRowCol.col, std::string(passengerBuffer));
						activeFlight->SaveToJSON(flightPath.string() + "\\" + (activeFlight->GetFlightNumber() + ".json"));
					}
					if (ImGui::Button("Delete"))
					{
						activeFlight->CancelBooking(g_SelectedRowCol.row, g_SelectedRowCol.col);;
						activeFlight->SaveToJSON(flightPath.string() + "\\" + (activeFlight->GetFlightNumber() + ".json"));
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
				ImGui::Text("(Row, Col): (%s, %s)", std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str());

				if (ImGui::Button("Book"))
				{
					if (activeFlight->BookSeat(g_SelectedRowCol.row, g_SelectedRowCol.col, nameBuffer))
					{
						// Valid
						g_LogBuf.appendf("BOOKED!\nPassenger Name: %s\n(Row, Col): (%s, %s)\n", nameBuffer, std::to_string(g_SelectedRowCol.row).c_str(), std::to_string(g_SelectedRowCol.col).c_str()); // Add Exception Handling
						activeFlight->SaveToJSON(flightPath.string() + "\\" + (activeFlight->GetFlightNumber() + ".json"));
					}
					else
					{
						// Invalid
						g_LogBuf.appendf("ERROR BOOKING!\nCheck if seat is available.\n", nameBuffer, g_SelectedRowCol.row, g_SelectedRowCol.col); // Add Exception Handling
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

	void GUIMain()
	{
		auto& guiInstance = WrapGui::GuiApp::Get();
		ImGui::SetNextWindowSize(g_WindowDimensions);
		ImGui::Begin("Airplane Reservation System", guiInstance.GetRunStatePtr(), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);
		fileDialog.SetTypeFilters({".json"});
		fileDialog.Display();
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
			std::string flightId = activeFlight->GetFlightNumber();
		ImGui::Text("Flight ID: %s", flightId.c_str());

		ImGui::Separator();

		ImGui::DockSpace(ImGui::GetID("SeatMap"), ImVec2(g_WindowDimensions.x, g_WindowDimensions.y/2.5f));
		ImGui::SetNextWindowDockID(ImGui::GetID("SeatMap"), ImGuiCond_FirstUseEver | ImGuiConfigFlags_DockingEnable);
		GUIComponents::SeatMapGUI();

		ImGui::Separator();

		ImGui::DockSpace(ImGui::GetID("Persistence"), ImVec2(g_WindowDimensions.x - 10.0f, g_WindowDimensions.y / 4.0f));
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
	WrapGui::GuiApp::InitGui({ "", 1, 1 });
	auto& guiInstance = WrapGui::GuiApp::Get();
	guiInstance.HideWindow();
	guiInstance.SetMain(GUIMain);
	guiInstance.Run();

	// Free memory
	delete AirlineReservationSystem::activeFlight;
}