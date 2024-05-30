// Dining philosophers problem.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <chrono>
#include <Etuni/Mutex.h>
#include <Etuni/Thread.h>
//using namespace std;
using namespace Etuni;


/// <summary>
/// 筷子數組
/// </summary>
int chopstick[5] = { 1, 1, 1, 1, 1 };

/// <summary>
/// 定義一個長度為5的Mutex數組
/// </summary>
Mutex mutexArr[5];

ConditionVariable philosopher_cv;    //條件變量, 哲學家執行緒的管理隊列

/// <summary>
/// 拿起筷子的互斥量, 初始值為1
/// </summary>
int get_cpstk_mutex = 1;

/// <summary>
/// 拿起筷子的互斥信號量
/// </summary>
Mutex get_cpstk_mtx;

/// <summary>
/// 哲學家函數
/// </summary>
void philosopher_fun(int index) {
	while (true)
	{	
		/*std::unique_lock<std::mutex> lock(get_cpstk_mtx);
		philosopher_cv.wait(lock, []() {return get_cpstk_mutex = 1; });*/
		ETUNI_GET_MUTEX_LOCK(lockini, get_cpstk_mtx);
		get_cpstk_mutex = 0;

		//申請左手邊的筷子
		/*std::unique_lock<Mutex> left_lock(mutexArr[index]);
		philosopher_cv.Wait(left_lock, [&]() { return chopstick[index] == 1; }); */    //若左邊的筷子數量為1, 則當前執行緒可以繼續執行
		ETUNI_GET_MUTEX_LOCK(left_lock, mutexArr[index]);
		chopstick[index] = 0;
		/*printf("哲學家%d申請左手邊的筷子\n", index);*/

		//申請右手邊的筷子
		/*std::unique_lock<Mutex> right_lock(mutexArr[(index + 1) % 5]);
		philosopher_cv.Wait(right_lock, [&]() { return chopstick[(index + 1) % 5] == 1; });*/     //若右邊的筷子數量為1, 則當前執行緒可以繼續執行
		ETUNI_GET_MUTEX_LOCK(right_lock, mutexArr[(index + 1) % 5]);
		chopstick[(index + 1) % 5] = 0;
		/*printf("哲學家%d申請右手邊的筷子\n", index);*/

		get_cpstk_mutex = 1;
		philosopher_cv.NotifyAll();

		printf("哲學家%d進餐\n", index);

		//歸還左手邊的筷子
		chopstick[index] = 1;
		philosopher_cv.NotifyAll();

		//歸還右手邊的筷子
		chopstick[(index + 1) % 5] = 1;
		philosopher_cv.NotifyAll();

		//ThisThread::Sleep(1000);   //當前執行緒暫停1秒

		printf("哲學家%d思考\n", index);
		ThisThread::Sleep(1000);   //當前執行緒暫停1秒
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	Thread philosopher_thread[5];
 
	for (int i = 0; i < 5; i++){
		philosopher_thread[i].StartLoop("philosopher"+std::to_string(i), boost::bind(philosopher_fun, i));
		philosopher_thread[i].Abort();
	}

	for (int i = 0; i < 5; i++){
		philosopher_thread[i].Join();
	}

	return 0;
}

