


#ifndef TIMER_H
#define TIMER_H
class Timer
{
public:
	Timer();

	
	/***********************
	* GetGameTime: Return the Current game time - not including paused time
	* @author: Jc Fowles
	* @return: float: The Current Game Time in seconds
	********************/
	float GetGameTime()const;  
	
	/***********************
	* GetDeltaTime: TO DO:
	* @author: Jc Fowles
	* @return: float: 
	********************/
	float GetDeltaTime()const; // in seconds

	
	/***********************
	* Reset: Reset the timer - Call before message loop.
	* @author: Jc Fowles
	* @return: void: 
	********************/
	void Reset(); 
	
	/***********************
	* Start: Start the timer - Call when unpaused.
	* @author: Jc Fowles
	* @return: void: 
	********************/
	void Start(); 
	
	/***********************
	* Stop: Stop the timer - // Call when paused.
	* @author: Jc Fowles
	* @return: void: 
	********************/
	void Stop(); 
	
	/***********************
	* Tick: Calculate delta tick (time taken since last frame) - Call every frame. 
	* @author: Jc Fowles
	* @return: void: 
	********************/
	void Tick(); 

private:
	double m_SecondsPerCount;
	double m_DeltaTime;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PreviousTime;
	__int64 m_CurrentTime;

	bool m_Stopped;
};

#endif