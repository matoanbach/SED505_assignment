// AutoDrivingSystem.cpp - an autonomous driving system

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

using namespace std;

// Sensor Data Structures
const double earthCircumference = 40040.00;

struct CameraData {
    // Camera-specific data
    enum class ObjectType {
        none,
        vehicle,
        pedestrian,
        bicycle,
        stopLight,
        speedLimit
    };
    ObjectType object = ObjectType::none;
};

struct LidarData {
    // LIDAR-specific data
    enum class ObjectType {
        roadCurvature,
        smallObstruction,
        largeObstruction,
    };
    ObjectType object = ObjectType::roadCurvature;
};

struct GPSData {
    // GPS-specific data
    double curr_longitude;
    double curr_latitude;
    double dest_longitude;
    double dest_latitude;
};

// Haversine function to calculate the distance between two GPS coordinates
// Link for reference: https://community.esri.com/t5/coordinate-reference-systems-blog/distance-on-a-sphere-the-haversine-formula/ba-p/902128
double haversine(double lat1, double lon1, double lat2, double lon2) {
    // Radius of Earth in km
    double R = 6371.0;

    // Convert latitudes and longitudes from degrees to radians
    lat1 = lat1 * M_PI / 180.0;
    lon1 = lon1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;
    lon2 = lon2 * M_PI / 180.0;

    // Differences
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    // Haversine formula
    double a = sin(dlat / 2) * sin(dlat / 2) +
        cos(lat1) * cos(lat2) *
        sin(dlon / 2) * sin(dlon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    // Distance
    double distance = R * c;

    return distance; // in km
}

// Function to calculate bearing from current location to destination
// formula: bearing=atan2(sin(Δlon)⋅cos(lat2),cos(lat1)⋅sin(lat2)−sin(lat1)⋅cos(lat2)⋅cos(Δlon))
double calculateBearing(double lat1, double lon1, double lat2, double lon2) {
    // Convert latitudes and longitudes from degrees to radians
    lat1 = lat1 * M_PI / 180.0;
    lon1 = lon1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;
    lon2 = lon2 * M_PI / 180.0;

    double dLon = lon2 - lon1;

    double x = sin(dLon) * cos(lat2);
    double y = cos(lat1) * sin(lat2) -
        sin(lat1) * cos(lat2) * cos(dLon);

    double initial_bearing = atan2(x, y);

    // Convert from radians to degrees
    initial_bearing = initial_bearing * 180.0 / M_PI;

    // Normalize to 0-360
    initial_bearing = fmod((initial_bearing + 360.0), 360.0);

    return initial_bearing;
}

// PERCEPTION MODULE - processes camera, lidar and GPS data
class PerceptionModule {
    int timer = 0; // increments once per hour
    const int MAX_TIME = 6;
public:
    void processCameraData(CameraData& cameraData);
    void processLidarData(LidarData& lidarData);
    void processGPSData(GPSData& gpsData, double speed, double direction); // update gps data
    void incrementTime() {
        ++timer;
        timer = timer % MAX_TIME;
    }
};

void PerceptionModule::processCameraData(CameraData& cameraData) {
    switch (timer) {
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

void PerceptionModule::processLidarData(LidarData& lidarData) {
    switch (timer) {
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

// Spherical trigonometry (Haversine-like approach)
void PerceptionModule::processGPSData(GPSData& gpsData, double speed, double direction) {
    // Earth's radius in km
    const double R = 6371.0;

    // Distance traveled in km
    double distance = speed * 1.0; // Speed is in km/h, time is 1 hour

    // Convert to radians
    double lat1 = gpsData.curr_latitude * M_PI / 180.0;
    double lon1 = gpsData.curr_longitude * M_PI / 180.0;
    double bearing = direction * M_PI / 180.0;

    double lat2 = asin(sin(lat1) * cos(distance / R) +
        cos(lat1) * sin(distance / R) * cos(bearing));

    double lon2 = lon1 + atan2(sin(bearing) * sin(distance / R) * cos(lat1),
        cos(distance / R) - sin(lat1) * sin(lat2));

    // Convert back to degrees
    gpsData.curr_latitude = lat2 * 180.0 / M_PI;
    gpsData.curr_longitude = lon2 * 180.0 / M_PI;
}

// PLANNING MODULE- plans a route based on GPS data and obstacle information (LIDAR data)
class PlanningModule {
private:
    double plannedDirection;
public:
    void planRoute(const GPSData& gpsData);
    double getPlannedDirection() const;
    void updateRoute(const CameraData& cameraData, const LidarData& lidarData);
};

void PlanningModule::planRoute(const GPSData& gpsData) {
    // Calculate initial planned direction towards the destination
    plannedDirection = calculateBearing(gpsData.curr_latitude, gpsData.curr_longitude,
                                        gpsData.dest_latitude, gpsData.dest_longitude);
}

double PlanningModule::getPlannedDirection() const {
    return plannedDirection;
}

void PlanningModule::updateRoute(const CameraData& cameraData, const LidarData& lidarData) {
    // Adjust the planned direction based on LIDAR data
    if (lidarData.object == LidarData::ObjectType::largeObstruction) {
        // Reroute to avoid large obstruction
        plannedDirection += 20.0; // change direction by 20 degrees
    } else if (lidarData.object == LidarData::ObjectType::smallObstruction) {
        // Slight adjustment
        plannedDirection += 10.0; // change direction by 10 degrees
    } else if (lidarData.object == LidarData::ObjectType::roadCurvature) {
        // Adjust for road curvature
        plannedDirection += 5.0; // change direction by 5 degrees
    }

    // Adjust the planned direction based on camera data
    if (cameraData.object == CameraData::ObjectType::stopLight) {
        // Simulate waiting at stoplight
        // No change to planned direction, but in real implementation, might need to handle timing
    }

    // Keep plannedDirection within 0-360 degrees
    plannedDirection = fmod(plannedDirection + 360.0, 360.0);
}

// CONTROL MODULE - this module brings together gps data, sensor data, and planning data
class ControlModule {
private:
    GPSData gpsData;
    double speed;     // in km/h
    double direction; // in degrees (0-360)
    int hoursDriven;
public:
    void initGPSData(double lat1, double lon1, double lat2, double lon2);
    void adjustSpeedAndDirection(const CameraData& cameraData, const LidarData& lidarData, PlanningModule& planningModule);
    bool hasArrived();
    GPSData& getGPSData() { return gpsData; }
    double getSpeed() const { return speed; }
    double getDirection() const { return direction; }
    int getHoursDriven() const { return hoursDriven; }
    void incrementHoursDriven() { ++hoursDriven; }
};

void ControlModule::initGPSData(double lat1, double lon1, double lat2, double lon2) {
    gpsData.curr_latitude = lat1;
    gpsData.curr_longitude = lon1;
    gpsData.dest_latitude = lat2;
    gpsData.dest_longitude = lon2;
    speed = 80.0; // default speed
    direction = 0.0;
    hoursDriven = 0;
}

void ControlModule::adjustSpeedAndDirection(const CameraData& cameraData, const LidarData& lidarData, PlanningModule& planningModule) {
    // Adjust speed based on camera data
    switch (cameraData.object) {
    case CameraData::ObjectType::none:
        speed = 80.0; // default speed
        break;
    case CameraData::ObjectType::vehicle:
        speed = 60.0;
        break;
    case CameraData::ObjectType::pedestrian:
        speed = 30.0;
        break;
    case CameraData::ObjectType::bicycle:
        speed = 40.0;
        break;
    case CameraData::ObjectType::stopLight:
        speed = 0.0; // stop at stoplight
        break;
    case CameraData::ObjectType::speedLimit:
        speed = 50.0;
        break;
    default:
        speed = 80.0;
    }

    // Get planned direction from planning module
    double plannedDirection = planningModule.getPlannedDirection();

    // Set the vehicle's direction to the planned direction
    direction = plannedDirection;
}

bool ControlModule::hasArrived() {
    double distance = haversine(gpsData.curr_latitude, gpsData.curr_longitude,
        gpsData.dest_latitude, gpsData.dest_longitude);

    return distance <= 25.0;
}

// Autonomous Driving System - starts, runs and stops the simulation
class AutonomousDrivingSystem {
public:
    void initGpsData(double lat1, double lon1, double lat2, double lon2);
    void runSimulation();
    ControlModule& getControlModule() { return controlModule; }

private:
    PerceptionModule perceptionModule;
    PlanningModule planningModule;
    ControlModule controlModule;

    CameraData cameraData;
    LidarData lidarData;
};

void AutonomousDrivingSystem::initGpsData(double lat1, double lon1, double lat2, double lon2) {
    controlModule.initGPSData(lat1, lon1, lat2, lon2);
    planningModule.planRoute(controlModule.getGPSData());
}

void AutonomousDrivingSystem::runSimulation() {
    // Initial output
    GPSData& gpsData = controlModule.getGPSData();
    double distance = haversine(gpsData.curr_latitude, gpsData.curr_longitude,
        gpsData.dest_latitude, gpsData.dest_longitude);
    double bearing = planningModule.getPlannedDirection();

    cout << "You are at " << gpsData.curr_longitude << " long, " << gpsData.curr_latitude << " lat. You want to be at "
        << gpsData.dest_longitude << " long, " << gpsData.dest_latitude << " lat. You are " << distance << "km away from your destination.\n";
    cout << "You need to travel at " << bearing << " degrees.\n\n";

    while (!controlModule.hasArrived() && controlModule.getHoursDriven() < 24) {
        // PERCEPTION
        perceptionModule.incrementTime(); //  Increment the perception module timer

        // PERCEPTION
        perceptionModule.processCameraData(cameraData); // process CAMERA
        perceptionModule.processLidarData(lidarData);   // process LIDAR

        // PLANNING 
        planningModule.updateRoute(cameraData, lidarData); // Update the route based on new sensor data

        // CONTROL
        controlModule.adjustSpeedAndDirection(cameraData, lidarData, planningModule); // Adjust speed and direction based on sensor data and planning

        // PERCEPTION
        perceptionModule.processGPSData(controlModule.getGPSData(), controlModule.getSpeed(), controlModule.getDirection()); // Update GPS data

        // Output status
        GPSData& gpsData = controlModule.getGPSData();
        double distance = haversine(gpsData.curr_latitude, gpsData.curr_longitude,
            gpsData.dest_latitude, gpsData.dest_longitude);
        double bearing = planningModule.getPlannedDirection();

        cout << "You are at " << gpsData.curr_longitude << " long, " << gpsData.curr_latitude << " lat. You want to be at "
            << gpsData.dest_longitude << " long, " << gpsData.dest_latitude << " lat. You are " << distance << "km away from your destination.\n";
        cout << "You need to travel at " << bearing << " degrees.\n\n";

        // Increase the hours driven
        controlModule.incrementHoursDriven();
    }
}

int main() {
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

    // Run the simulation
    autonomousDrivingSystem.runSimulation();

    cout << "You have arrived! (close enough)." << endl;

    return 0;
}
