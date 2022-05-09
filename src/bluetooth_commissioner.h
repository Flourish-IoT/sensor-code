#pragma once

#include "vector"

#include "./services/base_service.h"

class BluetoothCommissioner 
{
public:
	void startCommissioning()    const;
	int  completeCommissioning() const;

	void setupServices()         const;
	void registerServices()      const;

	int  initialize()            const;
	bool isInitialized()         const;

	int  execute()               const;

	// TODO: unique pointers
	BluetoothCommissioner(std::vector<BaseService *> const services) 
	{
		this->_services = services;
	};

	std::vector<BaseService *> const getServices() const
	{
		return this->_services;
	};

private:
	std::vector<BaseService *> _services;
};