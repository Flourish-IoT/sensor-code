#pragma once

namespace BluetoothOperations
{
	void startBle();

	void registerBleServices();
	void onBLEConnected(BLEDevice const central);
	void onBLEDisconnected(BLEDevice const central);

	void setupServices();
	void executeServices();

	void initializeServices();
	bool servicesInitialized();
}