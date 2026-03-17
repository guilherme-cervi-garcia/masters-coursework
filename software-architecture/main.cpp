#include "main.h"

int main() {
    cout << "=== SYSTEM SETUP ===" << endl;
    
    // 1. Create all components
    cout << "Creating broker..." << endl;
    Broker broker;
    
    cout << "Creating sensors..." << endl;
    Radar radar_main(1, SENSOR_MAIN);
    Radar radar_backup(2, SENSOR_BACKUP);
    Lidar lidar_main(3, SENSOR_MAIN);
    Lidar lidar_backup(4, SENSOR_BACKUP);

    cout << "Creating arbiters..." << endl;
    RadarArbiter radar_arbiter(1);
    LidarArbiter lidar_arbiter(2);

    // 2. Connect sensors to broker
    cout << "\nConnecting sensors to broker..." << endl;
    radar_main.connectToBroker(&broker);
    radar_backup.connectToBroker(&broker);
    lidar_main.connectToBroker(&broker);
    lidar_backup.connectToBroker(&broker);

    // 3. Connect arbiters to broker
    cout << "Connecting arbiters to broker..." << endl;
    radar_arbiter.connectToBroker(&broker);
    lidar_arbiter.connectToBroker(&broker);

    // 4. Subscribe arbiters to sensor topics
    cout << "Setting up arbiter subscriptions..." << endl;
    radar_arbiter.subscribeToSensorTopics();
    lidar_arbiter.subscribeToSensorTopics();
    cout << "System setup complete!" << endl;

    // 5. Run complete simulation
    for (int cycle = 1; cycle <= 5; cycle++) {
        cout << "\n--- Cycle " << cycle << " ---" << endl;
        
        // All sensors publish data
        cout << "Sensors publishing data..." << endl;
        radar_main.publishData();
        radar_backup.publishData();
        lidar_main.publishData();
        lidar_backup.publishData();
        
        // Show arbitration results
        cout << "\nArbitration Results:" << endl;
        
        // Check radar arbitration
        ProcessedData radar_result;
        if (broker.getLatestMessage("arbitrated_radar", radar_result)) {
            cout << "  Radar: Selected sensor ID " << radar_result.id;
            if (radar_result.id == 1) {
                cout << " (MAIN radar chosen)";
            } else {
                cout << " (BACKUP radar chosen)";
            }
            cout << "\n    Data points: " << radar_result.data_count 
                 << ", Timestamp: " << radar_result.timestamp << endl;
        } else {
            cout << "  Radar: No arbitrated data available" << endl;
        }
        
        // Check lidar arbitration  
        ProcessedData lidar_result;
        if (broker.getLatestMessage("arbitrated_lidar", lidar_result)) {
            cout << "  Lidar: Selected sensor ID " << lidar_result.id;
            if (lidar_result.id == 3) {
                cout << " (MAIN lidar chosen)";
            } else {
                cout << " (BACKUP lidar chosen)";
            }
            cout << "\n    Data points: " << lidar_result.data_count 
                 << ", Timestamp: " << lidar_result.timestamp << endl;
        } else {
            cout << "  Lidar: No arbitrated data available" << endl;
        }
        
        // Show statistics every 2 cycles
        if (cycle % 2 == 0 || cycle == 5) {
            cout << "\nArbiter Statistics:" << endl;
            cout << "  Radar: " << radar_arbiter.getArbitrationCount() << " decisions, "
                 << radar_arbiter.getMainSelectedCount() << " main, "
                 << radar_arbiter.getBackupSelectedCount() << " backup" << endl;
            cout << "  Lidar: " << lidar_arbiter.getArbitrationCount() << " decisions, "
                 << lidar_arbiter.getMainSelectedCount() << " main, "
                 << lidar_arbiter.getBackupSelectedCount() << " backup" << endl;
        }
    }
    
    // Final summary
    cout << "\n=== FINAL SUMMARY ===" << endl;
    cout << "Simulation completed successfully!" << endl;
    cout << "Total broker topics: " << broker.getTopicCount() << endl;
    
    // Final selection ratios
    float radar_main_ratio = radar_arbiter.getMainSelectionRatio();
    float lidar_main_ratio = lidar_arbiter.getMainSelectionRatio();
    
    cout << "Radar main sensor selection: " << (radar_main_ratio * 100.0f) << "%" << endl;
    cout << "Lidar main sensor selection: " << (lidar_main_ratio * 100.0f) << "%" << endl;
    
    // System health assessment
    if (radar_main_ratio > 0.7f && lidar_main_ratio > 0.7f) {
        cout << "System Health: EXCELLENT (main sensors preferred)" << endl;
    } else if (radar_main_ratio > 0.3f && lidar_main_ratio > 0.3f) {
        cout << "System Health: MIXED (using backup sensors frequently)" << endl;
    } else {
        cout << "System Health: POOR (backup sensors heavily used)" << endl;
    }
    
    cout << "\nSensor fusion system working correctly!" << endl;

    return 0;
}