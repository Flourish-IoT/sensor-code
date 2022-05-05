#pragma once

class BaseService 
{
public:
	virtual void registerService()    const = 0;
	virtual void registerAttributes() const = 0;

	virtual int  execute() 
	{
		// empty
	};
	virtual int  initialize()              
	{
		// empty
	};
	virtual bool isInitialized()     const 
	{ 
		return true; 
	};
	virtual void onBLEConnected()    const 
	{
		// empty
	};
	virtual void onBLEDisconnected() const 
	{
		// empty
	};
};
