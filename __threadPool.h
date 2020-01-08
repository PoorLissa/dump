// Пытаемся разобраться в чужом Thread Pool
// https://habrahabr.ru/post/188234/
//
// and another one to see to: https://github.com/progschj/ThreadPool
// and about c++11 threads:   https://habrahabr.ru/post/182610/
// -----------------------------------------------------------------------------------------------

/*
	// Usage example:
	for (size_t listNo = 0; listNo < VEC.size(); listNo++)
		thPool->runAsync(&Game::threadMonsterMove, this, listNo, playerPosX, playerPosY);

	thPool->waitForAll();   // Ждем, пока все потоки отработают до конца
*/

#pragma once
#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

#define DEBUG_MSG_ON
#undef DEBUG_MSG_ON

#if defined DEBUG_MSG_ON

	extern std::mutex gl_Console_Mutex;

#endif

// -----------------------------------------------------------------------------------------------

// Класс одного потока
class Worker {

	struct workerTask;

	#define setLock(x, lockName) std::unique_lock<std::mutex> lockName(x);

	typedef std::function<void()>		fn_type;
	typedef std::shared_ptr<workerTask> taskPtr;

	struct workerTask {

		workerTask(const fn_type f, const size_t c)
		{
			func = f;
			complexity = c;
		}

		fn_type func;
		size_t  complexity;
	};

	// ----------------------------------------------------------------------------------

	public:

		Worker() : threadEnabled(true), fqueue(), thread(&Worker::thread_fn, this), complexity(0u)
		{
		}

	   ~Worker()
		{
			threadEnabled = false;
			cndVar.notify_one();
			thread.join();
		}

		// Assign new task to the worker thread
		void appendTask(fn_type task_func, size_t task_complexity = 1)
		{
			taskPtr task = getNewTask(task_func, task_complexity);

			setLock(mutex, lck);
				complexity += task_complexity;
				fqueue.push(task);
				cndVar.notify_one();

			#if defined DEBUG_MSG_ON

				setLock(gl_Console_Mutex, console_lock);
					std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: Complexity Increased (" << complexity << ")" << std::endl;

			#endif
		}

		size_t getTaskCount()
		{ 
			setLock(mutex, lck);
				return fqueue.size();
		}

		bool isQueueEmpty()
		{
			setLock(mutex, lck);
				return fqueue.empty();
		}

		// Return total complexity of all worker's tasks
		size_t getWorkerComplexity()
		{
			setLock(mutex, lck);
				return complexity;
		}

	private:

		taskPtr getNewTask(const fn_type &func, const size_t &complexity)
		{
			return taskPtr(new workerTask({ func, complexity } ));
		}

		// Endless loop function executed in this thread
		void thread_fn()
		{
			while( threadEnabled )
			{
				setLock(mutex, lock);
				{
					// Block the current thread until the condition variable is woken up
					// Ожидаем уведомления и убедимся, что это не ложное пробуждение: Поток должен проснуться, если очередь не пустая, либо он выключен
					cndVar.wait(lock, [&]()
										{ 
											return !fqueue.empty() || !threadEnabled;
										}
					);

					// When the thread has woken up:
					while( !fqueue.empty() )
					{
						fn_type fn = fqueue.front()->func;

						lock.unlock();						// Разблокируем мютекс перед вызовом функтора
						{
							fn();							// Call our target function
						}
						lock.lock();						// Возвращаем блокировку снова перед вызовом fqueue.empty()

						complexity -= fqueue.front()->complexity;

						#if defined DEBUG_MSG_ON

							setLock(gl_Console_Mutex, console_lock);
								std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: Complexity Decreased (" << complexity << ")" << std::endl;

						#endif

						fqueue.pop();
					}
				}
			}
		}

	private:

		bool				     threadEnabled;
		std::condition_variable	 cndVar;
		std::queue<taskPtr>		 fqueue;
		std::mutex				 mutex;
		std::thread				 thread;
		size_t					 complexity;
};

// -----------------------------------------------------------------------------------------------

// Класс пула потоков
class ThreadPool {

	typedef std::shared_ptr<Worker> worker_ptr;

	public:

		ThreadPool(size_t threads = 0)
		{
			if( !threads )
				threads = std::thread::hardware_concurrency();

			// Create worker threads and put them into vector
			for(size_t i = 0; i < threads; i++)
			{
				vec_workers.push_back( worker_ptr(new Worker) );
			}
		}

	   ~ThreadPool()
		{
		   ;
		}

	   // Вызвать в отдельном потоке любой внешний метод с любым числом параметров
		template<class _FN, class... _ARGS>
		void runAsync(_FN _fn, _ARGS... _args)
		{
			getFreeWorker()->appendTask(std::bind(_fn, _args...));
		}

		// Вызвать в отдельном потоке любой внешний метод с любым числом параметров (с параметром сложности)
		template<class _FN, class... _ARGS>
		void runAsync_Complexity(size_t N, _FN _fn, _ARGS... _args)
		{
			getFreeWorker()->appendTask(std::bind(_fn, _args...), N);
		}

		// дождаться, пока все потоки отработают все свои задачи
		void waitForAll()
		{
			bool doProceed(false);

			do {

				doProceed = false;

				for( auto &it : vec_workers )
				{
					if( !it->isQueueEmpty() )
					{
						doProceed = true;
						break;
					}
				}
        
			} while (doProceed);

			return;
		}

	private:

		// Функция находит ближайший свободный поток, либо поток, у которого меньше всего сложность, и возвращает его.
		// Доступны варианты при использовании:
		// a) runAsync				-- В простейшем случае сложность равна числу заданий в потоке => Таким образом задания будут распределены между потоками равномерно, но это не гарантирует оптимальное распределение нагрузок.
		// b) runAsync_Complexity	-- В случае с заданной сложностью сложность потока складывается из сложностей всех заданий в потоке. Сложность каждого задания задается пользователем.
		worker_ptr getFreeWorker()
		{
			worker_ptr pWorker;
			size_t min = SIZE_MAX, workerComplexity(0);

			for( auto& it : vec_workers )
			{
				workerComplexity = it->getWorkerComplexity();

				if( workerComplexity == 0u )
				{
					pWorker = it;
					break;
				}
				else
				{
					if( workerComplexity < min )
					{
						min = workerComplexity;
						pWorker = it;
					}
				}
			}

			return pWorker;
		}

	private:

		std::vector<worker_ptr> vec_workers; 
};

// -----------------------------------------------------------------------------------------------

#endif
