#pragma once

class BaseService
{
public:
	virtual void registerService()    const = 0;
	virtual void registerAttributes() const = 0;

	virtual int  execute()
	{
		return 0;
	};
	virtual int  initialize()
	{
		return 0;
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
