// AutoDrivingSystem.cpp - an autonomous driving system

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

// Sensor Data Structures
const double earthCircumference = 40040.00;
struct CameraData
{
    // Camera-specific data
    enum class ObjectType
    {
        none,
        vehicle,
        pedestrian,
        bicycle,
        stopLight,
        speedLimit
    };
    ObjectType object;
};

struct LidarData
{
    // LIDAR-specific data
    enum class ObjectType
    {
        roadCurvature,
        smallObstruction,
        largeObstruction,
    };
    ObjectType object;
};

struct GPSData
{
    // GPS-specific data
    double curr_longitude;
    double curr_latitude;
    double dest_longitude;
    double dest_latitude;
};

// Perception Module - processes camera, lidar and GPS data
/*
    Description:
        - Process camera, LIDAR, and GPS data for object and obstacle detection once per hour
        - Camera's data adjust the vehicle's speed
        - Lidar's data adjust the vehicle's direction
*/
class PerceptionModule
{
    int timer = 0; // increments once per hour
    const int MAX_TIME = 6;

public:
    void processCameraData(CameraData &cameraData);
    void processLidarData(LidarData &lidarData);
    void processGPSData(GPSData &gpsData, double speed, double direction); // update gps data
    void incrementTime()
    {
        ++timer;
        timer = timer % MAX_TIME;
    }
};

void PerceptionModule::processCameraData(CameraData &cameraData)
{
    switch (timer)
    {
    case 0:
        cameraData.object = CameraData::ObjectType::none;
        break;
    case 1:
        cameraData.object = CameraData::ObjectType::vehicle;
        break;
    case 2:
        cameraData.object = CameraData::ObjectType::pedestrian;
        break;
    case 3:
        cameraData.object = CameraData::ObjectType::bicycle;
        break;
    case 4:
        cameraData.object = CameraData::ObjectType::stopLight;
        break;
    case 5:
        cameraData.object = CameraData::ObjectType::speedLimit;
        break;
    default:
        cameraData.object = CameraData::ObjectType::none;
    }
}

void PerceptionModule::processLidarData(LidarData &lidarData)
{
    switch (timer)
    {
    case 0:
    case 3:
        lidarData.object = LidarData::ObjectType::roadCurvature;
        break;
    case 1:
    case 4:
        lidarData.object = LidarData::ObjectType::smallObstruction;
        break;
    case 2:
    case 5:
        lidarData.object = LidarData::ObjectType::largeObstruction;
        break;
    default:
        lidarData.object = LidarData::ObjectType::roadCurvature;
    }
}

void PerceptionModule::processGPSData(GPSData &gpsData, double speed, double direction)
{
    // Process GPS data for localization, mapping, etc.
    gpsData.curr_longitude += 180.0 * speed * sin(M_PI / 180.0 * direction) / earthCircumference;
    gpsData.curr_latitude += 180.0 * speed * cos(M_PI / 180.0 * direction) / earthCircumference;
}

// Planning Module - plans a route based on GPS data and obstacle information (LIDAR data)
/*
    Description:
        - Plan the driving route based on the current GPS and the destination GPS data
        - Update the route based on the camera data and LIDAR data
*/
class PlanningModule
{

public:
    // write a function to plan your route based on current and destination gps data
    // write a function to update your route based on camera and lidar data
};

// Control Module - this module brings together gps data, sensor data, and planning data
/*
    Description:
        - Initialize GPS data (current and destination)
        - Process CAMERA, LIDAR, and GPS data through the perception module
        - Plan and update the route through the planning module
*/
class ControlModule
{

public:
    // write a function to initialize GPS data

    // write a function to adjust speed and direction based on:
    //- the processing of sensor data
    //- the planning of your route
    //- the updating of your route

    // end your journey after 24 hours or within 25km of your destination
};

// Autonomous Driving System - starts, runs and stops the simulation
class AutonomousDrivingSystem
{

public:
    // write a function to initialize data
    // write a function to run the overall simulation
};

int main()
{
    // Create an autonomous driving system
    AutonomousDrivingSystem autonomousDrivingSystem;
    double latitude1, latitude2;
    double longitude1, longitude2;

    cout << "What is your initial location (latitude): ";
    cin >> latitude1;
    cout << "                             (longitude): ";
    cin >> longitude1;

    cout << "What is your destination location (latitude): ";
    cin >> latitude2;
    cout << "                                 (longitude): ";
    cin >> longitude2;

    autonomousDrivingSystem.initGpsData(latitude1, longitude1, latitude2, longitude2);

    // Process the sensor data
    autonomousDrivingSystem.runSimulation();

    cout << "You have arrived! (close enough)" << endl;

    return 0;
}