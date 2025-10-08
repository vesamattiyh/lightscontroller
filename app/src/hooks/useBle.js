import { useState } from "react";
import { BleManager } from "react-native-ble-plx";

const useBle = () => {
  const manager = new BleManager();

  const [isScanning, setIsScanning] = useState(false);
  const [isConnected, setIsConnected] = useState(false);
  const [peripherals, setPeripherals] = useState([]);
  const [scanTimeout, setScanTimeout] = useState(false);

  const scanDevices = () => {
    setScanTimeout(false);

    const timer = setTimeout(setScanTimeout(true), 5000);

    manager.startDeviceScan(null, null, (error, device) => {
      if (error) {
        // Handle error (scanning will be stopped automatically)
        return;
      }

      // Check if it is a device you are looking for based on advertisement data
      // or other criteria.
      if (device.name === "TI BLE Sensor Tag" || device.name === "SensorTag") {
        // Stop scanning as it's not necessary if you are scanning for one device.
        clearTimeout(timer);
        manager.stopDeviceScan();
        // Proceed with connection.
      }
    });
  };

  const enableBluetooth = () => {};

  const connect = () => {};

  const disconnect = () => {};

  const write = () => {};

  const read = () => {};

  return { scanDevices, connectToDevice, isConnected };
};

// bleManager.stopDeviceScan()

export default useBle;
