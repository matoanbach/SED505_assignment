1. Compare and contrast standard design patterns with machine learning design patterns. Find one similarity and one difference.

- Similarity:
  - Both standard design patterns and machine learning design patterns serve as reusable solutions to common problems in their domains.
  - They provide a set of best practices and templates that developers and engineers can apply to solve design challenges effectively,
    promoting code reusability, modularity, and maintainability.
- Difference:
  - The primary difference lies in their focus and application domains.
  - Standard design patterns are concerned with general engineering and object-oriented programming challenges. They focus on the structure
    and behavior of software systems, providing solutions for class design, object creation, and interaction among components.
  - Machine learning design patterns are specialized for the unique challenges in building and deploying machine learning systems. They address
    issues like data preprocessing, model evaluation, feature engineering, training scalability, model, serving, and ensuring reproducibility and
    fairness in machine learning workflows.

2. In general, do you find machine learning design patterns useful?

- Yes, machine learning are highly useful, especially nowadays when AI is booming. The benefits include:
  - Accelerate deployment
  - Improve maintainability: patterns promote clean, modular, and organized code, making systems easier to understand, test, and maintain.
  - Enhance scalability: allow systems to handle increased data volumes or computational loads effectively.
  - Increase reliability
  - Facilitate collaboration: share a common language and methodology, improving communication among team members and stakeholders

3. Which standard design pattern could you have employed for this assignment? How?

- The Strategy Design Pattern could be an alternative. Because it defines a family of algorithms, encapsulates each one, and makes them interchangeable.
  It allows the algorithms to vary independently from the clients that use it.

  - Define Strategy Interface:

    ```cpp
    class IDrivingStrategy {
    public:
        virtual void adjustSpeedAndDirection(double& speed, double& direction, const CameraData& cameraData, const LidarData& lidarData, PlanningModule& planningModule) = 0;
        virtual ~IDrivingStrategy() = default;
    };
    ```

  - Implement Concrete Strategies:

    - Normal Driving Strategy: Default strategy for normal conditions
    - DefensiveDrivingStrategy: Adjusts behavior for safer, more cautious driving
    - AggressiveDrivingStrategy: Prioritizes speed and efficiency

    ```cpp

    class NormalDrivingStrategy : public IDrivingStrategy {
    public:
        void adjustSpeedAndDirection(double& speed, double& direction, const CameraData& cameraData, const LidarData& lidarData, PlanningModule& planningModule) override {
            // Existing logic from ControlModule::adjustSpeedAndDirection
        }
    };
    ```

    - Modify ControlModule to Use Strategy:

    ```cpp
    class ControlModule {
    private:
        std::unique_ptr<IDrivingStrategy> drivingStrategy;
        // Existing members...
    public:
        void setDrivingStrategy(std::unique_ptr<IDrivingStrategy> strategy) {
            drivingStrategy = std::move(strategy);
        }
        void adjustSpeedAndDirection(const CameraData& cameraData, const LidarData& lidarData, PlanningModule& planningModule) {
            drivingStrategy->adjustSpeedAndDirection(speed, direction, cameraData, lidarData, planningModule);
        }
        // Existing methods...
    };
    ```

    - Usage in AutonomousDrivingSystem:

    ```cpp
    controlModule.setDrivingStrategy(std::make_unique<NormalDrivingStrategy>());
    ```

4. Which other machine learning design pattern could you have employed for this assignment? How?

- We'd use Workflow Pipeline Pattern involving learning workflow as a sequence of steps, where each step process data and passes to the next one.
  This pattern promotes modularity, scalability, and reproducibility in machine learning systems.

- Define Pipeline Steps:
  - Data Acquisition: Collect sensor data (camera, LIDAR, GPS).
  - Data Processing: The PerceptionModule processes raw sensor data into meaningful information.
  - Decision Making: The PlanningModule plans routes and adjusts directions based on processed data.
  - Action Execution: The ControlModule adjusts speed and direction according to the planning output.
  - State Update: Update the vehicle's state (e.g., position via GPSData).
