#include<Windows.h>
#include<memory>

#pragma comment( lib, "winmm.lib")

void main(void){
	int loopCount = 10000000;
	const int avgNum = 50;
	DWORD rawPtrTime = 0, sharedPtrTime = 0, uniquePtrTime = 0; // 各平均時間
	DWORD rawPtrTimes[avgNum], sharedPtrTimes[avgNum], uniquePtrTimes[avgNum];
	DWORD time = timeGetTime();

	int* intPtr = new int(0);
	std::shared_ptr<int> intSharedPtr(new int(0));
	std::unique_ptr<int> intUniquePtr(new int(0));

	// 処理時間算出
	for(int avgLoopCnt = 0; avgLoopCnt < avgNum - 1; avgLoopCnt++){
		// 生ポインタ
		time = timeGetTime();
		for(int i = 0; i < loopCount; ++i){
			*intPtr += 1;
		}
		rawPtrTime += timeGetTime() - time;

		// std::shared_ptr
		time = timeGetTime();
		for(int i = 0; i < loopCount; ++i){
			(*intSharedPtr)++;
		}
		sharedPtrTime += timeGetTime() - time;

		// std::unique_ptr
		time = timeGetTime();
		for(int i = 0; i < loopCount; ++i){
			(*intUniquePtr)++;
		}
		uniquePtrTime += timeGetTime() - time;
	}

	delete intPtr;
	intSharedPtr = nullptr;
	intUniquePtr = nullptr;

	rawPtrTime /= avgNum;
	sharedPtrTime /= avgNum;
	uniquePtrTime /= avgNum;


	printf("raw ptr\t\t : %d\n", rawPtrTime);
	printf("shared ptr\t : %d\n", sharedPtrTime);
	printf("unique ptr\t : %d\n", uniquePtrTime);

	system("pause");

	return;
}