//#include "Airplane.h"
//#include "Flight.h"
//
//
//namespace AirlineReservationSystem
//{
//    Flight::Flight(std::string flightID, Airplane* airplane, std::string from, std::string to) : m_FlightId(flightID),
//        m_Airplane(airplane), m_From(from), m_To(to)
//    {
//
//    }
//    std::string AirlineReservationSystem::Flight::GetFlightID()
//    {
//        return m_FlightId;
//    }
//
//    Airplane* AirlineReservationSystem::Flight::GetAirplane()
//    {
//        return m_Airplane;
//    }
//
//    std::string AirlineReservationSystem::Flight::GetFrom()
//    {
//        return m_From;
//    }
//
//    std::string AirlineReservationSystem::Flight::GetTo()
//    {
//        return m_To;
//    }
//
//    //std::string AirlineReservationSystem::Flight::GetDepartureDateTime()
//    //{
//    //    return m_DepartureDateTime;
//    //}
//
//    //std::string AirlineReservationSystem::Flight::GetArrivalDateTime()
//    //{
//    //    return m_ArrivalDateTime;
//    //}
//
//    void Flight::ReserveFlight(Passenger* passenger, Seat* seat)
//    {
//        seat->SetPassenger(passenger);
//    }
//    void Flight::CancelReservation(Passenger* passenger)
//    {
//        passenger->seat->SetPassenger(nullptr);
//        passenger->seat = nullptr;
//    }
//}
//
