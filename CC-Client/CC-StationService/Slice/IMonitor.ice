module CCService
{
	/**
	Interface for monitor system info
	**/
	interface IMonitor
	{
		/**
		get system total memory
		**/
		idempotent long getTotalMemory();
		/**
		get current used memory for all process
		**/
		idempotent long getCurrentMemory();
		/**
		get app current memory 
		@param appName the name of the app
		**/
		idempotent long getAppCurrentMemory(string appName);
	};
};